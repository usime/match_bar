from __future__ import annotations

from pathlib import Path
from typing import List, Sequence, Tuple, Optional
import heapq  # 用于高效维护 Top-K

import dask.array as da
import matplotlib.pyplot as plt
import numpy as np
import torch
import zarr
from monai.losses import GlobalMutualInformationLoss
from torch import nn
import tifffile

# ================================================================= #
#                           核心配置区域 (针对 2048x2048 优化)
# ================================================================= #

# ----------------- 1. 区域选择 ----------------- #
USE_ROI: bool = True

# 指定配准范围
ROI_ROW_START: int = 20
ROI_ROW_END: int = 21
ROI_COL_START: int = 40
ROI_COL_END: int = 41

# ----------------- 2. 配准参数 (2048分辨率特调) ----------------- #
# 金字塔策略: [64x64, 128, 256, 512, 1024, 2048]
# 在低分辨率层(64x64)多迭代几次(100次)，计算量小且能纠正大位移
# 在高分辨率层(2048)少迭代(10次)，主要做亚像素微调，节省显存和时间
PYRAMID_ITERS: List[int] = [80,40,20,10]

# 学习率: 2048大图可能需要更大的步长来移动像素，推荐 1e-4 或 5e-5
LR: float = 1e-4

# 平滑正则: 分辨率越高，局部噪点越多，建议适当调高约束，防止网格撕裂
LAMBDA_SMOOTH: float = 1.0

EPS: float = 1e-6
MI_NUM_BINS: int = 64
TOP_K_SHOW: int = 3

# ----------------- 3. 路径配置 ----------------- #
PATCH_ZARR_S2 = "s2_patches.zarr"  # Moving
PATCH_ZARR_S1 = "s1_patches.zarr"  # Fixed
OUT_ZARR_S2 = "s2_registered_patches.zarr"  # Output

# 设备配置
DEVICE = torch.device("cuda" if torch.cuda.is_available() else "cpu")
print(f"运行设备: {DEVICE}")


# ================================================================= #


def open_zarr_array(path: str) -> da.Array:
    if not Path(path).exists():
        raise FileNotFoundError(f"未找到 zarr: {path}. 请先运行切割脚本生成滑窗 patches")
    return da.from_zarr(path)


def to_gray_s1(s1_patch: np.ndarray) -> np.ndarray:
    """S1(3通道) 转灰度。智能处理 HWC 或 CHW 格式。"""
    if s1_patch.ndim == 2:
        return s1_patch
    if s1_patch.shape[-1] <= 4:  # HWC
        return s1_patch.mean(axis=-1)
    if s1_patch.shape[0] <= 4:  # CHW
        return s1_patch.mean(axis=0)
    return s1_patch.mean(axis=-1)


def robust_normalize(img: np.ndarray, p_low: float = 1.0, p_high: float = 99.0) -> np.ndarray:
    """基于分位数的强度归一化。"""
    img_f = img.astype(np.float32, copy=False)
    lo = np.percentile(img_f, p_low)
    hi = np.percentile(img_f, p_high)
    if hi - lo < EPS:
        return np.zeros_like(img_f, dtype=np.float32)
    img_f = np.clip(img_f, lo, hi)
    img_f = (img_f - lo) / (hi - lo + EPS)
    return img_f


def build_pyramid(img: torch.Tensor, max_levels: int, verbose: bool = False) -> List[torch.Tensor]:
    """自适应金字塔构建。"""
    pyr = [img]
    if verbose:
        print(f"  [Pyramid] Level 0 (Original): {tuple(img.shape)}")

    # 动态构建金字塔，直到达到 max_levels 或图像太小
    for i in range(1, max_levels):
        _, _, h, w = img.shape
        if h < 8 or w < 8:  # 2048图没必要缩到2x2，缩到8x8或16x16足够了
            if verbose:
                print(f"  [Pyramid] Stop: Image too small ({h}x{w}) for next level.")
            break
        img = nn.functional.avg_pool2d(img, kernel_size=2, stride=2, padding=0)
        pyr.append(img)
        if verbose:
            print(f"  [Pyramid] Level {i} (Downsampled): {tuple(img.shape)}")

    if verbose:
        print(f"  [Pyramid] Construction Complete. Total Levels: {len(pyr)}")
    return pyr[::-1]


def gradient_loss(disp: torch.Tensor) -> torch.Tensor:
    dy = torch.abs(disp[:, :, 1:, :] - disp[:, :, :-1, :])
    dx = torch.abs(disp[:, :, :, 1:] - disp[:, :, :, :-1])
    return dx.mean() + dy.mean()


def _make_base_grid(h: int, w: int, device: torch.device) -> torch.Tensor:
    grid_y, grid_x = torch.meshgrid(
        torch.linspace(-1, 1, h, device=device),
        torch.linspace(-1, 1, w, device=device),
        indexing="ij",
    )
    return torch.stack((grid_x, grid_y), dim=-1).unsqueeze(0)


def _warp(moving: torch.Tensor, disp: torch.Tensor) -> torch.Tensor:
    _, _, h, w = moving.shape
    base_grid = _make_base_grid(h, w, moving.device)
    grid = base_grid + disp.permute(0, 2, 3, 1)
    return nn.functional.grid_sample(
        moving, grid, mode="bicubic", padding_mode="border", align_corners=True
    )


def compute_mi(fixed: torch.Tensor, warped: torch.Tensor, mi_loss_fn: GlobalMutualInformationLoss) -> float:
    with torch.no_grad():
        mi = -mi_loss_fn(warped, fixed).item()
    return mi


def get_pixel_displacement_stats(disp: np.ndarray, h: int, w: int) -> Tuple[float, float]:
    disp_x_px = disp[0, 0, :, :] * (w / 2.0)
    disp_y_px = disp[0, 1, :, :] * (h / 2.0)
    magnitude = np.sqrt(disp_x_px ** 2 + disp_y_px ** 2)
    return float(magnitude.mean()), float(magnitude.max())


def register_patch_monai(fixed_np: np.ndarray, moving_np: np.ndarray, verbose: bool = False) -> Tuple[
    np.ndarray, np.ndarray, float]:
    fixed_np = robust_normalize(fixed_np)
    moving_np = robust_normalize(moving_np)

    fixed = torch.from_numpy(fixed_np).unsqueeze(0).unsqueeze(0).float().to(DEVICE)
    moving = torch.from_numpy(moving_np).unsqueeze(0).unsqueeze(0).float().to(DEVICE)

    # 这里的 len(PYRAMID_ITERS) 决定了金字塔深度
    # 对于 2048x2048，建议设置为 5 或 6
    target_levels = len(PYRAMID_ITERS)

    if verbose:
        print(f"[Info] 构建 Fixed 图像金字塔 (Target Levels: {target_levels}):")
    fixed_pyr = build_pyramid(fixed, target_levels, verbose=verbose)

    if verbose:
        print(f"[Info] 构建 Moving 图像金字塔 (Target Levels: {target_levels}):")
    moving_pyr = build_pyramid(moving, target_levels, verbose=verbose)

    # 确保配置和实际生成的层数匹配
    levels = min(len(fixed_pyr), len(moving_pyr))
    iters_cfg = PYRAMID_ITERS[-levels:]

    _, _, H, W = fixed.shape
    disp = torch.zeros((1, 2, H, W), device=DEVICE, requires_grad=True)
    mi_loss_fn = GlobalMutualInformationLoss(num_bins=MI_NUM_BINS)
    optim = torch.optim.Adam([disp], lr=LR)

    for lvl, n_iter in enumerate(iters_cfg):
        fixed_lvl = fixed_pyr[lvl]
        moving_lvl = moving_pyr[lvl]
        _, _, h, w = fixed_lvl.shape
        scale_y, scale_x = h / H, w / W

        # 调试信息: 打印每层的处理尺寸
        # if r == start_r and c == start_c: # 仅在第一个patch打印避免刷屏
        #    print(f"  Level {lvl}: size=({h}, {w}), iters={n_iter}")

        for _ in range(n_iter):
            optim.zero_grad()
            disp_lvl = nn.functional.interpolate(disp, size=(h, w), mode="bilinear", align_corners=True)
            disp_lvl = torch.stack([disp_lvl[:, 0] * scale_x, disp_lvl[:, 1] * scale_y], dim=1)
            warped_moving = _warp(moving_lvl, disp_lvl)
            loss = mi_loss_fn(warped_moving, fixed_lvl) + LAMBDA_SMOOTH * gradient_loss(disp_lvl)
            loss.backward()
            optim.step()

        with torch.no_grad():
            disp_lvl = nn.functional.interpolate(disp, size=(h, w), mode="bilinear", align_corners=True)
            disp_lvl = torch.stack([disp_lvl[:, 0] * scale_x, disp_lvl[:, 1] * scale_y], dim=1)
            disp_up = nn.functional.interpolate(disp_lvl, size=(H, W), mode="bilinear", align_corners=True)
            disp_up = torch.stack([disp_up[:, 0] / (scale_x + EPS), disp_up[:, 1] / (scale_y + EPS)], dim=1)
            disp.copy_(disp_up)

    with torch.no_grad():
        warped = _warp(moving, disp)
        mi_score = compute_mi(fixed, warped, mi_loss_fn)

    return (
        warped.detach().cpu().numpy()[0, 0].astype(np.float32),
        disp.detach().cpu().numpy().astype(np.float32),
        mi_score
    )


def show_patch_registration(fixed, moving, warped, idx, **kwargs):
    # 1. 计算各种差异图
    diff_pre = np.abs(moving - fixed)  # 配准前误差
    diff_post = np.abs(warped - fixed)  # 配准后误差
    diff_change = np.abs(warped - moving)  # 配准带来的像素变化 (形变强度)

    # 统一误差显示的 Scale (vmax)，以便直观对比"配准前" vs "配准后" 的改善
    # 使用 99% 分位数作为最大值，避免极端噪点影响显示
    vmax_err = max(np.percentile(diff_pre, 99), np.percentile(diff_post, 99)) + 1e-5

    # 布局: 2行3列
    fig, axes = plt.subplots(2, 3, figsize=(15, 9), dpi=120)

    # --- 第一行: 原始图像 ---
    # S1 Fixed
    axes[0, 0].imshow(fixed, cmap="gray")
    axes[0, 0].set_title("Fixed (S1)")

    # S2 Moving
    axes[0, 1].imshow(moving, cmap="gray")
    axes[0, 1].set_title("Moving (S2)")

    # S2 Warped
    axes[0, 2].imshow(warped, cmap="gray")
    axes[0, 2].set_title("Warped (Result)")

    # --- 第二行: 差异与分析 ---

    # 1. 配准前误差 |Moving - Fixed|
    im1 = axes[1, 0].imshow(diff_pre, cmap="magma", vmin=0, vmax=vmax_err)
    axes[1, 0].set_title("Pre-Reg Error |Moving - Fixed|")
    plt.colorbar(im1, ax=axes[1, 0], fraction=0.046, pad=0.04)

    # 2. 配准后误差 |Warped - Fixed|
    im2 = axes[1, 1].imshow(diff_post, cmap="magma", vmin=0, vmax=vmax_err)
    axes[1, 1].set_title("Post-Reg Error |Warped - Fixed|")
    plt.colorbar(im2, ax=axes[1, 1], fraction=0.046, pad=0.04)

    # 3. 像素变化可视化 |Warped - Moving|
    im3 = axes[1, 2].imshow(diff_change, cmap="viridis")
    axes[1, 2].set_title("Pixel Change |Warped - Moving|")
    plt.colorbar(im3, ax=axes[1, 2], fraction=0.046, pad=0.04)

    # 构造信息标题
    info_parts = []
    for k, v in kwargs.items():
        if isinstance(v, float):
            info_parts.append(f"{k}: {v:.4f}")
        else:
            info_parts.append(f"{k}: {v}")
    info_str = " | ".join(info_parts)

    # 隐藏坐标轴
    for ax in axes.flatten():
        ax.axis("off")

    plt.suptitle(f"Patch ({idx[0]}, {idx[1]})\n{info_str}", fontsize=14)
    plt.tight_layout()
    plt.show()


def main() -> None:
    if DEVICE.type != "cuda":
        print("[WARN] 使用 CPU 运行，速度较慢。", flush=True)

    # 1. 打开数据
    s2_patches = open_zarr_array(PATCH_ZARR_S2)
    s1_patches = open_zarr_array(PATCH_ZARR_S1)

    nr_total, nc_total = s2_patches.shape[:2]
    # 理论上应该是 2048, 2048
    ph, pw = s2_patches.shape[2:4]
    print(f"[Info] Patch 尺寸: {ph} x {pw}")

    # 2. 确定循环范围
    if USE_ROI:
        # 限制范围，并防止越界
        start_r = max(0, ROI_ROW_START)
        end_r = min(nr_total, ROI_ROW_END)
        start_c = max(0, ROI_COL_START)
        end_c = min(nc_total, ROI_COL_END)
        print(f"[Config] 仅处理指定区域: Row [{start_r}, {end_r}), Col [{start_c}, {end_c})")
    else:
        start_r, end_r = 0, nr_total
        start_c, end_c = 0, nc_total
        print(f"[Config] 处理全图: {nr_total} x {nc_total}")

    # 3. 初始化输出 (保持全图尺寸，确保索引对应)
    # 注意：这里 mode='w' 会覆盖旧文件。如果你想分批跑，可以改用 mode='r+' (需要先创建好)
    out_store = zarr.open(
        OUT_ZARR_S2,
        mode="w",
        shape=(nr_total, nc_total, ph, pw),
        chunks=(1, 1, ph, pw),
        dtype="float16",
    )

    top_k_patches = []

    count = 0
    total_tasks = (end_r - start_r) * (end_c - start_c)
    print(f"[Info] 开始执行任务，共计 {total_tasks} 个 Patch...", flush=True)

    # 4. 执行循环
    for r in range(start_r, end_r):
        for c in range(start_c, end_c):
            count += 1

            # 仅在第一个 patch 打印金字塔构建信息，避免刷屏
            is_first_patch = (count == 1)

            # 读取数据
            s1_patch = s1_patches[r, c].compute()
            fixed_np = to_gray_s1(s1_patch)
            moving_np = s2_patches[r, c].compute()

            # 执行配准
            warped, disp, mi_score = register_patch_monai(fixed_np, moving_np, verbose=is_first_patch)

            # 保存结果
            out_store[r, c] = warped

            # 计算指标
            avg_disp_px, max_disp_px = get_pixel_displacement_stats(disp, ph, pw)

            # 显示进度
            print(f"[{count}/{total_tasks}] Patch ({r},{c}) MI={mi_score:.4f} "
                  f"AvgDisp={avg_disp_px:.2f}px", flush=True)

            # 维护 Top-K (用于最后展示)
            # 为了可视化，我们需要把原图和结果存起来
            # 注意：如果跑全图，这里存数据多了会爆内存。但因为只存 Top-K (K=3)，所以没问题。
            current_vis_data = {
                "idx": (r, c),
                "mi": mi_score,
                "avg_px": avg_disp_px,
                "max_px": max_disp_px,
                # 预先归一化，方便展示
                "fixed": robust_normalize(fixed_np),
                "moving": robust_normalize(moving_np),
                "warped": robust_normalize(warped)
            }

            if len(top_k_patches) < TOP_K_SHOW:
                heapq.heappush(top_k_patches, (mi_score, r, c, current_vis_data))
            else:
                if mi_score > top_k_patches[0][0]:
                    heapq.heappushpop(top_k_patches, (mi_score, r, c, current_vis_data))

    print("\n[DONE] 区域配准完成。")

    # 5. 展示结果
    top_k_sorted = sorted(top_k_patches, key=lambda x: x[0], reverse=True)

    if len(top_k_sorted) > 0:
        print(f"\n展示本区域内 MI 最高的 Top-{len(top_k_sorted)} 结果：")
        for i, item in enumerate(top_k_sorted):
            data = item[3]
            print(f"Rank #{i + 1}: Patch {data['idx']} (MI={data['mi']:.4f})")
            show_patch_registration(
                data['fixed'], data['moving'], data['warped'], data['idx'],
                mi=data['mi'], avg_disp=data['avg_px'], max_disp=data['max_px']
            )

        # 导出 Top-1
        best_data = top_k_sorted[0][3]
        best_idx = best_data['idx']

        # 导出 OME-TIFF
        try:
            name = f"best_region_patch_R{best_idx[0]}_C{best_idx[1]}.ome.tif"
            tifffile.imwrite(name, best_data['warped'], compression='zlib')
            print(f"已导出最佳结果到: {name}")
        except Exception as e:
            print(f"导出失败: {e}")
    else:
        print("未找到有效结果（可能区域为空或数据异常）。")


if __name__ == "__main__":
    main()