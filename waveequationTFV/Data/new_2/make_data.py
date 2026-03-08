import numpy as np
from scipy.special import j0, j1

# ----------------------------
# parameters
# ----------------------------
NX   = 1000
NINT = 2000
mu   = np.sqrt(3.0)/4.0
times = [0.0, 0.25, 0.5, 0.75, 1.0]

xgrid = np.linspace(0.0, 1.0, NX, endpoint=False)

# ----------------------------
# initial data for v
# ----------------------------
def f0(y):
    """v(x,0) on the fundamental domain [0,1)"""
    return np.exp(-y/4.0) * np.sin(2.0 * np.pi * y)

def f_twist(x):
    """twist-periodic extension: v(x+1)=exp(-1/4)v(x)"""
    k = np.floor(x).astype(int)
    y = x - k
    mask = y < 0
    y[mask] += 1
    k[mask] -= 1
    return np.exp(-k/4.0) * f0(y)

# ----------------------------
# compute and save data
# ----------------------------
for t in times:
    uvals = np.zeros_like(xgrid)

    for i, x in enumerate(xgrid):

        # main d'Alembert term
        v = 0.5 * (f_twist(np.array([x+t]))[0]
                 + f_twist(np.array([x-t]))[0])

        if t > 0.0:
            s = np.linspace(x-t, x+t, NINT)
            r2 = t*t - (x - s)**2
            mask = r2 > 0

            r = np.sqrt(r2[mask])
            z = mu * r

            integ0 = np.trapz(f_twist(s[mask]) * j0(z), s[mask])
            integ1 = np.trapz(f_twist(s[mask]) * j1(z) / r, s[mask])

            v += 0.25 * integ0
            v += (mu /2.0) * integ1

        # back transform
        uvals[i] = np.exp(-0.5*t + 0.25*x) * v

    # save data
    filename = f"u_t{t:.2f}_sin_exact.dat"
    np.savetxt(
        filename,
        np.column_stack((xgrid, uvals)),
        header="x u(x,t)",
    )

    print(f"saved: {filename}")
