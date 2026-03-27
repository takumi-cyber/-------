import numpy as np
from scipy.interpolate import interp1d
import matplotlib.pyplot as plt

# ----------------------------
# 設定
# ----------------------------
NX_list = [100, 200, 400, 800, 1000]
dx_list = [1.0/NX for NX in NX_list]

t_eval = 1.0

# 解析解
exact_file = "u_t1.00_step_exact_nu1.0mu0.5.dat"
data_exact = np.loadtxt(exact_file)
x_exact = data_exact[:,0]
u_exact = data_exact[:,1]

interp_func = interp1d(x_exact, u_exact, kind='linear', fill_value='extrapolate')

# ----------------------------
# 誤差格納
# ----------------------------
errors_L2_exp = []
errors_L2_imp = []
errors_L2_cn  = []

errors_Linf_exp = []
errors_Linf_imp = []
errors_Linf_cn  = []

# ----------------------------
# NXごとの誤差計算
# ----------------------------
for NX, dx in zip(NX_list, dx_list):

    # ----- Explicit -----
    num_file_exp = f"step_CFL0.8_t1.00_nu1.0mu0.5_ex_{NX}.dat"
    data_num_exp = np.loadtxt(num_file_exp)
    x_num_exp = data_num_exp[:,0]
    u_num_exp = data_num_exp[:,1]

    u_exact_on_exp = interp_func(x_num_exp)

    e_exp = u_num_exp - u_exact_on_exp
    err_L2_exp = np.sqrt(np.sum(e_exp**2) * dx)
    err_Linf_exp = np.max(np.abs(e_exp))

    errors_L2_exp.append(err_L2_exp)
    errors_Linf_exp.append(err_Linf_exp)

    # ----- Implicit -----
    num_file_imp = f"step_CFL12.5_t1.00_nu1.0mu0.5_im_{NX}.dat"
    data_num_imp = np.loadtxt(num_file_imp)
    x_num_imp = data_num_imp[:,0]
    u_num_imp = data_num_imp[:,1]

    u_exact_on_imp = interp_func(x_num_imp)

    e_imp = u_num_imp - u_exact_on_imp
    err_L2_imp = np.sqrt(np.sum(e_imp**2) * dx)
    err_Linf_imp = np.max(np.abs(e_imp))

    errors_L2_imp.append(err_L2_imp)
    errors_Linf_imp.append(err_Linf_imp)

    # ----- CN-CD (Proposed method) -----
    num_file_cn = f"step_CFL12.5_t1.00_nu1.0mu0.5_CN_CD_{NX}.dat"
    data_num_cn = np.loadtxt(num_file_cn)
    x_num_cn = data_num_cn[:,0]
    u_num_cn = data_num_cn[:,1]

    u_exact_on_cn = interp_func(x_num_cn)

    e_cn = u_num_cn - u_exact_on_cn
    err_L2_cn = np.sqrt(np.sum(e_cn**2) * dx)
    err_Linf_cn = np.max(np.abs(e_cn))

    errors_L2_cn.append(err_L2_cn)
    errors_Linf_cn.append(err_Linf_cn)

    print(f"NX={NX:5d}, Δx={dx:.5e}, "
          f"Explicit L2={err_L2_exp:.5e}, Explicit Linf={err_Linf_exp:.5e}, "
          f"Implicit L2={err_L2_imp:.5e}, Implicit Linf={err_Linf_imp:.5e}, "
          f"CN-CD L2={err_L2_cn:.5e}, CN-CD Linf={err_Linf_cn:.5e}")

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

# L2
plt.loglog(dx_list, errors_L2_exp, 'o-', label='Explicit(CFL=0.8) $L_2$')
plt.loglog(dx_list, errors_L2_imp, 's-', label='Implicit(CFL=12.5) $L_2$')
plt.loglog(dx_list, errors_L2_cn, '^-', label='Proposed(CFL=12.5) $L_2$')

# Linf
plt.loglog(dx_list, errors_Linf_exp, 'o--', label='Explicit(CFL=0.8) $L_\\infty$')
plt.loglog(dx_list, errors_Linf_imp, 's--', label='Implicit(CFL=12.5) $L_\\infty$')
plt.loglog(dx_list, errors_Linf_cn, '^--', label='Proposed(CFL=12.5) $L_\\infty$')

# 参考線
dx0 = dx_list[0]
err0 = errors_L2_exp[0]   # 基準はどれでもいい（見た目調整用）

plt.loglog(
    dx_list,
    (err0 / (dx0**0.5)) * np.array(dx_list)**0.5,
    ':',
    color='black',
    label='0.5th order'
)

plt.xlabel(r'$\Delta x$')
plt.ylabel(r'$E$')
plt.ylim(1e-7, 1e1)
plt.legend(loc='lower right')

plt.tight_layout()
plt.savefig("convergence_exp_imp_cn_L2_Linf.png", dpi=300)
plt.show()