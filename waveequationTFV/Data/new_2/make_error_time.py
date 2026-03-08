import numpy as np
from scipy.interpolate import interp1d
import matplotlib.pyplot as plt

# ----------------------------
# 設定
# ----------------------------
num_files = [
    ("sin_CFL0.8_t1.00_nu1000.0mu500.0_CN_1000.dat", 0.8),
    ("sin_CFL1.25_t1.00_nu1000.0mu500.0_CN_1000.dat", 1.25),
    ("sin_CFL2.5_t1.00_nu1000.0mu500.0_CN_1000.dat", 2.5),
    ("sin_CFL5.0_t1.00_nu1000.0mu500.0_CN_1000.dat", 5.0),
    ("sin_CFL10.0_t1.00_nu1000.0mu500.0_CN_1000.dat", 10.0),
    ("sin_CFL12.5_t1.00_nu1000.0mu500.0_CN_1000.dat", 12.5),
]

# ----------------------------
# 解析解読み込み
# ----------------------------
exact_file = "u_t1.00_sin_exact_nu1000.0mu500.0.dat"
data_exact = np.loadtxt(exact_file)
x_exact = data_exact[:,0]
u_exact = data_exact[:,1]

dt_list = []
err_list = []
CFL_list = []

# ----------------------------
# 誤差評価
# ----------------------------
for fname, CFL in num_files:

    data_num = np.loadtxt(fname)
    x_num = data_num[:,0]
    u_num = data_num[:,1]

    interp_func = interp1d(x_exact, u_exact,
                           kind='linear',
                           fill_value='extrapolate')
    u_exact_on_num = interp_func(x_num)

    e = u_num - u_exact_on_num

    dx = x_num[1] - x_num[0]
    dt = CFL * dx

    err_L2 = np.sqrt(np.sum(e**2) * dx)
    norm_exact = np.sqrt(np.sum(u_exact_on_num**2) * dx)
    err_relL2 = err_L2 / norm_exact

    dt_list.append(dt)
    err_list.append(err_relL2)
    CFL_list.append(CFL)

# numpy array
dt_array = np.array(dt_list)
err_array = np.array(err_list)
CFL_array = np.array(CFL_list)

# ----------------------------
# CFLごとの誤差表示
# ----------------------------
print("\n--- Relative L2 Error ---")
for CFL, err in zip(CFL_array, err_array):
    print(f"CFL = {CFL:8.2f}   Relative L2 = {err:.6e}")

# ----------------------------
# 局所収束率 p_i 計算
# ----------------------------
print("\n--- Local Convergence Order p ---")

for i in range(len(dt_array)-1):

    p_local = (
        np.log(err_array[i+1]/err_array[i])
        /
        np.log(dt_array[i+1]/dt_array[i])
    )

    print(f"CFL {CFL_array[i]:6.2f} → {CFL_array[i+1]:6.2f} :   p = {p_local:.4f}")

# ----------------------------
# 全体の最小二乗p
# ----------------------------
p_global = np.polyfit(np.log(dt_array), np.log(err_array), 1)[0]
print(f"\nGlobal fitted order p = {p_global:.4f}")

# ----------------------------
# グラフ
# ----------------------------
import matplotlib.pyplot as plt

plt.rcParams.update({
    'font.size': 24,          # 基本フォントサイズ
    'axes.labelsize': 24,     # 軸ラベル
    'axes.titlesize': 24,     
    'legend.fontsize': 16,    # 凡例
    'xtick.labelsize': 16,    # x軸目盛り
    'ytick.labelsize': 16     # y軸目盛り
})
ref_dt = np.linspace(min(dt_array), max(dt_array), 200)

C1 = err_array[0] / dt_array[0]
C2 = err_array[0] / dt_array[0]**2

ref1 = C1 * ref_dt
ref2 = C2 * ref_dt**2

plt.figure(figsize=(6,6))
plt.loglog(dt_array, err_array, 'o-', label='Relative L2')
plt.loglog(ref_dt, ref1, '--', label='1st order')
plt.loglog(ref_dt, ref2, '--', label='2nd order')

plt.xlabel('Δt')
plt.xlabel(r'$\Delta t$', fontsize=18)
plt.ylabel(r'$E$', fontsize=18)
#plt.grid(True, which='both', ls='--')
plt.legend(loc='lower right')
# レイアウト自動調整で見切れ防止
plt.tight_layout()

# PNG 保存
plt.savefig("convergence.png", dpi=300)
plt.show()
