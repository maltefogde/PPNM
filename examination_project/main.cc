#include <cmath>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <random>
#include <fstream>
#include <limits>

#include "lanczos.hpp"

lanczos::matrix random_symmetric_matrix(std::size_t n, unsigned seed = 1) {
    std::mt19937 gen(seed);
    std::uniform_real_distribution<double> dist(-1.0, 1.0);

    lanczos::matrix A(static_cast<int>(n), static_cast<int>(n));

    for(std::size_t i = 0; i < n; ++i) {
        for(std::size_t j = i; j < n; ++j) {
            const double x = dist(gen);
            A(i,j) = x;
            A(j,i) = x;
        }
    }

    return A;
}

lanczos::vector random_vector(std::size_t n, unsigned seed = 2) {
    std::mt19937 gen(seed);
    std::uniform_real_distribution<double> dist(-1.0, 1.0);

    lanczos::vector v(n);

    for(std::size_t i = 0; i < n; ++i) {
        v[i] = dist(gen);
    }

    return v;
}

lanczos::matrix hydrogen_hamiltonian(std::size_t N, double dr) {
    lanczos::matrix H(static_cast<int>(N), static_cast<int>(N));

    const double offdiag = -0.5 / (dr * dr);

    for(std::size_t i = 0; i < N; ++i) {
        const double r = (i + 1) * dr;

        H(i,i) = 1.0 / (dr * dr) - 1.0 / r;

        if(i + 1 < N) {
            H(i,i + 1) = offdiag;
            H(i + 1,i) = offdiag;
        }
    }

    return H;
}

double lowest_eigenvalue(lanczos::matrix A) {
    const std::size_t n = A.size1();
    const double tol = 1e-12;
    const std::size_t max_sweeps = 100 * n * n;

    for(std::size_t sweep = 0; sweep < max_sweeps; ++sweep) {
        bool changed = false;

        for(std::size_t p = 0; p < n; ++p) {
            for(std::size_t q = p + 1; q < n; ++q) {
                const double apq = A(p,q);

                if(std::abs(apq) < tol) continue;

                const double app = A(p,p);
                const double aqq = A(q,q);

                const double tau = (aqq - app) / (2.0 * apq);
                const double t = (tau >= 0.0)
                    ? 1.0 / (tau + std::sqrt(1.0 + tau * tau))
                    : -1.0 / (-tau + std::sqrt(1.0 + tau * tau));

                const double c = 1.0 / std::sqrt(1.0 + t * t);
                const double s = t * c;

                A(p,p) = app - t * apq;
                A(q,q) = aqq + t * apq;
                A(p,q) = 0.0;
                A(q,p) = 0.0;

                for(std::size_t i = 0; i < n; ++i) {
                    if(i == p || i == q) continue;

                    const double aip = A(i,p);
                    const double aiq = A(i,q);

                    A(i,p) = c * aip - s * aiq;
                    A(p,i) = A(i,p);

                    A(i,q) = s * aip + c * aiq;
                    A(q,i) = A(i,q);
                }

                changed = true;
            }
        }

        if(!changed) break;
    }

    double Emin = A(0,0);
    for(std::size_t i = 1; i < n; ++i) {
        Emin = std::min(Emin, A(i,i));
    }

    return Emin;
}

lanczos::vector hydrogen_start_vector(std::size_t N, double dr) {
    lanczos::vector q(N);

    for(std::size_t i = 0; i < N; ++i) {
        const double r = (i + 1) * dr;
        q[i] = r * std::exp(-0.5 * r);
    }

    return q;
}

int main() {
    std::cout << std::setprecision(16);

    const std::size_t N = 8;

    const lanczos::matrix A = random_symmetric_matrix(N, 1234);
    const lanczos::vector q0 = random_vector(N, 5678);

    std::cout << "------ TASK A ------\n";
    std::cout << "Random real symmetric matrix A of size N = " << N << "\n\n";

    const auto full = lanczos::tridiagonalize(A, q0, N);
    const lanczos::matrix full_projection = lanczos::project(A, full.V);

    std::cout << "Full tridiagonalization with n = N\n";
    std::cout << "Number of Lanczos vectors      = " << full.iterations << "\n";
    std::cout << "Lanczos breakdown occurred     = "
              << std::boolalpha << full.breakdown << "\n";
    std::cout << "max |V^T V - I|                = "
              << lanczos::orthogonality_error(full.V) << "\n";
    std::cout << "max |V^T A V - T|              = "
              << lanczos::max_abs_difference(full_projection, full.T) << "\n";
    std::cout << "max non-tridiagonal element    = "
              << lanczos::tridiagonal_error(full.T) << "\n\n";

    full.T.print("T for n = N:");

    const std::size_t n = 4;

    const auto lossy = lanczos::tridiagonalize(A, q0, n);
    const lanczos::matrix lossy_projection = lanczos::project(A, lossy.V);

    std::cout << "\nLossy Krylov compression with n = " << n << " < N\n";
    std::cout << "Number of Lanczos vectors      = " << lossy.iterations << "\n";
    std::cout << "max |V^T V - I|                = "
              << lanczos::orthogonality_error(lossy.V) << "\n";
    std::cout << "max |V^T A V - T|              = "
              << lanczos::max_abs_difference(lossy_projection, lossy.T) << "\n";
    std::cout << "max non-tridiagonal element    = "
              << lanczos::tridiagonal_error(lossy.T) << "\n\n";

    lossy.T.print("T for n < N:");


    std::cout << "\n\n------ TASK B ------\n";

    const double dr = 0.1;
    const double rmax = 20.0;
    const std::size_t Nh = static_cast<std::size_t>(rmax / dr);

    const lanczos::matrix H = hydrogen_hamiltonian(Nh, dr);
    //const lanczos::vector qh = random_vector(Nh, 4321);
    const lanczos::vector qh = hydrogen_start_vector(Nh, dr);

    const double E_exact = -0.5;
    const double E_full = lowest_eigenvalue(H);

    std::ofstream data("hydrogen_convergence.dat");
    data << "# n E0_Lanczos abs_error_to_full abs_error_to_exact\n";

    const std::size_t nmax = std::min<std::size_t>(Nh, 80);

    double best_E0 = 0.0;
    double best_error_full = std::numeric_limits<double>::infinity();
    double best_error_exact = 0.0;
    std::size_t best_n = 0;

    for(std::size_t n = 2; n <= nmax; n += 2) {
        const auto res = lanczos::tridiagonalize(H, qh, n);

        const double E0 = lowest_eigenvalue(res.T);
        const double error_full = std::abs(E0 - E_full);
        const double error_exact = std::abs(E0 - E_exact);

        data << res.iterations << " "
            << E0 << " "
            << error_full << " "
            << error_exact << "\n";

        if(error_full < best_error_full) {
            best_error_full = error_full;
            best_error_exact = error_exact;
            best_E0 = E0;
            best_n = res.iterations;
        }
    }

    std::cout << "Hydrogen matrix size N = " << Nh << "\n";
    std::cout << "dr = " << dr << ", rmax = " << rmax << "\n";
    std::cout << "Exact continuum ground state energy = " << E_exact << "\n";
    std::cout << "Full finite-difference ground state = " << E_full << "\n";
    std::cout << "Discretization error |E_full + 0.5| = "
            << std::abs(E_full - E_exact) << "\n\n";
    std::cout << "Best Lanczos value = " << best_E0 << "\n";
    std::cout << "Best Lanczos dimension n = " << best_n << "\n";
    std::cout << "Lanczos error |E0(T_n)-E_full| = " << best_error_full << "\n";
    std::cout << "Total error |E0(T_n)+0.5| = " << best_error_exact << "\n\n";
    std::cout << "Wrote convergence data to hydrogen_convergence.dat\n";
    std::cout << "See convergence plot: hydrogen_convergence.svg \n";
    std::cout << "See error plot: hydrogen_error.svg \n\n";

    return 0;
}