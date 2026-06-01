#include <cmath>
#include <cstddef>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>

#include "mc.hpp"

const double pi = std::acos(-1.0);

double unit_circle(const std::vector<double>& x) {
    return (x[0] * x[0] + x[1] * x[1] <= 1.0) ? 1.0 : 0.0; // inside 1.0, outside 0.0
}

double ellipsoid(const std::vector<double>& x) { 
    const double a = 1.0; 
    const double b = 2.0;
    const double c = 3.0;

    const double q =
        x[0] * x[0] / (a * a)
        + x[1] * x[1] / (b * b)
        + x[2] * x[2] / (c * c);

    return (q <= 1.0) ? 1.0 : 0.0;
}
// follows directly from homework description

double smooth_test(const std::vector<double>& x) {
    return x[0] * x[0] + x[1] * x[1];
}

double singular_integrand(const std::vector<double>& x) {
    const double cx = std::cos(pi * x[0]);
    const double cy = std::cos(pi * x[1]);
    const double cz = std::cos(pi * x[2]);

    return 1.0 / (1.0 - cx * cy * cz);
}

double loglog_slope(const std::vector<double>& xs, const std::vector<double>& ys) {
    double sx = 0.0;   // sum of log(x)
    double sy = 0.0;   // sum of log(y)
    double sxx = 0.0;  // sum of log(x)^2
    double sxy = 0.0;  // sum of log(x)*log(y)
    std::size_t n = 0; // number of valid points

    for(std::size_t i = 0; i < xs.size(); ++i) {
        if(xs[i] > 0.0 && ys[i] > 0.0) {
            const double x = std::log(xs[i]);
            const double y = std::log(ys[i]);

            sx += x;
            sy += y;
            sxx += x * x;
            sxy += x * y;
            ++n;
        }
    }

    const double nd = static_cast<double>(n);

    return (nd * sxy - sx * sy) / (nd * sxx - sx * sx); // Just standard least squares
}

int main(int argc, char** argv) {
    bool do_taskA = false;
    bool do_taskB = false;
    bool do_taskC = false;

    for(int i = 1; i < argc; ++i) {
        if(std::strcmp(argv[i], "-taskA") == 0) {
            do_taskA = true;
        } else if(std::strcmp(argv[i], "-taskB") == 0) {
            do_taskB = true;
        } else if(std::strcmp(argv[i], "-taskC") == 0) {
            do_taskC = true;
        }
    }

    if(!do_taskA && !do_taskB && !do_taskC) {
        do_taskA = do_taskB = do_taskC = true;
    }

    std::cout << std::setprecision(16);

    if(do_taskA) {
        std::cout << "\n------ TASK A: plain Monte Carlo ------\n\n";

        const std::vector<double> circle_a = {-1.0, -1.0};
        const std::vector<double> circle_b = { 1.0,  1.0};

        const std::vector<std::size_t> Ns = {
            100, 300, 1000, 3000, 10000, 30000, 100000, 300000
        };

        const int repeats = 20;

        std::ofstream file("A_circle_errors.dat");
        file << "# N estimate estimated_error actual_error reference_1_over_sqrt_N\n";

        std::vector<double> N_for_fit;
        std::vector<double> actual_for_fit;

        double first_actual_error = 0.0;
        for(const std::size_t N : Ns) {
            double estimate_sum = 0.0;
            double estimated_error_sum = 0.0;
            double actual_error_sum = 0.0;

            for(int r = 0; r < repeats; ++r) {
                mc::lcg rng(1234u + static_cast<unsigned int>(17 * r));

                const mc::result res = mc::plainmc(
                    unit_circle,
                    circle_a,
                    circle_b,
                    N,
                    rng
                );

                estimate_sum += res.value;
                estimated_error_sum += res.error;
                actual_error_sum += std::abs(res.value - pi);
            }

            const double estimate = estimate_sum / repeats;
            const double estimated_error = estimated_error_sum / repeats;
            const double actual_error = actual_error_sum / repeats;

            if(N == Ns.front()) {
                first_actual_error = actual_error;
            }

            const double reference =
                first_actual_error * std::sqrt(
                    static_cast<double>(Ns.front()) / static_cast<double>(N) 
                ); // Using $\mathrm{error} \propto \frac{1}{\sqrt{N}}$ to find the expected error scaling

            file << N << " "
                 << estimate << " "
                 << estimated_error << " "
                 << actual_error << " "
                 << reference << "\n";

            N_for_fit.push_back(static_cast<double>(N));
            actual_for_fit.push_back(actual_error);
        }

        const double slope = loglog_slope(N_for_fit, actual_for_fit);
        // Why is is smart? Because using $\mathrm{error} \propto \frac{1}{\sqrt{N}}$ then
        // $\log(\mathrm{error}) = \log( C N^{-1/2}) = \log(C) - \frac{1}{2} \log(N)$, so expected slope of -0.5

        std::cout << "Area of unit circle\n";
        std::cout << "Exact value                  = " << pi << "\n";
        std::cout << "Data written to              = A_circle_errors.dat\n";
        std::cout << "Fitted slope of actual error = " << slope << "\n";
        std::cout << "Expected slope               = -0.5\n\n";

        std::cout << "Log-log plot of these errors can be found in A_circle_errors.svg\n\n";

        // Second part of (a)
        const double ea = 1.0;
        const double eb = 2.0;
        const double ec = 3.0;

        const double exact_ellipsoid = 4.0 * pi * ea * eb * ec / 3.0;

        const std::vector<double> ellipsoid_a = {-ea, -eb, -ec};
        const std::vector<double> ellipsoid_b = { ea,  eb,  ec};

        mc::lcg rng(999u);

        const mc::result ell = mc::plainmc(
            ellipsoid,
            ellipsoid_a,
            ellipsoid_b,
            1000000,
            rng
        );

        std::cout << "Volume of ellipsoid with a=1, b=2, c=3\n";
        std::cout << "Monte Carlo estimate         = " << ell.value << "\n";
        std::cout << "Estimated error              = " << ell.error << "\n";
        std::cout << "Exact value                  = " << exact_ellipsoid << "\n";
        std::cout << "Actual error                 = "
                  << std::abs(ell.value - exact_ellipsoid) << "\n\n";
    }

    if(do_taskB) {
        std::cout << "\n------ TASK B: quasi-random sequences ------\n\n";

        const std::vector<double> a2 = {0.0, 0.0};
        const std::vector<double> b2 = {1.0, 1.0};

        const double exact_smooth = 2.0 / 3.0;

        const std::vector<std::size_t> Ns = {
            100, 300, 1000, 3000, 10000, 30000, 100000, 300000
        };

        const int repeats = 20;

        std::ofstream file("B_scaling.dat");
        file << "# N LCG_actual_error std_actual_error quasi_actual_error quasi_estimated_error\n";

        for(const std::size_t N : Ns) {
            double lcg_actual_sum = 0.0;
            double std_actual_sum = 0.0;

            for(int r = 0; r < repeats; ++r) {
                mc::lcg rng1(100u + static_cast<unsigned int>(r));
                mc::std_rng rng2(200u + static_cast<unsigned int>(r));

                const mc::result lcg_res = mc::plainmc(
                    smooth_test,
                    a2,
                    b2,
                    N,
                    rng1
                );

                const mc::result std_res = mc::plainmc(
                    smooth_test,
                    a2,
                    b2,
                    N,
                    rng2
                );

                lcg_actual_sum += std::abs(lcg_res.value - exact_smooth);
                std_actual_sum += std::abs(std_res.value - exact_smooth);
            }

            const mc::result quasi_res = mc::quasimc(
                smooth_test,
                a2,
                b2,
                N
            );

            file << N << " "
                 << lcg_actual_sum / repeats << " "
                 << std_actual_sum / repeats << " "
                 << std::abs(quasi_res.value - exact_smooth) << " "
                 << quasi_res.error << "\n";
        }

        std::cout << "Scaling comparison made with a non-singular test integral\n";
        std::cout << "Integral int_0^1 int_0^1 (x^2+y^2) dx dy = "
                  << exact_smooth << "\n";
        std::cout << "Data written to B_scaling.dat\n\n";
        std::cout << "A plot of this comparison can be found in B_scaling.svg\n\n";

        const std::vector<double> a3 = {0.0, 0.0, 0.0};
        const std::vector<double> b3 = {1.0, 1.0, 1.0};

        const double exact_singular = 1.3932039296856768591842462603255;
        const std::size_t N = 200000;

        mc::lcg rng1(123u);
        mc::std_rng rng2(123u);

        const mc::result lcg_res = mc::plainmc(
            singular_integrand,
            a3,
            b3,
            N,
            rng1
        );

        const mc::result std_res = mc::plainmc(
            singular_integrand,
            a3,
            b3,
            N,
            rng2
        );

        const mc::result quasi_res = mc::quasimc(
            singular_integrand,
            a3,
            b3,
            N
        );

        std::cout << "Difficult singular integral, N = " << N << "\n";
        std::cout << "Exact value                  = " << exact_singular << "\n";

        std::cout << "LCG estimate                 = " << lcg_res.value
                  << " +/- " << lcg_res.error
                  << ", actual error = "
                  << std::abs(lcg_res.value - exact_singular) << "\n";

        std::cout << "std::mt19937 estimate        = " << std_res.value
                  << " +/- " << std_res.error
                  << ", actual error = "
                  << std::abs(std_res.value - exact_singular) << "\n";

        std::cout << "Halton estimate              = " << quasi_res.value
                  << " +/- " << quasi_res.error
                  << ", actual error = "
                  << std::abs(quasi_res.value - exact_singular) << "\n";
        
        std::cout << "For the smooth test integral, the Halton sequence shows the expected improved scaling compared to pseudo-random Monte Carlo.\n"
                  << "For the singular integral, however, the advantage is not guaranteed, since the integrand is not sufficiently regular near the origin\n"
                  << "In addition, the usual Monte Carlo error estimate is not directly meaningful for deterministic quasi-random points.\n\n";
    }

    if(do_taskC) {
        std::cout << "\n------ TASK C: stratified sampling ------\n\n";

        const std::vector<double> a = {0.0, 0.0};
        const std::vector<double> b = {1.0, 1.0};

        const double exact = 2.0 / 3.0;
        const std::size_t N = 100000;

        mc::lcg rng_plain(777u);
        mc::lcg rng_strat(777u);

        const mc::result plain = mc::plainmc(
            smooth_test,
            a,
            b,
            N,
            rng_plain
        );

        const mc::result strat = mc::stratified(
            smooth_test,
            a,
            b,
            N,
            rng_strat
        );

        std::cout << "Smooth test integral, N = " << N << "\n";
        std::cout << "Integral int_0^1 int_0^1 (x^2+y^2) dx dy\n";
        std::cout << "Exact value                  = " << exact << "\n";

        std::cout << "Plain MC estimate            = " << plain.value
                  << " +/- " << plain.error
                  << ", actual error = "
                  << std::abs(plain.value - exact) << "\n";

        std::cout << "Stratified estimate          = " << strat.value
                  << " +/- " << strat.error
                  << ", actual error = "
                  << std::abs(strat.value - exact) << "\n\n";
        std::cout << "Stratified sampling produces the correct result and reduces the estimated error.";
    }

    return 0;
}