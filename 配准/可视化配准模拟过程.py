from __future__ import annotations

import matplotlib.pyplot as plt
import numpy as np
from matplotlib.colors import LogNorm


# ================================================================= #
#                        核心模拟函数
# ================================================================= #

def generate_phantom_data(size=256):
    """
    生成一对模拟的跨模态图像 (S1 vs S2)。
    模拟逻辑：S1 是 SAR (边缘亮，结构清晰)，S2 是光学 (填充颜色，灰度不同)。
    """
    # 1. 创建基础网格
    y, x = np.mgrid[:size, :size]
    center = size // 2

    # 2. 创建几何形状：一个圆形和一个矩形
    # 圆形 mask
    dist = np.sqrt((x - center) ** 2 + (y - center) ** 2)
    circle_mask = dist < (size // 4)

    # 矩形 mask
    rect_mask = np.zeros((size, size), dtype=bool)
    rect_mask[size // 5:size // 3, size // 5:size // 3] = True

    # --- 生成 S1 (Fixed): 模拟 SAR ---
    # 假设 SAR 对边缘和特定材质反射强 (高亮)
    s1 = np.zeros((size, size), dtype=np.float32)
    s1[circle_mask] = 0.8  # 圆形区域反射强
    s1[rect_mask] = 0.5  # 矩形区域反射中等
    # 添加相干斑噪声 (Speckle noise)
    noise = np.random.normal(0, 0.1, (size, size))
    s1 = s1 + s1 * noise
    s1 = np.clip(s1, 0, 1)

    # --- 生成 S2 (Moving): 模拟光学 ---
    # 假设光学图像与 SAR 是 "负相关" 或 "非线性关系"
    # 例如：SAR 亮的地方，光学可能是暗的 (水体/平滑表面)
    s2 = np.ones((size, size), dtype=np.float32) * 0.2  # 背景有些灰度
    s2[circle_mask] = 0.1  # 圆形区域在光学下很暗
    s2[rect_mask] = 0.9  # 矩形区域在光学下很亮
    # 添加高斯噪声
    s2 = s2 + np.random.normal(0, 0.05, (size, size))
    s2 = np.clip(s2, 0, 1)

    return s1, s2


def shift_image(img, dy, dx):
    """简单的平移模拟"""
    h, w = img.shape
    shifted = np.zeros_like(img)
    # 使用切片进行平移
    if dy >= 0:
        src_y_start, src_y_end = 0, h - dy
        dst_y_start, dst_y_end = dy, h
    else:
        src_y_start, src_y_end = -dy, h
        dst_y_start, dst_y_end = 0, h + dy

    if dx >= 0:
        src_x_start, src_x_end = 0, w - dx
        dst_x_start, dst_x_end = dx, w
    else:
        src_x_start, src_x_end = -dx, w
        dst_x_start, dst_x_end = 0, w + dx

    # 简单的非插值平移 (仅用于演示直方图变化)
    if dst_y_end > dst_y_start and dst_x_end > dst_x_start:
        shifted[dst_y_start:dst_y_end, dst_x_start:dst_x_end] = \
            img[src_y_start:src_y_end, src_x_start:src_x_end]

    return shifted


def compute_joint_histogram(img1, img2, bins=64):
    """计算 2D 联合直方图"""
    # 展平
    i1 = img1.ravel()
    i2 = img2.ravel()

    # 仅计算非背景区域 (简单处理，避免大量0值主导)
    mask = (i1 > 0.01) | (i2 > 0.01)
    i1 = i1[mask]
    i2 = i2[mask]

    hist_2d, x_edges, y_edges = np.histogram2d(
        i1, i2, bins=bins, range=[[0, 1], [0, 1]]
    )
    return hist_2d.T  # 转置以匹配笛卡尔坐标系 (X=img1, Y=img2)


def compute_mi_from_hist(hist_2d):
    """从联合直方图计算互信息"""
    pxy = hist_2d / (np.sum(hist_2d) + 1e-10)
    px = np.sum(pxy, axis=1)  # 边缘分布 P(X)
    py = np.sum(pxy, axis=0)  # 边缘分布 P(Y)

    px = px[px > 0]
    py = py[py > 0]
    pxy = pxy[pxy > 0]

    Hx = -np.sum(px * np.log2(px))
    Hy = -np.sum(py * np.log2(py))
    Hxy = -np.sum(pxy * np.log2(pxy))

    return Hx + Hy - Hxy


# ================================================================= #
#                        可视化主程序
# ================================================================= #

def visualize_process():
    # 1. 准备数据
    fixed, moving_orig = generate_phantom_data()

    # 2. 定义三个模拟阶段：[严重错位, 轻微错位, 完美对齐]
    scenarios = [
        {"title": "Step 0: Initial (Large Offset)", "shift": (40, 40)},
        {"title": "Step 50: Optimizing (Small Offset)", "shift": (10, 10)},
        {"title": "Step 100: Aligned (Zero Offset)", "shift": (0, 0)}
    ]

    # 3. 创建画布
    fig, axes = plt.subplots(3, 3, figsize=(15, 12), dpi=100)
    plt.subplots_adjust(hspace=0.4, wspace=0.3)

    # 4. 循环绘制每一行
    for i, scen in enumerate(scenarios):
        dy, dx = scen["shift"]
        title = scen["title"]

        # 模拟移动
        moving_shifted = shift_image(moving_orig, dy, dx)

        # 计算联合直方图
        hist_2d = compute_joint_histogram(fixed, moving_shifted)
        mi_val = compute_mi_from_hist(hist_2d)

        # --- Column 1: Overlay Image ---
        ax_img = axes[i, 0]
        # 以此制作一个 RGB 叠加图 (Red=Fixed, Green=Moving)
        overlay = np.zeros((fixed.shape[0], fixed.shape[1], 3))
        overlay[..., 0] = fixed  # R通道放 Fixed (SAR)
        overlay[..., 1] = moving_shifted  # G通道放 Moving (Optical)
        # B通道保持黑，这样重叠部分会变成黄色

        ax_img.imshow(overlay)
        ax_img.set_title(f"{title}\nOverlay (R=Fixed, G=Moving)", fontsize=11)
        ax_img.axis('off')

        # --- Column 2: Joint Histogram (2D) ---
        ax_hist = axes[i, 1]
        # 使用 LogNorm 让分布更清晰，否则高频点太亮掩盖细节
        im = ax_hist.imshow(hist_2d, origin='lower', extent=[0, 1, 0, 1],
                            cmap='jet', norm=LogNorm())
        ax_hist.set_xlabel("Fixed Image Intensity")
        ax_hist.set_ylabel("Moving Image Intensity")
        ax_hist.set_title(f"Joint Histogram\nMI Score: {mi_val:.4f}", fontsize=11, fontweight='bold', color='blue')

        # 添加辅助线解释
        if i == 2:  # 在最后一张图上画出理想关系
            # 对于我们的模拟数据，圆: S1=0.8, S2=0.1; 矩形: S1=0.5, S2=0.9
            # 所以应该会有两个明显的聚类中心
            ax_hist.annotate('Cluster 1\n(Rectangle)', xy=(0.5, 0.9), xytext=(0.2, 0.9),
                             arrowprops=dict(facecolor='white', shrink=0.05), color='white')
            ax_hist.annotate('Cluster 2\n(Circle)', xy=(0.8, 0.1), xytext=(0.8, 0.4),
                             arrowprops=dict(facecolor='white', shrink=0.05), color='white')

        # --- Column 3: Analysis / Interpretation ---
        ax_text = axes[i, 2]
        ax_text.axis('off')

        if i == 0:
            desc = (
                "STATUS: Misaligned\n\n"
                "Histogram Analysis:\n"
                "- Points are scattered everywhere.\n"
                "- This 'cloud' means knowing pixel value in S1\n"
                "  tells you NOTHING about S2.\n"
                "- High Joint Entropy -> Low MI."
            )
        elif i == 1:
            desc = (
                "STATUS: Approaching\n\n"
                "Histogram Analysis:\n"
                "- The scattered cloud starts to condense.\n"
                "- Some clusters begin to form but are\n"
                "  still blurry/wide.\n"
                "- MI score increases."
            )
        else:
            desc = (
                "STATUS: Registered!\n\n"
                "Histogram Analysis:\n"
                "- Sharp, distinct clusters visible.\n"
                "- Relationship is clear:\n"
                "  When S1=0.8, S2 is consistently 0.1.\n"
                "- Minimum Entropy -> Maximum MI."
            )

        ax_text.text(0.1, 0.5, desc, fontsize=12, va='center', family='monospace')

    plt.suptitle(
        "Visualizing Registration via Joint Histograms\n(Why simple gray histograms aren't enough for Multi-Modal)",
        fontsize=16)
    plt.tight_layout()
    plt.show()


if __name__ == "__main__":
    visualize_process()