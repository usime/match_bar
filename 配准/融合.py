import os
import zarr
import numpy as np
import tifffile
import scipy.signal
from tqdm import tqdm

# =========================== 配置区域 =========================== #

# 1. 输入 Zarr 路径
ZARR_PATH_S2 = r"F:\PythonProject3\pythonProject\keti\组织切片\code\s2_patches.zarr"  # 配准后的 Patch
# 如果你想合并 S1，修改为 s1_patches.zarr 即可

# 2. Patch 参数 (必须与生成 Zarr 时一致)
PATCH_SIZE = 2048
OVERLAP = 0.5  # 50% 重叠

# 3. 指定合并的区域 (ROI) - 用于快速验证
# 例如：合并第 10 行到第 12 行，第 20 列到第 22 列 (即 2x2 个块)
ROI_ROW_START = 10
ROI_ROW_END = 12  # 不包含 End
ROI_COL_START = 20
ROI_COL_END = 22  # 不包含 End

# 4. 输出文件路径
OUTPUT_FILENAME = f"merged_roi_r{ROI_ROW_START}-{ROI_ROW_END}_c{ROI_COL_START}-{ROI_COL_END}.ome.tif"

# 5. 高斯权重参数 (sigma_scale 越大，中心越平坦，边缘衰减越慢)
# MONAI 默认约为 0.125
SIGMA_SCALE = 0.125


# =============================================================== #

def get_gaussian_weight_map(size: int, sigma_scale: float = 0.125) -> np.ndarray:
    """
    生成 2D 高斯权重图。
    模仿 MONAI SlidingWindowInferer 的默认行为。
    """
    sigma = size * sigma_scale
    # 生成 1D 高斯窗
    window = scipy.signal.windows.gaussian(size, std=sigma)
    # 外积生成 2D 权重图 (H, W)
    weight_map = np.outer(window, window)
    return weight_map.astype(np.float32)


def stitch_region_gaussian(
        zarr_path: str,
        row_start: int, row_end: int,
        col_start: int, col_end: int,
        patch_size: int,
        overlap: float
):
    """
    使用高斯加权平均合并指定区域的 Patches
    """
    if not os.path.exists(zarr_path):
        raise FileNotFoundError(f"Zarr 文件未找到: {zarr_path}")

    # 1. 打开 Zarr
    store = zarr.open(zarr_path, mode='r')
    # 假设 Zarr 结构为 (Rows, Cols, H, W) 或 (Rows, Cols, C, H, W)
    print(f"[Info] Zarr Shape: {store.shape}")

    # 检查维度
    has_channel = False
    if store.ndim == 5:  # (R, C, Ch, H, W)
        has_channel = True
        n_channels = store.shape[2]
        print(f"[Info] 检测到多通道数据: {n_channels} Channels")
    elif store.ndim == 4:  # (R, C, H, W)
        has_channel = False
        n_channels = 1
        print("[Info] 检测到单通道数据")
    else:
        raise ValueError(f"不支持的 Zarr 维度: {store.ndim}, 期望 4 或 5 维")

    # 2. 计算画布大小
    # Stride (步长) = Patch 大小 * (1 - 重叠率)
    stride = int(patch_size * (1 - overlap))

    num_rows = row_end - row_start
    num_cols = col_end - col_start

    # 画布总高度 = (行数 - 1) * 步长 + Patch高度
    canvas_h = (num_rows - 1) * stride + patch_size
    canvas_w = (num_cols - 1) * stride + patch_size

    print(f"[Info] 合并区域: {num_rows}x{num_cols} Patches")
    print(f"[Info] 步长 (Stride): {stride}")
    print(f"[Info] 输出画布尺寸: {canvas_h} x {canvas_w}")

    # 3. 初始化累加器 (使用 float32 避免溢出和精度问题)
    if has_channel:
        # (C, H, W) 方便计算，最后保存时转回 (H, W, C) 或保持 OME 标准
        accumulator = np.zeros((n_channels, canvas_h, canvas_w), dtype=np.float32)
        weight_sum = np.zeros((1, canvas_h, canvas_w), dtype=np.float32)
    else:
        accumulator = np.zeros((canvas_h, canvas_w), dtype=np.float32)
        weight_sum = np.zeros((canvas_h, canvas_w), dtype=np.float32)

    # 4. 生成高斯权重图
    # 形状 (H, W)，值域 0~1
    weight_patch = get_gaussian_weight_map(patch_size, SIGMA_SCALE)

    # 如果是多通道，权重图需要增加维度以匹配广播 (1, H, W)
    if has_channel:
        weight_patch_expanded = weight_patch[np.newaxis, :, :]

    # 5. 开始循环合并
    print("[Info] 开始高斯加权融合...")

    total_patches = num_rows * num_cols
    pbar = tqdm(total=total_patches)

    for r_idx in range(num_rows):
        for c_idx in range(num_cols):
            # 全局坐标索引
            global_r = row_start + r_idx
            global_c = col_start + c_idx

            # 读取 Patch 数据
            try:
                patch_data = store[global_r, global_c]  # 可能是 (H, W) 或 (C, H, W)
            except Exception as e:
                print(f"\n[Warn] 读取 Patch ({global_r}, {global_c}) 失败: {e}")
                continue

            # 转换数据为 float32 用于计算
            patch_data = patch_data.astype(np.float32)

            # 计算在局部画布中的放置位置 (Top-Left)
            y_start = r_idx * stride
            x_start = c_idx * stride
            y_end = y_start + patch_size
            x_end = x_start + patch_size

            # 累加：分子 += 图像 * 权重
            if has_channel:
                # 确保 patch_data 是 (C, H, W)
                if patch_data.ndim == 2: patch_data = patch_data[np.newaxis, ...]
                accumulator[:, y_start:y_end, x_start:x_end] += patch_data * weight_patch_expanded
                weight_sum[:, y_start:y_end, x_start:x_end] += weight_patch_expanded
            else:
                accumulator[y_start:y_end, x_start:x_end] += patch_data * weight_patch
                weight_sum[y_start:y_end, x_start:x_end] += weight_patch

            pbar.update(1)

    pbar.close()

    # 6. 归一化：结果 = 分子 / 分母
    # 添加极小值 epsilon 防止除以零 (虽然理论上不会发生)
    print("[Info] 正在归一化...")
    final_image = accumulator / (weight_sum + 1e-6)

    # 7. 转换回原始数据类型 (假设原始是 float16 或 uint8，这里转回常见的 uint8 或 float32)
    # 根据您的上一个代码，存的是 float16。为了通用性，这里保留 float32 或者转 uint8
    # 如果您需要查看，建议转为 float32 保存，或者归一化到 0-255 转 uint8

    # 简单处理：保留 float32 精度，tifffile 支持
    final_image = final_image.astype(np.float32)

    return final_image


def main():
    print("=== 高斯加权图像融合 Demo ===")

    try:
        merged_img = stitch_region_gaussian(
            ZARR_PATH_S2,
            ROI_ROW_START, ROI_ROW_END,
            ROI_COL_START, ROI_COL_END,
            PATCH_SIZE, OVERLAP
        )

        print(f"[Info] 融合完成，结果形状: {merged_img.shape}")

        # 保存为 OME-TIFF
        # ImageJ 偏好的顺序通常是 (Channels, Y, X) 或者 (Y, X)
        # 如果是多通道 (C, H, W)，tifffile 可以处理
        print(f"[Info] 正在保存到: {OUTPUT_FILENAME}")

        # 确定 metadata axes
        if merged_img.ndim == 3:
            axes = 'CYX'
        else:
            axes = 'YX'

        tifffile.imwrite(
            OUTPUT_FILENAME,
            merged_img,
            imagej=True,
            metadata={'axes': axes},
            compression='zlib'
        )

        print("✅ 任务完成！")
        print(f"请使用 ImageJ/Fiji 打开 {OUTPUT_FILENAME} 查看接缝融合效果。")

    except Exception as e:
        print(f"❌ 发生错误: {e}")
        import traceback
        traceback.print_exc()


if __name__ == "__main__":
    main()