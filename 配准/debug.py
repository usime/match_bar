from __future__ import annotations

import os
from pathlib import Path
import matplotlib.pyplot as plt
import numpy as np
import dask.array as da
import zarr
import cv2  # pip install opencv-python
import tifffile  # pip install tifffile
from skimage.exposure import match_histograms  # pip install scikit-image

# ================================================================= #
#                           配置区域
# ================================================================= #

# ----------------- 1. 路径配置 ----------------- #
PATCH_ZARR_S1 = "s1_patches.zarr"
PATCH_ZARR_S2 = "s2_patches.zarr"
OUTPUT_ZARR_PATH = "s1_patches_processed.zarr"  # 新的输出 Zarr 路径
OUTPUT_DIR = "output_results"  # 用于存放少量示例图的文件夹


# ================================================================= #
#                        核心辅助函数
# ================================================================= #

def open_zarr_array(path: str, is_s2: bool = False) -> da.Array:
    """加载 Zarr 数据，如果不存在则生成模拟数据用于演示。"""
    if not Path(path).exists():
        print(f"[Warning] 未找到 {path}，正在生成临时模拟数据...")
        store = zarr.DirectoryStore(path)
        root = zarr.group(store=store, overwrite=True)

        # 模拟 10x10 的 patches (批量演示用小一点), 每个 256x256
        # 实际使用时会自动读取您真实文件的大小
        rows, cols = 10, 10
        channels = 3 if is_s2 else 3

        # 创建基础数据
        dummy_data = np.zeros((rows, cols, 256, 256, channels), dtype=np.float32)

        # 填充一些随机纹理
        for i in range(rows):
            for j in range(cols):
                base = np.random.rand(256, 256, channels) * 0.2
                for _ in range(5):
                    rx, ry = np.random.randint(20, 230, 2)
                    y, x = np.ogrid[-rx:256 - rx, -ry:256 - ry]
                    mask = x * x + y * y <= 10 * 10
                    base[mask] = 0.8 + np.random.rand(mask.sum(), channels) * 0.2

                dummy_data[i, j] = np.clip(base, 0, 1)

        z = root.create_dataset("data", data=dummy_data, chunks=(1, 1, 256, 256, channels))
        return da.from_zarr(z)
    return da.from_zarr(path)


def robust_normalize(img: np.ndarray, p_low: float = 1.0, p_high: float = 99.0) -> np.ndarray:
    """基于分位数的强度归一化 (0-1范围)，抗离群点干扰。"""
    img_f = img.astype(np.float32, copy=False)
    if np.all(img_f == img_f.flat[0]):
        return np.zeros_like(img_f)

    lo = np.percentile(img_f, p_low)
    hi = np.percentile(img_f, p_high)

    if hi - lo < 1e-6:
        return np.zeros_like(img_f)

    img_f = np.clip(img_f, lo, hi)
    img_f = (img_f - lo) / (hi - lo + 1e-6)
    return img_f


def method_b_enhanced(s1_patch: np.ndarray) -> np.ndarray:
    """Method B: 颜色反卷积 + Gamma + CLAHE (基础增强)。"""
    # 1. 格式统一化
    if s1_patch.ndim == 3 and s1_patch.shape[0] < s1_patch.shape[-1] and s1_patch.shape[0] <= 4:
        img = np.transpose(s1_patch, (1, 2, 0))  # CHW -> HWC
    else:
        img = s1_patch

    img_norm = robust_normalize(img)

    # 2. 构建 Pseudo-RGB
    H, W = img_norm.shape[:2]
    rgb_input = np.zeros((H, W, 3), dtype=np.float32)
    current_channels = img_norm.shape[-1] if img_norm.ndim == 3 else 1

    if current_channels >= 3:
        rgb_input = img_norm[..., :3]
    elif current_channels == 2:
        rgb_input[..., 0] = img_norm[..., 0]
        rgb_input[..., 1] = img_norm[..., 1]
        rgb_input[..., 2] = np.mean(img_norm, axis=-1)
    else:
        rgb_input = np.stack([img_norm] * 3, axis=-1)

    # 3. 颜色反卷积 (提取 H 通道)
    with np.errstate(divide='ignore', invalid='ignore'):
        od_img = -np.log10(rgb_input + 1e-6)

    # 经验参数矩阵
    h_channel = (od_img[..., 0] * 0.644 +
                 od_img[..., 1] * 0.717 +
                 od_img[..., 2] * 0.267)

    # 4. 增强
    h_norm = robust_normalize(h_channel)
    h_gamma = np.power(h_norm, 0.8)  # Gamma 校正

    # CLAHE
    h_uint8 = (h_gamma * 255).clip(0, 255).astype(np.uint8)
    clahe = cv2.createCLAHE(clipLimit=3.0, tileGridSize=(8, 8))
    h_clahe = clahe.apply(h_uint8)

    return h_clahe.astype(np.float32) / 255.0


def apply_global_hist_matching(source: np.ndarray, reference: np.ndarray) -> np.ndarray:
    """
    全局直方图匹配 (包含 Dithering，防止离散断层)。
    """
    if reference.ndim == 3: reference = reference.mean(axis=-1)
    reference = robust_normalize(reference)

    # Dithering (防止色阶断层)
    # 这里的噪声水平可以非常微小，足以打散相同的灰度值即可
    noise_level = 0.002
    dithering = np.random.uniform(-noise_level, noise_level, source.shape).astype(np.float32)
    source_dithered = np.clip(source + dithering, 0, 1)

    matched = match_histograms(source_dithered, reference, channel_axis=None)
    return matched


# ================================================================= #
#                            主程序 (批量处理)
# ================================================================= #

def main() -> None:
    print("-> 正在初始化数据读取...")
    # 1. 读取输入数据 (Dask 懒加载模式)
    s1_patches_dask = open_zarr_array(PATCH_ZARR_S1, is_s2=False)
    s2_patches_dask = open_zarr_array(PATCH_ZARR_S2, is_s2=True)

    nr, nc = s1_patches_dask.shape[:2]
    H, W =1024,1024  # 假设每个 patch 都是 256x256，可从 s1_patches_dask.shape 获取

    print(f"[Info] S1 数据形状: {s1_patches_dask.shape}")
    print(f"[Info] 总分组数 (Rows x Cols): {nr} x {nc} = {nr * nc}")
    print(f"[Info] 准备处理并输出到: {OUTPUT_ZARR_PATH}")

    # 2. 初始化输出 Zarr 存储
    # 我们创建一个新的 Zarr 文件，结构与原文件对应的网格一致
    # 输出为单通道灰度图 (Nr, Nc, H, W)
    store_out = zarr.DirectoryStore(OUTPUT_ZARR_PATH)
    root_out = zarr.group(store=store_out, overwrite=True)

    # 创建数据集，块大小设为 (1, 1, 256, 256) 方便按 Patch 读写
    processed_ds = root_out.create_dataset(
        "data",
        shape=(nr, nc, H, W),
        chunks=(1, 1, H, W),
        dtype=np.float32,
        compressor=zarr.Blosc(cname='zstd', clevel=3, shuffle=2)
    )

    # 3. 批量循环处理
    total_patches = nr * nc
    processed_count = 0

    print("\n-> 开始批量处理 (Histogram Matching with Dithering)...")

    for r in range(nr):
        for c in range(nc):
            # (A) 读取数据: compute() 将 dask chunk 加载到内存
            try:
                s1_raw = s1_patches_dask[r, c].compute()
                s2_raw = s2_patches_dask[r, c].compute()
            except Exception as e:
                print(f"\n[Error] 读取 Patch ({r}, {c}) 失败: {e}")
                continue

            # (B) 核心处理流程
            # 1. 增强 (Method B)
            res_enhanced = method_b_enhanced(s1_raw)

            # 2. 匹配 (Global Matching + Dithering)
            res_matched = apply_global_hist_matching(res_enhanced, s2_raw)

            # (C) 写入新的 Zarr 数据集
            processed_ds[r, c] = res_matched

            processed_count += 1

            # (D) 进度打印
            if processed_count % 10 == 0 or processed_count == total_patches:
                percent = (processed_count / total_patches) * 100
                print(f"   Progress: {processed_count}/{total_patches} ({percent:.1f}%)", end='\r')

    print(f"\n\n[Success] 所有分组处理完毕。")
    print(f"-> 新的 S1 Patches 已保存至: {os.path.abspath(OUTPUT_ZARR_PATH)}")
    print("-> 数据格式: Float32 (0.0 - 1.0), Shape: (Rows, Cols, 256, 256)")


if __name__ == "__main__":
    main()