import numpy as np
from scipy.integrate import quad

acc = 1e-6
eps = 1e-6


def run_test(name, f, a, b, exact):
    result = quad(
        f,
        a,
        b,
        epsabs=acc,
        epsrel=eps,
        full_output=True,
        limit=1000,
    )

    q = result[0]
    estimated_error = result[1]
    info = result[2]

    error = abs(q - exact)
    ncalls = info["neval"]

    print(name)
    print(f"  scipy result          = {q:.16g}")
    print(f"  exact                 = {exact:.16g}")
    print(f"  actual error          = {error:.16g}")
    print(f"  scipy estimated error = {estimated_error:.16g}")
    print(f"  scipy ncalls          = {ncalls}")
    print()


print("------ PYTHON / SCIPY COMPARISON ------\n")

run_test(
    "Integral of 1/sqrt(x) from 0 to 1",
    lambda x: 0.0 if x == 0.0 else 1.0 / np.sqrt(x),
    0.0,
    1.0,
    2.0,
)

run_test(
    "Integral of log(x)/sqrt(x) from 0 to 1",
    lambda x: 0.0 if x == 0.0 else np.log(x) / np.sqrt(x),
    0.0,
    1.0,
    -4.0,
)

run_test(
    "Integral of 1/sqrt(1 - x) from 0 to 1",
    lambda x: 0.0 if x == 1.0 else 1.0 / np.sqrt(1.0 - x),
    0.0,
    1.0,
    2.0,
)

run_test(
    "Integral of exp(-x) from 0 to infinity",
    lambda x: np.exp(-x),
    0.0,
    np.inf,
    1.0,
)

run_test(
    "Integral of 1/(1 + x^2) from -infinity to infinity",
    lambda x: 1.0 / (1.0 + x*x),
    -np.inf,
    np.inf,
    np.pi,
)

run_test(
    "Integral of exp(-x^2) from -infinity to infinity",
    lambda x: np.exp(-x*x),
    -np.inf,
    np.inf,
    np.sqrt(np.pi),
)