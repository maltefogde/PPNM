Malte Fogde Mikkelsen, 202308803, Examination project: Lanczos tridiagonalization algorithm

https://github.com/maltefogde/PPNM

Suggested grading: 12


## Task A
I implemented the Lanczos tridiagonalization algorithm for real symmetric matrices using the description from `https://en.wikipedia.org/wiki/Lanczos_algorithm`.  The implementation was tested on a random real symmetric matrix of size $N=8$. I tested both the full case $n=N$ and a lossy Krylov compression with $n<N$.
The output in `Out.txt` shows that the Lanczos vectors are orthonormal to numerical precision,

$$ \max |V^T V-I| \sim 10^{-13}, $$

for $N=8$., and that the projected matrix agrees with the computed tridiagonal matrix,

$$ \max |V^T A V - T| \sim 10^{-13}. $$

The output also shows that the maximum non-tridiagonal element of $T$ is zero. This confirms that the algorithm produces the expected tridiagonal representation.

## Task B
I used the finite-difference discretization of the radial hydrogen Hamiltonian with

$$ \Delta r = 0.1, \qquad r_{\max}=20, $$

which gives a matrix size $N=200$. I then used the Lanczos algorithm to construct tridiagonal matrices $T_n$ of increasing size $n\leq 100$. I choose a starting vector $\vec{q}$ close to the exact solution for the algorithm to work more efficiently. For each $T_n$, I computed the lowest eigenvalue and compared it with the lowest eigenvalue of the full discretized Hamiltonian matrix $H$.

The full finite-difference Hamiltonian gives

$$ E_0(H) = -0.4987562112088574, $$

whereas the exact continuum value is $ E_0 = -0.5. $ The difference

$$ |E_0(H)+0.5| = 0.001243788791142564 $$

is therefore due to the finite-difference discretization, not the Lanczos algorithm.
The best Lanczos result in `Out.txt` is obtained for $n=100$,

$$ E_0(T_{100}) = -0.4987562085704186, $$

with Lanczos error

$$ |E_0(T_{100})-E_0(H)| = 2.638438800772747\times 10^{-9}. $$

This shows that the Lanczos approximation converges very accurately to the lowest eigenvalue of the discretized Hamiltonian. The remaining difference from $-0.5$ is caused by the discretization of the continuum hydrogen problem and can be improved by changing $\Delta r$ $r_{\max}=20$.
The convergence is shown in `hydrogen_convergence.svg`, where $E_0(T_n)$ approaches the full matrix value $E_0(H)$. The absolute Lanczos error is shown in `hydrogen_error.svg`. The approximately linear behaviour in the semi-logarithmic error plot indicates nearly exponential convergence of the lowest Lanczos eigenvalue. This behaviour will continue for $n>100$ until the double-precision numerical accuracy is reached.

## Task C
The Lanczos algorithm produces a tridiagonal matrix $T$. This might initially seem useful for the Jacobi eigenvalue algorithm, since only the diagonal and first off-diagonal elements are nonzero. One might therefore try to restrict the Jacobi rotations to neighbouring indices $(p,p+1)$ such that the computation becomes easier.

However, ordinary Jacobi rotations do not preserve tridiagonal form.

This is shown in `Out.txt`, but I also show it mathematically here from a simple example. Consider a Jacobi rotation in the $(0,1)$-plane used to eliminate $T_{01}$. Before the rotation, the tridiagonal structure gives $T_{02}=0,$ but in general $T_{12}\neq 0$. After the Jacobi rotation, rows and columns 0 and 1 are mixed. The new matrix element $T'_{02}$ becomes

$$ T'_{02}=cT_{02}-sT_{12}. $$

Since $T_{02}=0$, this reduces to

$$ T'_{02}=-sT_{12}. $$

For a non-trivial rotation, $s\neq0$, and therefore

$$ T'_{02}\neq0. $$

Thus, a Jacobi rotation creates a nonzero element outside the tridiagonal band. In other words, the matrix no longer remains tridiagonal. This is also demonstrated numerically in `Out.txt`: the maximum non-tridiagonal element of $T$ is initially zero, but after one Jacobi rotation on neighbouring indices it becomes nonzero.

Therefore, the standard Jacobi eigenvalue algorithm cannot be efficiently tuned to fully exploit the tridiagonal form. It can still diagonalize the matrix, but after the first rotations the matrix must essentially be treated as dense. 