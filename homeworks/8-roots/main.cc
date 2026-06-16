#include <cmath>
#include <cstddef>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>

#include "roots.hpp"

const double exact_hydrogen_E0 = -0.5;

roots::vector simple_1d(const roots::vector& x) {
    return roots::make_vector({std::cos(x[0]) - x[0]});
}

roots::vector simple_2d(const roots::vector& x) {
    return roots::make_vector({
        x[0] * x[0] + x[1] * x[1] - 1.0,
        x[0] - x[1]
    });
}

roots::vector rosenbrock_gradient(const roots::vector& x) {
    const double X = x[0];
    const double Y = x[1];

    return roots::make_vector({
        2.0 * (X - 1.0) - 400.0 * X * (Y - X * X),
        200.0 * (Y - X * X)
    });
}

double rosenbrock(const roots::vector& x) {
    const double X = x[0];
    const double Y = x[1];

    return (1.0 - X) * (1.0 - X) + 100.0 * (Y - X * X) * (Y - X * X);
}

roots::vector himmelblau_gradient(const roots::vector& x) {
    const double X = x[0];
    const double Y = x[1];

    const double A = X * X + Y - 11.0;
    const double B = X + Y * Y - 7.0;

    return roots::make_vector({
        4.0 * X * A + 2.0 * B,
        2.0 * A + 4.0 * Y * B
    });
}

double himmelblau(const roots::vector& x) {
    const double X = x[0];
    const double Y = x[1];

    return (X * X + Y - 11.0) * (X * X + Y - 11.0)
         + (X + Y * Y - 7.0) * (X + Y * Y - 7.0);
}

// "hydrogen_energy" written using ChatGPT
roots::newton_result hydrogen_energy(double rmin, double rmax,
                                     double ode_acc, double ode_eps) {
    const auto M = [rmin, rmax, ode_acc, ode_eps](const roots::vector& x) {
        return roots::make_vector({
            roots::hydrogen_M(x[0], rmin, rmax, ode_acc, ode_eps)
        });
    };

    return roots::newton(
        M,
        roots::make_vector({-0.7}),
        1e-9,
        1e-5,
        100,
        roots::make_vector({1e-6})
    );
}

// "main" written using ChatGPT
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
        std::cout << "\n------ TASK A: Newton root finding ------\n\n";

        const roots::newton_result root1 = roots::newton(
            simple_1d,
            roots::make_vector({1.0}),
            1e-9
        );

        std::cout << "Simple one-dimensional test: cos(x)-x=0\n";
        std::cout << "Root                         = " << root1.x[0] << "\n";
        std::cout << "Norm of f(root)              = " << root1.fx_norm << "\n";
        std::cout << "Iterations                   = " << root1.iterations << "\n\n";

        const roots::newton_result root2 = roots::newton(
            simple_2d,
            roots::make_vector({0.5, 0.8}),
            1e-10
        );

        std::cout << "Simple two-dimensional test\n";
        std::cout << "Equations x^2+y^2=1 and x=y\n";
        std::cout << "Root                         = ("
                  << root2.x[0] << ", " << root2.x[1] << ")\n";
        std::cout << "Norm of f(root)              = " << root2.fx_norm << "\n";
        std::cout << "Iterations                   = " << root2.iterations << "\n\n";

        const roots::newton_result rosen = roots::newton(
            rosenbrock_gradient,
            roots::make_vector({1.2, 1.2}),
            1e-10
        );

        std::cout << "Rosenbrock valley function\n";
        std::cout << "Minimum found at             = ("
                  << rosen.x[0] << ", " << rosen.x[1] << ")\n";
        std::cout << "f(x,y) at this point         = " << rosenbrock(rosen.x) << "\n";
        std::cout << "Norm of gradient             = " << rosen.fx_norm << "\n";
        std::cout << "Iterations                   = " << rosen.iterations << "\n\n";

        const std::vector<roots::vector> starts = {
            roots::make_vector({ 3.0,  2.0}),
            roots::make_vector({-3.0,  3.0}),
            roots::make_vector({-4.0, -3.0}),
            roots::make_vector({ 4.0, -2.0})
        };

        std::cout << "Himmelblau function\n";

        for(std::size_t i = 0; i < starts.size(); ++i) {
            const roots::newton_result him = roots::newton(
                himmelblau_gradient,
                starts[i],
                1e-6
            );

            std::cout << "Minimum " << i + 1 << " found at          = ("
                      << him.x[0] << ", " << him.x[1] << ")\n";
            std::cout << "f(x,y) at this point         = " << himmelblau(him.x) << "\n";
            std::cout << "Norm of gradient             = " << him.fx_norm << "\n";
        }
    }

    if(do_taskB) {
        std::cout << "\n\n------ TASK B: Hydrogen shooting method ------\n\n";

        std::cout << "Near r=0, inserting f=a*r+b*r^2+... gives the constant term -a-b=0,\n";
        std::cout << "so b=-a. Choosing a=1 gives f(r->0)=r-r^2.\n\n";

        const double rmin = 1e-3;
        const double rmax = 8.0;
        const double ode_acc = 1e-7;
        const double ode_eps = 1e-7;

        const roots::newton_result E0 = hydrogen_energy(rmin, rmax, ode_acc, ode_eps);

        std::cout << "Using rmin = " << rmin << ", rmax = " << rmax << "\n";
        std::cout << "Lowest numerical energy      = " << E0.x[0] << "\n";
        std::cout << "Exact ground-state energy    = " << exact_hydrogen_E0 << "\n";
        std::cout << "Actual error                 = "
                  << std::abs(E0.x[0] - exact_hydrogen_E0) << "\n";
        std::cout << "M(E0)=F_E0(rmax)             = " << E0.fx_norm << "\n";
        std::cout << "Iterations                   = " << E0.iterations << "\n\n";

        const roots::hydrogen_result sol = roots::hydrogen_wavefunction(
            E0.x[0],
            rmin,
            rmax,
            ode_acc,
            ode_eps
        );

        std::ofstream wave("B_hydrogen_wave.dat");
        wave << "# r numerical exact\n";

        for(std::size_t i = 0; i < sol.r.size(); ++i) {
            if(i % 50 == 0 || i + 1 == sol.r.size()) {
                const double r = sol.r[i];
                wave << r << " "
                     << sol.f[i] << " "
                     << roots::exact_hydrogen_f(r) << "\n";
            }
        }

        std::cout << "Wave-function data written to B_hydrogen_wave.dat\n";
        std::cout << "Plot can be found in B_hydrogen_wave.svg\n\n";

        std::ofstream rmax_file("B_convergence_rmax.dat");
        rmax_file << "# rmax E0 actual_error\n";

        const std::vector<double> rmax_values = {4.0, 5.0, 6.0, 8.0, 10.0, 12.0};

        for(const double Rmax : rmax_values) {
            const roots::newton_result E = hydrogen_energy(rmin, Rmax, ode_acc, ode_eps);
            rmax_file << Rmax << " "
                      << E.x[0] << " "
                      << std::abs(E.x[0] - exact_hydrogen_E0) << "\n";
        }

        std::ofstream rmin_file("B_convergence_rmin.dat");
        rmin_file << "# rmin E0 actual_error\n";

        const std::vector<double> rmin_values = {1e-1, 5e-2, 1e-2, 5e-3, 1e-3, 5e-4};

        for(const double Rmin : rmin_values) {
            const roots::newton_result E = hydrogen_energy(Rmin, rmax, ode_acc, ode_eps);
            rmin_file << Rmin << " "
                      << E.x[0] << " "
                      << std::abs(E.x[0] - exact_hydrogen_E0) << "\n";
        }

        std::ofstream acc_file("B_convergence_acc.dat");
        acc_file << "# acc E0 actual_error\n";

        const std::vector<double> acc_values = {1e-3, 3e-4, 1e-4, 3e-5, 1e-5, 3e-6, 1e-6};

        for(const double acc : acc_values) {
            const roots::newton_result E = hydrogen_energy(rmin, rmax, acc, ode_eps);
            acc_file << acc << " "
                     << E.x[0] << " "
                     << std::abs(E.x[0] - exact_hydrogen_E0) << "\n";
        }

        std::ofstream eps_file("B_convergence_eps.dat");
        eps_file << "# eps E0 actual_error\n";

        const std::vector<double> eps_values = {1e-3, 3e-4, 1e-4, 3e-5, 1e-5, 3e-6, 1e-6};

        for(const double eps : eps_values) {
            const roots::newton_result E = hydrogen_energy(rmin, rmax, ode_acc, eps);
            eps_file << eps << " "
                     << E.x[0] << " "
                     << std::abs(E.x[0] - exact_hydrogen_E0) << "\n";
        }

        std::cout << "Convergence data written to B_convergence_*.dat\n";
        std::cout << "Convergence plots can be found in B_convergence_*.svg\n";
    }

    if(do_taskC) {
        std::cout << "\n\n------ TASK C: quadratic interpolation line-search ------\n\n";

        const roots::newton_result rosen = roots::newton(
            rosenbrock_gradient,
            roots::make_vector({1.2, 1.2}),
            1e-10,
            1e-3,
            100,
            roots::vector(),
            true
        );

        std::cout << "Rosenbrock minimum with quadratic line-search\n";
        std::cout << "Minimum found at             = ("
                  << rosen.x[0] << ", " << rosen.x[1] << ")\n";
        std::cout << "f(x,y) at this point         = " << rosenbrock(rosen.x) << "\n";
        std::cout << "Norm of gradient             = " << rosen.fx_norm << "\n";
        std::cout << "Iterations                   = " << rosen.iterations << "\n\n";

        const roots::newton_result him = roots::newton(
            himmelblau_gradient,
            roots::make_vector({-4.0, -3.0}),
            1e-6,
            1e-3,
            100,
            roots::vector(),
            true
        );

        std::cout << "Himmelblau test with quadratic line-search\n";
        std::cout << "Minimum found at             = ("
                  << him.x[0] << ", " << him.x[1] << ")\n";
        std::cout << "f(x,y) at this point         = " << himmelblau(him.x) << "\n";
        std::cout << "Norm of gradient             = " << him.fx_norm << "\n";
        std::cout << "Iterations                   = " << him.iterations << "\n\n";

        std::cout << "The Jacobian matrix is allocated once in roots::newton and then updated at each iteration.\n";
    }

    return 0;
}