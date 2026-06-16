Malte Fogde Mikkelsen, 202308803, Examination project: Lanczos tridiagonalization algorithm

https://github.com/maltefogde/PPNM

Suggested grading: 12


## Task A
I implemented the Lanczos tridiagonalization algorithm for real symmetric matrices using the description from `https://en.wikipedia.org/wiki/Lanczos_algorithm`.  The implementation was tested on a random real symmetric matrix of size $N=8$. I tested both the full case $n=N$ and a lossy Krylov compression with $n<N$.
The output in `Out.txt` shows that the Lanczos vectors are orthonormal to numerical precision,

$$ \max |V^T V-I| \sim 10^{-13}, $$

for $N=8$, and that the projected matrix agrees with the computed tridiagonal matrix,

$$ \max |V^T A V - T| \sim 10^{-13}. $$

The output also shows that the maximum non-tridiagonal element of $T$ is zero. This confirms that the algorithm produces the expected tridiagonal representation.

Note that matrix $T_n$ obtained for $n<N$ is the leading principal submatrix of the matrix $T_N$ obtained by continuing to $n=N$ such $T_n=V_n^TAV_n$ is the upper-left $n\times n$ block of $T_N$. This is shown in `Out.txt`.

## Task B
I used the finite-difference discretization of the radial hydrogen Hamiltonian with

$$ \Delta r = 0.1, \qquad r_{\max}=20, $$

which gives a matrix size $N=200$. I then used the Lanczos algorithm to construct tridiagonal matrices $T_n$ of increasing size $n\leq 50$ because the convergence is already clear in this interval. I also performed an additional numerical check at $n=100$, not included in the plot, to verify that the error continues to decrease beyond the plotted interval. I used a smooth hydrogen-like starting vector, $q_i=r_i e^{-r_i/2}$, that is not meant to be the exact ground-state vector. It is used to make the convergence easier to see. For each $T_n$, I computed the lowest eigenvalue and compared it with the lowest eigenvalue of the full discretized Hamiltonian matrix $H$.

The full finite-difference Hamiltonian gives

$$ E_0(H) = -0.498756211208868, $$

whereas the exact continuum value is $E_0=-0.5$. The difference

$$ |E_0(H)+0.5| = 0.001243788791131961 $$

is therefore due to the finite-difference discretization, not the Lanczos algorithm.
The best Lanczos result in `Out.txt` is obtained for $n=50$,

$$ E_0(T_{50}) = -0.4987314493158973, $$

with Lanczos error

$$ |E_0(T_{50})-E_0(H)| \sim 10^{-5}. $$

This shows that the Lanczos approximation has already converged well to the lowest eigenvalue of the discretized Hamiltonian on the scale relevant for the finite-difference error. The remaining difference from $-0.5$ is caused by the discretization of the continuum hydrogen problem and can be improved by decreasing $\Delta r$ and/or increasing $r_{\max}$.
The convergence is shown in `hydrogen_convergence.svg`, where $E_0(T_n)$ approaches the full matrix value $E_0(H)$. The absolute Lanczos error is shown in `hydrogen_error.svg`. The approximately linear behaviour in the semi-logarithmic error plot indicates nearly exponential convergence of the lowest Lanczos eigenvalue. For larger $n$ obtained by increasing $n_\max$, the error is not expected to decrease indefinitely, since roundoff error and possible loss of orthogonality of the Lanczos vectors eventually become relevant. 

The orthogonality error remained small in the tested range, including the additional test $n=100$. For much larger Krylov dimensions, loss of orthogonality can become relevant in finite precision, but this is not the limiting error in the results shown here.


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

This does not mean that tridiagonalisation is useless for eigenvalue problems. The standard use of tridiagonalisation is instead to combine it with methods that preserve or exploit tridiagonal form, such as QR/QL methods for symmetric tridiagonal matrices. In contrast, ordinary cyclic Jacobi rotations destroy the tridiagonal sparsity pattern, as shown above, and therefore the matrix must essentially be treated as dense after the first rotations.