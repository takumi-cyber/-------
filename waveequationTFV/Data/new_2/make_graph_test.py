import numpy as np
import matplotlib.pyplot as plt
from scipy.special import j0, j1, i0, i1

# parameters
c = 1.0
mu = 0.5
nu = 1.0
k = 0.25*(mu**2 - nu**2)

NX = 200
NINT = 1000

xs = np.linspace(0,1,NX,endpoint=False)

# -----------------------------
# fundamental initial function
# -----------------------------
def phi0(x):
    return np.exp(-0.5*mu*x)*np.where((x>=0.4)&(x<=0.6),1.0,0.0)

def psi0(x):
    return np.exp(-0.5*mu*x)*0.5*nu*np.where((x>=0.4)&(x<=0.6),1.0,0.0)

# -----------------------------
# twist periodic extension
# v(x+1)=exp(-μ/2)v(x)
# -----------------------------
def twist_phi(x):

    kshift = np.floor(x).astype(int)
    y = x - kshift

    mask = y < 0
    y[mask] += 1
    kshift[mask] -= 1

    return np.exp(-0.5*mu*kshift)*phi0(y)

def twist_psi(x):

    kshift = np.floor(x).astype(int)
    y = x - kshift

    mask = y < 0
    y[mask] += 1
    kshift[mask] -= 1

    return np.exp(-0.5*mu*kshift)*psi0(y)

# -----------------------------
# trapezoidal rule
# -----------------------------
def trapz_integral(f,a,b,n):

    s = np.linspace(a,b,n)
    return np.trapz(f(s),s)

# -----------------------------
# solution for v
# -----------------------------
def v(t,x):

    term1 = 0.5*(twist_phi(np.array([x+t]))[0] +
                 twist_phi(np.array([x-t]))[0])

    if t == 0:
        return term1

    a = x-t
    b = x+t

    def f2(s):

        inside = t*t-(x-s)**2
        inside = np.maximum(inside,0)

        if k>=0:
            z = np.sqrt(k*inside)
            return twist_psi(s)*j0(z)
        else:
            z = np.sqrt(-k*inside)
            return twist_psi(s)*i0(z)

    term2 = 0.5*trapz_integral(f2,a,b,NINT)

    def f3(s):

        inside = t*t-(x-s)**2
        inside = np.maximum(inside,0)

        r = np.sqrt(inside)
        r = np.where(r==0,1e-14,r)

        if k>=0:
            z = np.sqrt(k*inside)
            return t*twist_phi(s)/r*j1(z)
        else:
            z = np.sqrt(-k*inside)
            return t*twist_phi(s)/r*i1(z)

    term3 = np.sqrt(abs(k))/2*trapz_integral(f3,a,b,NINT)

    return term1+term2+term3


# -----------------------------
# transform back to u
# -----------------------------
def u(t,x):

    return np.exp(-0.5*nu*t+0.5*mu*x)*v(t,x)


# -----------------------------
# compute and plot
# -----------------------------
times=[0,0.25,0.5,0.75,1.0]

plt.figure()

for t in times:

    vals = np.array([u(t,x) for x in xs])

    filename=f"u_t{t:.2f}.dat"

    np.savetxt(
        filename,
        np.column_stack((xs,vals)),
        header="x u(x,t)"
    )

    print("saved:",filename)

    plt.plot(xs,vals,label=f"t={t}")

plt.xlabel("x")
plt.ylabel("u(t,x)")
plt.title("Periodic solution of damped advection wave equation")
plt.legend()
plt.grid()

plt.show()