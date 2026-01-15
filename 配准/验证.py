"""
Zarr Patch 交互式预处理参数调优工具
功能：
1. 读取 S1 (HE) 和 S2 (PI) 的 Patch。
2. 提供交互式界面：
   - 拖动滑动条实时调整 Gamma, CLAHE ClipLimit, LoG Sigma。
   - 勾选框切换是否使用 LoG 特征。
   - 按钮切换上一个/下一个 Patch。
3. 实时展示：
   - 基础流程：原图 -> 中间增强图 -> 最终特征图。
   - [新增] HE 原始 vs 处理后对比。
   - [新增] HE 处理后 vs PI 处理后 (配准目标) 对比。
"""
import os
import zarr
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.widgets import Slider, Button, CheckButtons
import cv2
from skimage.color import rgb2hed
from skimage import exposure
from scipy.ndimage import gaussian_laplace

# --------------------------- 配置区域 --------------------------- #
PATCH_ZARR_S1 = "s1_patches.zarr"  # HE (Moving)
PATCH_ZARR_S2 = "s2_patches.zarr"  # PI (Fixed)

# 默认起始位置
START_ROW = 20
START_COL = 40

# ----------------------- 核心处理逻辑 ----------------------- #

def robust_normalize(img, p_low=1, p_high=99):
    """鲁棒归一化"""
    if img is None: return None
    vmin, vmax = np.percentile(img, [p_low, p_high])
    img_clip = np.clip(img, vmin, vmax)
    return (img_clip - vmin) / (vmax - vmin + 1e-6)

def run_he_pipeline(patch_rgb, gamma, clip_limit, sigma, use_log):
    """HE 处理流水线: RGB -> H通道 -> Gamma -> CLAHE -> (LoG)"""
    # 1. 颜色反卷积提取 H 通道
    # 注意：rgb2hed 返回的 H 通道，值越大表示染色越深（光密度）
    ihc_hed = rgb2hed(patch_rgb)
    h_channel = ihc_hed[:, :, 0]

    # 2. 归一化
    h_norm = robust_normalize(h_channel)

    # 3. Gamma 校正
    h_gamma = exposure.adjust_gamma(h_norm, gamma=gamma)

    # 4. CLAHE
    h_uint8 = (h_gamma * 255).astype(np.uint8)
    clahe = cv2.createCLAHE(clipLimit=clip_limit, tileGridSize=(16, 16))
    h_clahe = clahe.apply(h_uint8)
    img_enhanced = h_clahe.astype(np.float32) / 255.0

    # 5. LoG (可选)
    if use_log:
        # LoG 响应：亮斑点中心通常为负值
        log_response = gaussian_laplace(img_enhanced, sigma=sigma)
        img_final = robust_normalize(-log_response)
    else:
        img_final = img_enhanced

    return h_norm, img_enhanced, img_final

def run_pi_pipeline(patch_gray, clip_limit, sigma, use_log):
    """PI 处理流水线: Raw -> Norm -> CLAHE -> (LoG)"""
    # 1. 归一化
    p_norm = robust_normalize(patch_gray)

    # 2. CLAHE
    p_uint8 = (p_norm * 255).astype(np.uint8)
    clahe = cv2.createCLAHE(clipLimit=clip_limit, tileGridSize=(16, 16))
    p_clahe = clahe.apply(p_uint8)
    img_enhanced = p_clahe.astype(np.float32) / 255.0

    # 3. LoG (可选)
    if use_log:
        log_response = gaussian_laplace(img_enhanced, sigma=sigma)
        img_final = robust_normalize(-log_response)
    else:
        img_final = img_enhanced

    return p_norm, img_enhanced, img_final

# ----------------------- 交互式界面类 ----------------------- #

class InteractiveViewer:
    def __init__(self, s1_store, s2_store, start_r, start_c):
        self.s1_store = s1_store
        self.s2_store = s2_store
        self.rows, self.cols = s1_store.shape[:2]

        self.current_r = start_r
        self.current_c = start_c

        # 初始参数
        self.params = {
            'gamma': 1.0,
            'clip_limit': 3.0,
            'sigma': 4.0,
            'use_log': True
        }

        # 创建画布布局 (更宽以容纳5列)
        self.fig = plt.figure(figsize=(20, 10))
        plt.subplots_adjust(left=0.02, right=0.98, top=0.92, bottom=0.25, wspace=0.15, hspace=0.3)

        # 定义子图 (2行5列)
        # Row 1: HE pipeline + HE Comparison
        # Row 2: PI pipeline + Registration Target Comparison
        self.axes = {
            'he_org': self.fig.add_subplot(2, 5, 1),
            'he_h':   self.fig.add_subplot(2, 5, 2),
            'he_enh': self.fig.add_subplot(2, 5, 3),
            'he_fin': self.fig.add_subplot(2, 5, 4),
            'he_cmp': self.fig.add_subplot(2, 5, 5), # [新增] HE Gray vs Processed

            'pi_org': self.fig.add_subplot(2, 5, 6),
            'pi_nrm': self.fig.add_subplot(2, 5, 7),
            'pi_enh': self.fig.add_subplot(2, 5, 8),
            'pi_fin': self.fig.add_subplot(2, 5, 9),
            'reg_cmp': self.fig.add_subplot(2, 5, 10), # [新增] HE Final vs PI Final
        }

        # 隐藏坐标轴
        for ax in self.axes.values():
            ax.axis('off')

        # 添加控件区域 (Bottom)
        # 调整布局以适应更宽的窗口
        ax_gamma = plt.axes([0.10, 0.13, 0.25, 0.03])
        ax_clip  = plt.axes([0.10, 0.08, 0.25, 0.03])
        ax_sigma = plt.axes([0.45, 0.13, 0.25, 0.03])
        ax_check = plt.axes([0.45, 0.05, 0.10, 0.06])

        ax_prev  = plt.axes([0.75, 0.10, 0.08, 0.05])
        ax_next  = plt.axes([0.85, 0.10, 0.08, 0.05])
        ax_idx   = plt.axes([0.75, 0.04, 0.18, 0.04]) # 仅显示文本

        # 创建控件
        self.slider_gamma = Slider(ax_gamma, 'HE Gamma', 0.1, 3.0, valinit=1.0, valstep=0.1)
        self.slider_clip  = Slider(ax_clip, 'CLAHE Limit', 0.1, 10.0, valinit=3.0, valstep=0.1)
        self.slider_sigma = Slider(ax_sigma, 'LoG Sigma', 0.5, 10.0, valinit=4.0, valstep=0.5)

        self.check_log = CheckButtons(ax_check, ['Apply LoG'], [True])

        self.btn_prev = Button(ax_prev, '<< Prev')
        self.btn_next = Button(ax_next, 'Next >>')

        # 绑定事件
        self.slider_gamma.on_changed(self.update_params)
        self.slider_clip.on_changed(self.update_params)
        self.slider_sigma.on_changed(self.update_params)
        self.check_log.on_clicked(self.update_params)

        self.btn_prev.on_clicked(self.prev_patch)
        self.btn_next.on_clicked(self.next_patch)

        # 状态文本
        self.txt_idx = ax_idx
        self.txt_idx.axis('off')
        self.idx_label = self.txt_idx.text(0.5, 0.5, '', ha='center', va='center', fontsize=12)

        # 加载第一个 Patch
        self.load_current_patch()
        self.update_plots()

    def load_current_patch(self):
        """读取当前的 Zarr Patch 数据"""
        try:
            # 读取数据
            self.raw_he = self.s1_store[self.current_r, self.current_c]
            self.raw_pi = self.s2_store[self.current_r, self.current_c]

            # 格式修正
            # HE: (3, H, W) -> (H, W, 3)
            if self.raw_he.ndim == 3 and self.raw_he.shape[0] == 3:
                self.raw_he = np.transpose(self.raw_he, (1, 2, 0))

            # PI: (1, H, W) -> (H, W)
            if self.raw_pi.ndim == 3:
                self.raw_pi = self.raw_pi.squeeze()

            # 基础归一化用于显示原图
            self.disp_he = self.raw_he.astype(np.float32) / 255.0 if self.raw_he.dtype == np.uint8 else self.raw_he
            self.disp_pi = robust_normalize(self.raw_pi)

            # 更新标题信息
            self.idx_label.set_text(f"Patch: ({self.current_r}, {self.current_c})")
            print(f"[Info] Loaded Patch ({self.current_r}, {self.current_c})")

        except Exception as e:
            print(f"[Error] Load failed: {e}")

    def update_params(self, val=None):
        """回调：当滑块或复选框改变时"""
        self.params['gamma'] = self.slider_gamma.val
        self.params['clip_limit'] = self.slider_clip.val
        self.params['sigma'] = self.slider_sigma.val
        self.params['use_log'] = self.check_log.get_status()[0]

        self.update_plots()

    def update_plots(self):
        """重新计算流水线并更新图像显示"""
        # 1. 运行 HE 流水线
        h_norm, he_enh, he_fin = run_he_pipeline(
            self.raw_he,
            self.params['gamma'],
            self.params['clip_limit'],
            self.params['sigma'],
            self.params['use_log']
        )

        # 2. 运行 PI 流水线
        pi_norm, pi_enh, pi_fin = run_pi_pipeline(
            self.raw_pi,
            self.params['clip_limit'],
            self.params['sigma'],
            self.params['use_log']
        )

        # 3. 更新图像显示
        # Row 1: HE Steps
        self.axes['he_org'].imshow(self.disp_he)
        self.axes['he_org'].set_title("1. HE Original")

        self.axes['he_h'].imshow(h_norm, cmap='bone')
        self.axes['he_h'].set_title("2. H-Channel (Gray)")

        self.axes['he_enh'].imshow(he_enh, cmap='gray')
        self.axes['he_enh'].set_title(f"3. Enhanced (Gamma={self.params['gamma']:.1f})")

        self.axes['he_fin'].imshow(he_fin, cmap='gray')
        self.axes['he_fin'].set_title("4. Final Input (Target)")

        # [新增] Comparison 1: HE Gray (H-Channel) vs HE Processed
        # Red = H-Channel, Green = Processed
        cmp1 = np.stack([h_norm, he_fin, np.zeros_like(h_norm)], axis=-1)
        self.axes['he_cmp'].imshow(cmp1)
        self.axes['he_cmp'].set_title("5. Effect (R=Gray, G=Proc)\nYellow=NoChange")

        # Row 2: PI Steps
        self.axes['pi_org'].imshow(self.disp_pi, cmap='gray')
        self.axes['pi_org'].set_title("1. PI Original")

        self.axes['pi_nrm'].imshow(pi_norm, cmap='gray')
        self.axes['pi_nrm'].set_title("2. Normalized")

        self.axes['pi_enh'].imshow(pi_enh, cmap='gray')
        self.axes['pi_enh'].set_title(f"3. Enhanced (Clip={self.params['clip_limit']:.1f})")

        self.axes['pi_fin'].imshow(pi_fin, cmap='gray')
        self.axes['pi_fin'].set_title("4. Final Input (Source)")

        # [新增] Comparison 2: HE Final vs PI Final (The Registration Target)
        # Red = HE Final, Green = PI Final
        # 这展示了配准算法"眼中"两个模态的相似度
        cmp2 = np.stack([he_fin, pi_fin, np.zeros_like(he_fin)], axis=-1)
        self.axes['reg_cmp'].imshow(cmp2)
        self.axes['reg_cmp'].set_title("5. Align Target (R=HE, G=PI)\nYellow=Match")

        self.fig.canvas.draw_idle()

    def next_patch(self, event):
        if self.current_c + 1 < self.cols:
            self.current_c += 1
        elif self.current_r + 1 < self.rows:
            self.current_r += 1
            self.current_c = 0
        else:
            print("Already at last patch")
            return
        self.load_current_patch()
        self.update_plots()

    def prev_patch(self, event):
        if self.current_c - 1 >= 0:
            self.current_c -= 1
        elif self.current_r - 1 >= 0:
            self.current_r -= 1
            self.current_c = self.cols - 1
        else:
            print("Already at first patch")
            return
        self.load_current_patch()
        self.update_plots()

def main():
    if not os.path.exists(PATCH_ZARR_S1) or not os.path.exists(PATCH_ZARR_S2):
        print("❌ 错误：找不到 Zarr 文件。")
        return

    try:
        # 打开数据
        s1_store = zarr.open(PATCH_ZARR_S1, mode='r')
        s2_store = zarr.open(PATCH_ZARR_S2, mode='r')

        print("=== 交互式参数调优工具启动 ===")
        print("操作指南:")
        print("1. 拖动 Slider 调整参数。")
        print("2. 观察右上角 'Effect': 红色=原始H通道, 绿色=处理后。")
        print("3. 观察右下角 'Align Target': 红色=HE处理后, 绿色=PI处理后。黄色越多越好。")

        viewer = InteractiveViewer(s1_store, s2_store, START_ROW, START_COL)
        plt.show()

    except Exception as e:
        print(f"❌ 运行失败: {e}")
        import traceback
        traceback.print_exc()

if __name__ == "__main__":
    main()