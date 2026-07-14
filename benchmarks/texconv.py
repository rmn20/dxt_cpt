import os
import subprocess
import glob
import sys
from datetime import datetime

import numpy
from PIL import Image
from skimage import metrics

import torch
import lpips

TEX_SRC_DIR = "textures_source2/"
TEX_BC7ENC_DIR = "textures_bc7enc/"
TEX_MYENC_DIR = "textures_myenc/"

os.makedirs(TEX_BC7ENC_DIR, exist_ok=True)
os.makedirs(TEX_MYENC_DIR, exist_ok=True)

png_files = glob.glob(os.path.join(TEX_SRC_DIR, "*.png"))

device = 'cuda' if torch.cuda.is_available() else 'cpu'
loss_fn = lpips.LPIPS(net='alex').to(device)
    
ssim_sum = [0, 0]
mse_sum = [0, 0]
lpips_sum = [0, 0]

images_processed = 0
channels_processed = 0

results_list = []

def print_stats(f):
    avg_ssim_1 = ssim_sum[0] / channels_processed
    avg_ssim_2 = ssim_sum[1] / channels_processed
    
    print("SSIM (higher is better):", file=f)
    print(f"BC7ENC: {avg_ssim_1:.4f}", file=f)
    print(f"DXTCPT: {avg_ssim_2:.4f}\n", file=f)

    avg_rmse_1 = (mse_sum[0] / channels_processed) ** 0.5
    avg_rmse_2 = (mse_sum[1] / channels_processed) ** 0.5
    
    print("RMSE (lower is better):", file=f)
    print(f"BC7ENC: {avg_rmse_1:.4f}", file=f)
    print(f"DXTCPT: {avg_rmse_2:.4f}\n", file=f)

    avg_lpips_1 = lpips_sum[0] / channels_processed if channels_processed else float('nan')
    avg_lpips_2 = lpips_sum[1] / channels_processed if channels_processed else float('nan')
    
    print("LPIPS (lower is better):", file=f)
    print(f"BC7ENC: {avg_lpips_1:.4f}", file=f)
    print(f"DXTCPT: {avg_lpips_2:.4f}", file=f)


for img_idx, src in enumerate(png_files):
    base_name = os.path.splitext(os.path.basename(src))[0]
    dst_bc7 = os.path.join(TEX_BC7ENC_DIR, base_name + ".dds")
    dst_my  = os.path.join(TEX_MYENC_DIR, base_name + ".dds")
    
    def needs_update(src, dst):
        if not os.path.exists(dst):
            return True
        return os.path.getmtime(src) > os.path.getmtime(dst)

    if needs_update(src, dst_bc7):
        subprocess.run(
            ["bc7enc", "-1", "-g", "-q", "-r", "-L18", src, dst_bc7],
            stdout=subprocess.DEVNULL,
            check=True
        )

    if needs_update(src, dst_my):
        subprocess.run(
            ["dxt_cpt", src, dst_my],
            stdout=subprocess.DEVNULL,
            check=True
        )
    
    src_image = numpy.array(Image.open(src))
    dds_image1 = numpy.array(Image.open(dst_bc7))
    dds_image2 = numpy.array(Image.open(dst_my))
    
    if len(src_image.shape) != 3: continue
    
    tex_lpips = [0, 0]
    tex_ssim = [0, 0]
    
    img_channels = 0
    
    def bw2tensor(ch_np):
        return (torch.Tensor(ch_np) * 2 / 255 - 1).to(device)
    
    for dim in range(max(3, img_channels)):
        src_ch = src_image[:, :, dim]
        img_channels += 1
        
        src_tensor = bw2tensor(src_ch)
        
        for i, dds_img in enumerate([dds_image1, dds_image2]):
            dds_ch = dds_img[:, :, dim]
            
            tex_lpips[i] += loss_fn.forward(
                src_tensor,
                bw2tensor(dds_ch)
            ).item()
            
            tex_ssim[i] += metrics.structural_similarity(
                src_ch, dds_ch, 
                win_size = 11, data_range = 255, 
                gaussian_weights=True, sigma=1.5
            )
            
            mse_sum[i] += metrics.mean_squared_error(src_ch, dds_ch)
    
    lpips_sum[0] += tex_lpips[0]
    lpips_sum[1] += tex_lpips[1]
    
    ssim_sum[0] += tex_ssim[0]
    ssim_sum[1] += tex_ssim[1]
    
    channels_processed += img_channels
    images_processed += 1
    
    results_list.append([
        (tex_lpips[1] - tex_lpips[0]) / img_channels, 
        tex_lpips[0] / img_channels,
        tex_lpips[1] / img_channels,
        (tex_ssim[1] - tex_ssim[0]) / img_channels, 
        tex_ssim[0] / img_channels,
        tex_ssim[1] / img_channels,
        base_name
    ])
    
    #Print current stats
    print(f"{img_idx + 1} / {len(png_files)}\n")
    print_stats(None)
    print("")

#Save sorted stats
def sort_by_lpips(e): return e[0]
results_list.sort(reverse=False, key=sort_by_lpips)

with open("results_lpips.txt", "w") as f:
    print(f"{datetime.today().year}.{datetime.today().month:02d}.{datetime.today().day:02d}", file=f);
    print("LPIPS2-LPIPS1 LPIPS1 LPIPS2 Texture\n", file=f);
    
    for img_data in results_list:
        print(f"{img_data[0]:.4f} {img_data[1]:.4f} {img_data[2]:.4f} {img_data[6]}", file=f)

def sort_by_ssim(e): return e[3]
results_list.sort(reverse=True, key=sort_by_ssim)

with open("results_ssim.txt", "w") as f:
    print(f"{datetime.today().year}.{datetime.today().month:02d}.{datetime.today().day:02d}", file=f);
    print("SSIM2-SSIM1 SSIM1 SSIM2 Texture\n", file=f);
    
    for img_data in results_list:
        print(f"{img_data[3]:.4f} {img_data[4]:.4f} {img_data[5]:.4f} {img_data[6]}", file=f)

with open("results_stats.txt", "w") as f:
    print(f"{datetime.today().year}.{datetime.today().month:02d}.{datetime.today().day:02d}", file=f);
    print(f"Images processed: {images_processed} ({channels_processed} channels)\n", file=f);
    print_stats(f)
    