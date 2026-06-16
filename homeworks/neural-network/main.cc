#include <cmath>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>

#include "ann.hpp"

const double pi = std::acos(-1.0);

double g(double x) {
    return std::cos(5.0 * x - 1.0) * std::exp(-x * x);
}

double gp(double x) {
    return std::exp(-x * x) * (-5.0 * std::sin(5.0 * x - 1.0)
                               -2.0 * x * std::cos(5.0 * x - 1.0));
}

double gpp(double x) {
    return std::exp(-x * x) * ((4.0 * x * x - 27.0) * std::cos(5.0 * x - 1.0)
                               + 20.0 * x * std::sin(5.0 * x - 1.0));
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

    if(do_taskA || do_taskB) {
        annlib::vector xs;
        annlib::vector ys;

        const int samples = 25;

        for(int i = 0; i < samples; ++i) {
            const double x = -1.0 + 2.0 * i / (samples - 1.0);

            xs.push_back(x);
            ys.push_back(g(x));
        }

        annlib::network net(8);
        net.train(xs, ys);

        if(do_taskA) {
            std::cout << "\n------ TASK A: interpolation with neural network ------\n\n";

            std::cout << "Function sampled on [-1,1]: cos(5*x-1)*exp(-x*x)\n";
            std::cout << "Hidden neurons               = " << net.n << "\n";
            std::cout << "Final cost                   = " << net.last_result.cost << "\n";
            std::cout << "Gradient norm                = " << net.last_result.grad_norm << "\n";
            std::cout << "Iterations                   = " << net.last_result.iterations << "\n";

            std::ofstream file("A_interpolation.dat");
            file << "# x exact network\n";

            for(int i = 0; i <= 200; ++i) {
                const double x = -1.0 + 2.0 * i / 200.0;

                file << x << " "
                     << g(x) << " "
                     << net.response(x) << "\n";
            }

            std::cout << "Interpolation data written to A_interpolation.dat\n";
            std::cout << "Plot can be found in A_interpolation.svg\n";
        }

        if(do_taskB) {
            std::cout << "\n\n------ TASK B: derivatives and anti-derivative ------\n\n";

            std::cout << "The Gaussian wavelet has analytic first derivative, ";
            std::cout << "second derivative and anti-derivative.\n";

            std::ofstream file("B_derivatives.dat");
            file << "# x g Fp gprime Fpprime gsecond Fpsecond Fp_antiderivative\n";

            for(int i = 0; i <= 200; ++i) {
                const double x = -1.0 + 2.0 * i / 200.0;

                file << x << " "
                     << g(x) << " " << net.response(x) << " "
                     << gp(x) << " " << net.derivative(x) << " "
                     << gpp(x) << " " << net.second_derivative(x) << " "
                     << net.antiderivative(x) - net.antiderivative(-1.0) << "\n";
            }

            std::cout << "Derivative data written to B_derivatives.dat\n";
            std::cout << "Plot can be found in B_derivatives.svg\n";
        }
    }

    if(do_taskC) {
        std::cout << "\n\n------ TASK C: differential equation example ------\n\n";

        std::cout << "Solving y''+y=0 on [0,pi] with y(0)=0 and y'(0)=1.\n";
        std::cout << "Exact solution is sin(x).\n";

        annlib::network ode_net(8);
        ode_net.set_interval(0.0, pi);

        auto phi = [](double ypp, double yp, double y, double x) {
            (void)yp;
            (void)x;

            return ypp + y;
        };

        ode_net.train_de(phi, 0.0, pi, 0.0, 0.0, 1.0);

        std::cout << "Final cost                   = " << ode_net.last_result.cost << "\n";
        std::cout << "Gradient norm                = " << ode_net.last_result.grad_norm << "\n";
        std::cout << "Iterations                   = " << ode_net.last_result.iterations << "\n";

        std::ofstream file("C_ode.dat");
        file << "# x exact network\n";

        for(int i = 0; i <= 200; ++i) {
            const double x = pi * i / 200.0;

            file << x << " "
                 << std::sin(x) << " "
                 << ode_net.response(x) << "\n";
        }

        std::cout << "ODE data written to C_ode.dat\n";
        std::cout << "Plot can be found in C_ode.svg\n";
    }

    return 0;
}