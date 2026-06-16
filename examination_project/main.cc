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

struct jacobi_result {
    lanczos::vector w;
    std::size_t sweeps;
    std::size_t rotations;
    std::size_t inspected_pairs;
    double offdiag_norm;
};

double offdiag_norm(const lanczos::matrix& A) {
    const std::size_t n = A.size1();

    double sum = 0.0;

    for(std::size_t i = 0; i < n; ++i) {
        for(std::size_t j = i + 1; j < n; ++j) {
            sum += 2.0 * A(i,j) * A(i,j);
        }
    }

    return std::sqrt(sum);
}

jacobi_result jacobi(lanczos::matrix A, std::size_t nvals = 0) {
    const std::size_t n = A.size1();
    if(nvals == 0) nvals = n;

    const double tol = 1e-12;

    std::size_t sweeps = 0;
    std::size_t rotations = 0;
    std::size_t inspected_pairs = 0;

    bool changed;

    do {
        changed = false;
        ++sweeps;

        for(std::size_t p = 0; p < nvals; ++p) {
            for(std::size_t q = p + 1; q < n; ++q) {
                ++inspected_pairs;

                const double app = A(p,p);
                const double aqq = A(q,q);
                const double apq = A(p,q);

                if(std::abs(apq) < tol) continue;

                const double phi = 0.5 * std::atan2(2.0 * apq, aqq - app);
                const double c = std::cos(phi);
                const double s = std::sin(phi);

                const double app1 = c * c * app - 2.0 * s * c * apq + s * s * aqq;
                const double aqq1 = s * s * app + 2.0 * s * c * apq + c * c * aqq;

                if(app1 != app || aqq1 != aqq) {
                    changed = true;
                    ++rotations;

                    A(p,p) = app1;
                    A(q,q) = aqq1;
                    A(p,q) = 0.0;

                    for(std::size_t i = 0; i < p; ++i) {
                        const double aip = A(i,p);
                        const double aiq = A(i,q);

                        A(i,p) = c * aip - s * aiq;
                        A(i,q) = c * aiq + s * aip;
                    }

                    for(std::size_t i = p + 1; i < q; ++i) {
                        const double api = A(p,i);
                        const double aiq = A(i,q);

                        A(p,i) = c * api - s * aiq;
                        A(i,q) = c * aiq + s * api;
                    }

                    for(std::size_t i = q + 1; i < n; ++i) {
                        const double api = A(p,i);
                        const double aqi = A(q,i);

                        A(p,i) = c * api - s * aqi;
                        A(q,i) = c * aqi + s * api;
                    }
                }
            }
        }

    } while(changed);

    lanczos::vector w(n);
    for(std::size_t i = 0; i < n; ++i) {
        w[i] = A(i,i);
    }

    return {w, sweeps, rotations, inspected_pairs, offdiag_norm(A)};
}

double lowest_eigenvalue(lanczos::matrix A) {
    const auto res = jacobi(A);

    double Emin = res.w[0];

    for(std::size_t i = 1; i < res.w.size(); ++i) {
        Emin = std::min(Emin, res.w[i]);
    }

    return Emin;
}

lanczos::matrix one_jacobi_rotation(lanczos::matrix A, std::size_t p, std::size_t q) {
    const double app = A(p,p);
    const double aqq = A(q,q);
    const double apq = A(p,q);

    const double phi = 0.5 * std::atan2(2.0 * apq, aqq - app);
    const double c = std::cos(phi);
    const double s = std::sin(phi);

    const std::size_t n = A.size1();

    for(std::size_t i = 0; i < n; ++i) {
        if(i == p || i == q) continue;

        const double aip = A(i,p);
        const double aiq = A(i,q);

        A(i,p) = c * aip - s * aiq;
        A(p,i) = A(i,p);

        A(i,q) = s * aip + c * aiq;
        A(q,i) = A(i,q);
    }

    A(p,p) = c * c * app - 2.0 * s * c * apq + s * s * aqq;
    A(q,q) = s * s * app + 2.0 * s * c * apq + c * c * aqq;

    A(p,q) = 0.0;
    A(q,p) = 0.0;

    return A;
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

    const std::size_t nmax = std::min<std::size_t>(Nh, 100);

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


    std::cout << "\n\n------ TASK C ------\n";

    const std::size_t nC = 8;
    const auto resC = lanczos::tridiagonalize(H, qh, nC);

    const double before = lanczos::tridiagonal_error(resC.T);

    const lanczos::matrix T_rotated = one_jacobi_rotation(resC.T, 0, 1);

    const double after = lanczos::tridiagonal_error(T_rotated);

    std::cout << "Testing one Jacobi rotation on Lanczos tridiagonal matrix T\n";
    std::cout << "Lanczos dimension n = " << nC << "\n";
    std::cout << "Initial max non-tridiagonal element  = " << before << "\n\n";
    std::cout << "After rotation on indices (0,1):\n";
    std::cout << "max non-tridiagonal element          = " << after << "\n";

    return 0;
}