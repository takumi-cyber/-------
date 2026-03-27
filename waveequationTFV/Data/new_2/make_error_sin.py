import numpy as np
from scipy.interpolate import interp1d
import matplotlib.pyplot as plt

# ----------------------------
# 設定
# ----------------------------
NX_list = [100, 200, 400, 800, 1000]
dx_list = [1.0/NX for NX in NX_list]

t_eval = 1.0

# 解析解ファイル
exact_file = "u_t1.00_sin_exact_nu1000.0mu500.0.dat"
data_exact = np.loadtxt(exact_file)
x_exact = data_exact[:,0]
u_exact = data_exact[:,1]

# ----------------------------
# 誤差格納用
# ----------------------------
errors_abs_L2_exp = []
errors_abs_L2_imp = []
errors_abs_L2_cn  = []

# ----------------------------
# NXごとの誤差計算
# ----------------------------
for NX, dx in zip(NX_list, dx_list):

    # ----- 陽解法 -----
    num_file_exp = f"sin_CFL0.8_t1.00_nu1000.0mu500.0_ex_{NX}.dat"
    data_num_exp = np.loadtxt(num_file_exp)
    x_num_exp = data_num_exp[:,0]
    u_num_exp = data_num_exp[:,1]

    interp_func = interp1d(x_exact, u_exact, kind='linear', fill_value='extrapolate')
    u_exact_on_exp = interp_func(x_num_exp)

    e_exp = u_num_exp - u_exact_on_exp
    err_L2_exp = np.sqrt(np.sum(e_exp**2) * dx)
    errors_abs_L2_exp.append(err_L2_exp)

    # ----- 陰解法 -----
    num_file_imp = f"sin_CFL12.5_t1.00_nu1000.0mu500.0_im_{NX}.dat"
    data_num_imp = np.loadtxt(num_file_imp)
    x_num_imp = data_num_imp[:,0]
    u_num_imp = data_num_imp[:,1]

    u_exact_on_imp = interp_func(x_num_imp)

    e_imp = u_num_imp - u_exact_on_imp
    err_L2_imp = np.sqrt(np.sum(e_imp**2) * dx)
    errors_abs_L2_imp.append(err_L2_imp)

    # ----- CN-CD -----
    num_file_cn = f"sin_CFL12.5_t1.00_nu1000.0mu500.0_CN_CD_{NX}.dat"
    data_num_cn = np.loadtxt(num_file_cn)
    x_num_cn = data_num_cn[:,0]
    u_num_cn = data_num_cn[:,1]

    u_exact_on_cn = interp_func(x_num_cn)

    e_cn = u_num_cn - u_exact_on_cn
    err_L2_cn = np.sqrt(np.sum(e_cn**2) * dx)
    errors_abs_L2_cn.append(err_L2_cn)

    print(f"NX={NX:5d}, Δx={dx:.5e}, Explicit={err_L2_exp:.5e}, Implicit={err_L2_imp:.5e}, CN-CD={err_L2_cn:.5e}")

# ----------------------------
# グラフ
# ----------------------------
plt.rcParams.update({
    'font.size': 24,
    'axes.labelsize': 24,
    'axes.titlesize': 24,
    'legend.fontsize': 12,
    'xtick.labelsize': 16,
    'ytick.labelsize': 16
})

plt.figure(figsize=(6,6))

# Explicit
plt.loglog(dx_list, errors_abs_L2_exp, 'o-', label='Explicit(CFL=0.8)')

# Implicit
plt.loglog(dx_list, errors_abs_L2_imp, 's-', label='Implicit(CFL=12.5)')

# CN-CD
plt.loglog(dx_list, errors_abs_L2_cn, '^-', label='Proposed(CFL=12.5)')

# 参考線
dx0_exp = dx_list[0]
err0_exp = errors_abs_L2_exp[0]

dx0_imp = dx_list[0]
err0_imp = errors_abs_L2_imp[0]

plt.loglog(dx_list, (err0_imp/dx0_imp) * np.array(dx_list), '--', label='1st order')
plt.loglog(dx_list, (err0_exp/(dx0_exp**2)) * np.array(dx_list)**2, ':', label='2nd order')

plt.xlabel(r'$\Delta x$', fontsize=18, labelpad=5)
plt.ylabel(r'$E$', fontsize=18)
plt.ylim(1e-8, 1e1)
plt.legend(loc='lower right')

plt.tight_layout()
plt.savefig("convergence_exp_imp_cn_L2.png", dpi=300)
plt.show()