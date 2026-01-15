import os
import sys
import numpy as np
import pandas as pd
from sklearn.metrics import normalized_mutual_info_score

# ==========================================
# 1. 环境配置 (针对 Windows 和 pyvips)
# ==========================================

# 请确保这个路径是你解压的 libvips bin 文件夹的真实路径
vipshome = r'F:/opencv/envs/wsi_real/libvips/vips-dev-8.18/bin'

# 将 bin 目录加入环境变量 PATH
os.environ['PATH'] = vipshome + ';' + os.environ['PATH']

# 针对 Python 3.8+ 的 DLL 加载修复
if hasattr(os, 'add_dll_directory'):
    try:
        os.add_dll_directory(vipshome)
    except Exception as e:
        print(f"Warning: Could not add DLL directory: {e}")

import pyvips
from valis import registration

# 打印版本确认加载成功
print(f"Pyvips version: {pyvips.__version__}")
print("Pyvips loaded successfully!")


# ==========================================
# 2. 辅助函数：计算归一化互信息 (NMI)
# ==========================================
def calculate_nmi(img1_array, img2_array):
    """
    计算两个图像数组之间的归一化互信息 (Normalized Mutual Information)。
    """
    c1 = img1_array.ravel()
    c2 = img2_array.ravel()
    return normalized_mutual_info_score(c1, c2)


# ==========================================
# 3. 路径与参数设置
# ==========================================

src_dir = r"F:\keti_item\correct_data\big_data\TEST1"
dst_dir = r"F:\keti_item\correct_data\big_data\RESULT3"

if not os.path.exists(dst_dir):
    os.makedirs(dst_dir)

# ==========================================
# 4. 初始化配准器 (针对超大尺度图像优化)
# ==========================================
print("\n--- 初始化 VALIS (高精度配置) ---")

# max_processed_image_dim_px=5000: 刚性配准时的缩放基准
# 针对 48399 x 32999 像素的超大图像，适当增大此值有助于粗配准
registrar = registration.Valis(
    src_dir=src_dir,
    dst_dir=dst_dir,
    series=0,
    align_to_reference=True,
    max_processed_image_dim_px=2000,
    denoise_rigid=False,
    image_type=None
)

# ==========================================
# 5. 执行配准流程
# ==========================================

# --- [Step 1] 刚性配准 ---
print("\n--- [Step 1] 执行刚性配准 ---")
registrar.register()

# --- [Step 2] 非刚性配准 (微调局部 10 像素偏差的关键) ---
print("\n--- [Step 2] 执行非刚性配准 (高密度采样) ---")
# 15000px 采样保证算法“看清”局部变形
registrar.register_micro(max_non_rigid_registration_dim_px=2000)

# ==========================================
# 6. 验证与量化分析
# ==========================================
print("\n--- 验证配准效果 ---")

calc_level = 6  # 计算指标时使用较小的层级以节省内存
ref_slide = registrar.get_ref_slide()


def get_processed_numpy(slide_obj, level, apply_non_rigid, target_width=1536):
    """
    获取处理后的图像并转为单通道灰度用于 NMI 计算
    注意：这里的灰度转换仅用于计算指标，不影响最终保存
    """
    vips_img = slide_obj.warp_slide(level=level, non_rigid=apply_non_rigid)

    if vips_img.width > target_width:
        scale = target_width / vips_img.width
        vips_img = vips_img.resize(scale)

    arr = vips_img.numpy()
    # 如果是多通道，取平均值转灰度
    if arr.ndim == 3:
        arr = np.mean(arr, axis=2)
    return arr.astype(int)


print(f"正在读取参考图像进行 NMI 计算...")
ref_arr = get_processed_numpy(ref_slide, calc_level, apply_non_rigid=False)

nmi_results = []
for slide_obj in registrar.slide_dict.values():
    try:
        rigid_arr = get_processed_numpy(slide_obj, calc_level, apply_non_rigid=False)
        final_arr = get_processed_numpy(slide_obj, calc_level, apply_non_rigid=True)

        h, w = ref_arr.shape
        min_h, min_w = min(h, final_arr.shape[0]), min(w, final_arr.shape[1])

        score_rigid = calculate_nmi(ref_arr[:min_h, :min_w], rigid_arr[:min_h, :min_w])
        score_final = calculate_nmi(ref_arr[:min_h, :min_w], final_arr[:min_h, :min_w])

        nmi_results.append({
            "filename": slide_obj.name,
            "NMI_Rigid": score_rigid,
            "NMI_Final": score_final,
            "Improvement": score_final - score_rigid
        })
        print(f"处理完成: {slide_obj.name} | 提升: {score_final - score_rigid:.4f}")
    except Exception as e:
        print(f"验证失败: {slide_obj.name}, 错误: {e}")

# ==========================================
# 7. 保存最终结果 (修复灰度显示问题)
# ==========================================
print("\n--- 保存最终 OME-TIFF (Level 0 原始分辨率) ---")

save_level = 0

for slide_id, slide_obj in registrar.slide_dict.items():
    print(f"正在处理并保存: {slide_obj.name} -> .ome.tiff ...")

    try:
        # 1. 获取配准并变形后的图像
        warped_img = slide_obj.warp_slide(
            level=save_level,
            non_rigid=True,
            crop="overlap"
        )
        if warped_img.bands >= 3:

            warped_img = warped_img.copy(interpretation='srgb')
            print(f"  - 已强制指定颜色空间: sRGB (Bands: {warped_img.bands})")

        filename_no_ext = os.path.splitext(slide_obj.name)[0]
        out_path = os.path.join(dst_dir, f"{filename_no_ext}.ome.tiff")
        warped_img.write_to_file(
            out_path,
            tile=True,
            pyramid=True,
            compression="lzw",
            bigtiff=True,
            tile_width=512,
            tile_height=512
        )
        print(f"保存成功: {out_path}")

    except Exception as e:
        print(f"保存失败 {slide_obj.name}: {e}")

# 打印最终统计
df_res = pd.DataFrame(nmi_results)
print("\n--- 最终指标统计 ---")
print(df_res)

# 保存指标
df_res.to_csv(os.path.join(dst_dir, 'registration_metrics.csv'), index=False)

registration.kill_jvm()
print("\n🎉 任务完成！")