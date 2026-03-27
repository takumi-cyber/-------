import numpy as np
from scipy.interpolate import interp1d
import matplotlib.pyplot as plt

# ----------------------------
# 設定
# ----------------------------
NX_list = [100, 200, 400, 600, 800, 1000]
dx_list = [1.0/NX for NX in NX_list]

t_eval = 1.0

# 解析解ファイル
exact_file = "u_t1.00_step_exact_nu1.0mu0.5.dat"
data_exact = np.loadtxt(exact_file)
x_exact = data_exact[:,0]
u_exact = data_exact[:,1]

# ----------------------------
# 誤差格納用
# ----------------------------
errors_abs_L2 = []
errors_Linf = []

# ----------------------------
# NXごとの誤差計算
# ----------------------------
for NX, dx in zip(NX_list, dx_list):

    # 数値解ファイル
    num_file = f"step_CFL12.5_t1.00_nu1.0mu0.5_CN_CD_{NX}.dat"
    data_num = np.loadtxt(num_file)
    x_num = data_num[:,0]
    u_num = data_num[:,1]

    # 解析解を数値格子へ補間
    interp_func = interp1d(x_exact, u_exact,
                           kind='linear',
                           fill_value='extrapolate')
    u_exact_on_num = interp_func(x_num)

    # 誤差
    e = u_num - u_exact_on_num

    # ---- 絶対 L2 誤差
    err_L2 = np.sqrt(np.sum(e**2) * dx)
    errors_abs_L2.append(err_L2)

    # ---- L∞誤差
    err_Linf = np.max(np.abs(e))
    errors_Linf.append(err_Linf)

    print(f"NX={NX:5d}, Δx={dx:.5e}, Absolute L2={err_L2:.5e}, L∞={err_Linf:.5e}")

# ----------------------------
# 空間収束率（絶対 L2）
# ----------------------------
print("\nAbsolute L2 収束率:")
for i in range(len(errors_abs_L2)-1):
    p = np.log(errors_abs_L2[i] / errors_abs_L2[i+1]) / np.log(dx_list[i] / dx_list[i+1])
    print(f"NX={NX_list[i]} -> NX={NX_list[i+1]} : p = {p:.2f}")

# ----------------------------
# グラフ（絶対 L2）
# ----------------------------
plt.rcParams.update({
    'font.size': 24,
    'axes.labelsize': 24,
    'axes.titlesize': 24,
    'legend.fontsize': 18,
    'xtick.labelsize': 16,
    'ytick.labelsize': 16
})

plt.figure(figsize=(6,6))

# 絶対L2誤差とL∞誤差
plt.loglog(dx_list, errors_abs_L2, 'o-', label=r'Absolute $L_2$')
plt.loglog(dx_list, errors_Linf, 's-', label=r'$L_\infty$')

#参考直線用の係数（最初の点に合わせる）
C1 = errors_abs_L2[0] / dx_list[0]          # 1次用
C05 = errors_abs_L2[0] / (dx_list[0]**0.5)  # 0.5次用

#一次、二次精度用
#dx0 = dx_list[0]
#err0 = errors_abs_L2[0]

# 1次線
#plt.loglog(dx_list, (err0/dx0) * np.array(dx_list), '--', label='1st order')

# 2次線
#plt.loglog(dx_list, (err0/(dx0**2)) * np.array(dx_list)**2, ':', label='2nd order')

#一次、0.5次精度用
plt.loglog(dx_list, C1*np.array(dx_list), '--', label='1st order')
plt.loglog(dx_list, C05*np.array(dx_list)**0.5, ':', label='0.5 order')

# ----------------------------
plt.xlabel(r'$\Delta x$', fontsize=18, labelpad=5)  
plt.ylabel(r'$E$', fontsize=18)                     
plt.legend()
plt.tight_layout()
plt.savefig("convergence_abs_L2.png", dpi=300)
plt.show()
