import numpy as np

# ------------------
# parameters
# ------------------
N = 8192
L = 1.0
c = 1.0
nu = 1000.0
mu = 500.0

times = [0.0, 0.25, 0.5, 0.75, 1.0]

x = np.linspace(0, L, N, endpoint=False)
dx = L/N

# ------------------
# initial condition
# ------------------
def u0(x):
    return np.where((x>=0.4)&(x<=0.6),1.0,0.0)

def ut0(x):
    return np.zeros_like(x)

f = u0(x)
g = ut0(x)

# ------------------
# Fourier transform
# ------------------
fhat = np.fft.fft(f)
ghat = np.fft.fft(g)

k = 2*np.pi*np.fft.fftfreq(N, d=dx)

# ------------------
# time evolution function
# ------------------
def exact_solution(t):

    uhat = np.zeros(N, dtype=complex)

    for j in range(N):

        kj = k[j]

        lam1 = (-nu + np.sqrt(nu**2 - 4*(kj**2 + 1j*mu*kj)))/2
        lam2 = (-nu - np.sqrt(nu**2 - 4*(kj**2 + 1j*mu*kj)))/2

        if abs(lam1-lam2) < 1e-14:
            uhat[j] = fhat[j]*np.exp(lam1*t)
        else:
            A = (ghat[j] - lam2*fhat[j])/(lam1-lam2)
            B = (lam1*fhat[j] - ghat[j])/(lam1-lam2)
            uhat[j] = A*np.exp(lam1*t) + B*np.exp(lam2*t)

    return np.real(np.fft.ifft(uhat))

# ------------------
# compute and save data
# ------------------
for t in times:

    uvals = exact_solution(t)

    filename = f"u_t{t:.2f}_step_exact_nu1000.0mu500.0.dat"

    np.savetxt(
        filename,
        np.column_stack((x, uvals)),
        header="x u(x,t)",
    )

    print(f"saved: {filename}")
