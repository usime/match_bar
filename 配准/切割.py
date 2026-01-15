"""
基于 Zarr + Dask 的大图切块（50% overlap）预处理脚本。

背景
----
S2 : 单通道 (H, W)
S1 : 三通道 (H, W, 3)

流程
----
1. 将超大 TIFF 转为 Zarr 分块存储（不会一次性写满内存）。
2. 用 `dask.array.from_zarr` 懒加载为虚拟大数组。
3. 通过 `dask.array.sliding_window_view` 生成 50% 重叠的滑窗 patch。
   - patch 大小、overlap 比例、边界 padding 可调。
4. 将滑窗 patch 数据写回 zarr，供后续“跨模态局部配准”使用。

注意
----
- 如果 TIFF 很大，最好用 `tifffile.imread(..., aszarr=True)` + `dask` 直接写 zarr，避免 `skimage.io.imread` 将整张图片读入内存。
- 这里给出“可运行示例”——若显存/内存不足，请根据硬件能力调小 `PATCH_SIZE` 或换更懒加载方案。
"""

from __future__ import annotations

import os
from pathlib import Path
from typing import Tuple

import dask.array as da
import numpy as np
import zarr
from skimage import io

# --------------------------- 可调参数 --------------------------- #
# 原始大图路径（VALIS 粗配准结果）
TIF_PATH_S2 = r"F:\\keti_item\\correct_data\\big_data\\result\\S2.ome.tiff"
TIF_PATH_S1 = r"F:\\keti_item\\correct_data\\big_data\\result\\S1.ome.tiff"

# Zarr 输出目录（会自动创建）
ZARR_PATH_S2 = "s2_coarse_registered.zarr"
ZARR_PATH_S1 = "s1_coarse_registered.zarr"
PATCH_ZARR_S2 = "s2_patches.zarr"
PATCH_ZARR_S1 = "s1_patches.zarr"

# 每个 patch 的高、宽 (像素)
PATCH_SIZE: Tuple[int, int] = (1024, 1024)
# overlap 百分比（0~1）: 0.5 == 50% 重叠
OVERLAP_RATIO: float = 0.5
# zarr chunks，跟 patch_size 保持一致或稍大/小均可
CHUNK_SIZE: Tuple[int, int] = PATCH_SIZE

# 边界填充策略（与 numpy.pad 一致）: "reflect", "edge", etc.
BOUNDARY_MODE = "reflect"
# -------------------------------------------------------------- #


# --------------------------- 工具函数 --------------------------- #

def ensure_zarr(image_np: np.ndarray, zarr_path: str, chunks: Tuple[int, ...] | None = None) -> None:
    """把 numpy.ndarray 保存为 zarr。若文件已存在则跳过。"""
    if Path(zarr_path).exists():
        return

    # 自动推断 chunks（按维度数量补齐），最后一个维度通常是通道，保持全量 chunk
    if chunks is None:
        *spatial, c = image_np.shape if image_np.ndim == 3 else (*image_np.shape,)
        chunks = (*CHUNK_SIZE, c) if image_np.ndim == 3 else CHUNK_SIZE  # type: ignore[assignment]

    store = zarr.open(
        zarr_path,
        mode="w",
        shape=image_np.shape,
        chunks=chunks,
        dtype=image_np.dtype,
    )
    store[:] = image_np


def tif_to_zarr(tif_path: str, zarr_path: str) -> None:
    """将 tiff 转为 zarr（简单示例: 先读为 numpy，再写 zarr）。"""
    print(f"[INFO] Loading TIFF => {tif_path}")
    img_np = io.imread(tif_path)
    print(f"[INFO] TIFF shape: {img_np.shape}, dtype: {img_np.dtype}")
    ensure_zarr(img_np, zarr_path)
    del img_np  # 释放内存


# --------------------------- 滑窗生成 --------------------------- #

def build_sliding_windows(da_img: da.Array) -> da.Array:
    """对 dask array 生成 50% overlap patches，返回新的 dask array。

    输出 shape:
    (num_row, num_col, patch_h, patch_w, [C])
    """
    patch_h, patch_w = PATCH_SIZE
    step_h = int(patch_h * (1 - OVERLAP_RATIO))
    step_w = int(patch_w * (1 - OVERLAP_RATIO))

    # full sliding window (stride 1)
    patches = da.lib.stride_tricks.sliding_window_view(
        da_img,
        window_shape=(patch_h, patch_w),
        axis=(0, 1),
    )
    # sub-sample步长 (step_h, step_w)
    patches = patches[::step_h, ::step_w]
    return patches


# --------------------------- 主流程 --------------------------- #

def main() -> None:
    # 1) 把 tiff 转为 zarr（如果已存在则跳过）
    tif_to_zarr(TIF_PATH_S2, ZARR_PATH_S2)
    tif_to_zarr(TIF_PATH_S1, ZARR_PATH_S1)

    # 2) 用 dask.from_zarr 懒加载
    s2_dask = da.from_zarr(ZARR_PATH_S2)  # shape: (H, W)
    s1_dask = da.from_zarr(ZARR_PATH_S1)  # shape: (H, W, 3)

    # 3) 生成滑窗 dask array（50% overlap）
    s2_patches = build_sliding_windows(s2_dask)
    s1_patches = build_sliding_windows(s1_dask)

    # 4) 写 zarr（可选 => 这里为了示例直接写）
    print("[INFO] Write patches to zarr (this may take time & disk space)...")
    s2_patches.to_zarr(PATCH_ZARR_S2, overwrite=True)
    s1_patches.to_zarr(PATCH_ZARR_S1, overwrite=True)
    print("[DONE] Patches saved!")


if __name__ == "__main__":
    os.makedirs(Path(ZARR_PATH_S2).parent, exist_ok=True)
    main()
