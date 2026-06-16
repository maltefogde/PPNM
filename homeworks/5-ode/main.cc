#include <iostream>
#include <vector>
#include <fstream>
#include <iomanip>
#include <cmath>
#include <cstring>
#include <algorithm>
#include <stdexcept>
#include <numbers>
#include <string>
#include <limits>

#include "ode.h"


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

    if(do_taskA) {
        std::cout << "\n------ TASK A ------\n";

        // main.cc for Task A written by ChatGPT
        // Testing the adaptive ODE driver on u'' = -u.

        // Rewrite as first-order system: y = {u,u'}
        // y0 = u       y0' = y1
        // y1 = u'      y1' = -y0

        // With u(0)=0 and u'(0)=1, the exact solution is u=sin(x).
        //  dy/dx = {u', u''} = {u', -u} = {y[1], -y[0]}

        auto harmonic = [](double x, const vec& y) {
            (void)x;
            return vec{y[1], -y[0]}; // dy/dx = y_0', y_1' = y_1,-y_0 = u',-u
        };

        auto [xs, ys] = driver(
            harmonic,
            0.0, 10.0,          /* initial-point,final-point */
            vec{0.0, 1.0},      /* y(initial-point) */
            0.1,                /* initial step-size */
            1e-6,               /* absolute accuracy goal */
            1e-6                /* relative accuracy goal */
        );

        std::ofstream out1("Task_A_harmonic.dat");
        out1 << "# x u_numeric u_exact error_u uprime_numeric uprime_exact error_uprime\n";

        double max_error_u = 0.0;
        double max_error_up = 0.0;

        for(std::size_t i = 0; i < xs.size(); ++i) {
            double x = xs[i];

            double u_num = ys[i][0]; // u
            double up_num = ys[i][1]; // u'

            double u_exact = std::sin(x);
            double up_exact = std::cos(x);

            double err_u = u_num - u_exact;
            double err_up = up_num - up_exact;

            max_error_u = std::max(max_error_u, std::abs(err_u));
            max_error_up = std::max(max_error_up, std::abs(err_up));

            out1 << std::setprecision(16)
                 << x << " "
                 << u_num << " "
                 << u_exact << " "
                 << err_u << " "
                 << up_num << " "
                 << up_exact << " "
                 << err_up << "\n";
        }

        std::cout << "Exercise A.3: Harmonic oscillator test: u'' = -u\n";
        std::cout << "Accepted points: " << xs.size() << "\n";
        std::cout << "Max error in u:  " << max_error_u << "\n";
        std::cout << "Max error in u': " << max_error_up << "\n";
        std::cout << "Wrote Task_A_harmonic.dat\n";
        std::cout << "See Task_A_harmonic.svg\n\n";


        // Damped pendulum example, from https://docs.scipy.org/doc/scipy/reference/generated/scipy.integrate.odeint.html
        // theta'' + b theta' + c sin(theta) = 0
 
        // y = {theta, omega} = {theta, theta'}
        // dy/dx = {theta', theta''}

        // theta'' = -b theta' - c sin(theta)
        // dy/dx = {omega, -b omega - c sin(theta)}
        //       = {y[1], -b*y[1] - c*std::sin(y[0])}

        const double pi = std::acos(-1.0);
        const double b_friction = 0.25;
        const double c_pendulum = 5.0;

        auto friction = [b_friction, c_pendulum](double t, const vec& y) {
            (void)t;
            double theta = y[0];
            double omega = y[1];
            return vec{omega, -b_friction * omega - c_pendulum * std::sin(theta)};
        };

        auto [ts_fric, ys_fric] = driver(
            friction,
            0.0, 10.0,              /* initial-point,final-point */
            vec{pi - 0.1, 0.0},     /* y(initial-point) */
            0.05,                   /* initial step-size */
            1e-6,                   /* absolute accuracy goal */
            1e-6                    /* relative accuracy goal */
        );

        std::ofstream out2("Task_A_friction.dat");
        out2 << "# t theta omega\n";

        for(std::size_t i = 0; i < ts_fric.size(); ++i) {
            out2 << std::setprecision(16)
                 << ts_fric[i] << " "
                 << ys_fric[i][0] << " "
                 << ys_fric[i][1] << "\n";
        }

        std::cout << "Exercise A.4.1: Damped pendulum\n";
        std::cout << "Accepted points: " << ts_fric.size() << "\n";
        std::cout << "Wrote Task_A_friction.dat\n";
        std::cout << "See Task_A_friction.svg\n\n";


        // Lotka-Volterra example, scipy.integrate.solve_ivp manual and 
        // https://en.wikipedia.org/wiki/Lotka–Volterra_equations
        //
        // x' = alpha*x - beta*x*y
        // y' = delta*x*y - gamma*y

        // y_vec = {x, y} = {y0, y1}
        // x = y[0] = prey population
        // y = y[1] = predator population
        // dy_vec/dt = {x', y'}


        // dy_vec/dt = {
        //     alpha*y[0] - beta*y[0]*y[1],
        //     delta*y[0]*y[1] - gamma*y[1]
        // }


        // Constants chosen like scipy.integrate.solve_ivp manual
        const double alpha = 1.5;
        const double beta = 1.0;
        const double gamma = 3.0;
        const double delta = 1.0;


        auto lotka = [alpha, beta, gamma, delta](double t, const vec& y) {
            (void)t;

            double x = y[0];
            double y_pop = y[1];

            return vec{
                alpha * x - beta * x * y_pop,
                delta * x * y_pop - gamma * y_pop
            };
        };

        auto [ts_lotka, ys_lotka] = driver(
            lotka,
            0.0, 15.0,          /* initial-point,final-point */
            vec{10.0, 5.0},     /* y(initial-point) */
            0.01,               /* initial step-size */
            1e-6,               /* absolute accuracy goal */
            1e-6                /* relative accuracy goal */
        );

        std::ofstream out3("Task_A_lotka.dat");
        out3 << "# t x y\n";

        for(std::size_t i = 0; i < ts_lotka.size(); ++i) {
            out3 << std::setprecision(16)
                 << ts_lotka[i] << " "
                 << ys_lotka[i][0] << " "
                 << ys_lotka[i][1] << "\n";
        }

        std::cout << "Exercise A.4.2: Lotka-Volterra test\n";
        std::cout << "Accepted points: " << ts_lotka.size() << "\n";
        std::cout << "Wrote Task_A_lotka.dat\n";
        std::cout << "Task_A_lotka.svg\n\n";
    }

    if(do_taskB) {
        std::cout << "\n\n------ TASK B ------\n";

        /*
            Relativistic precession of planetary orbit

            Equation:
                u''(phi) + u(phi) = 1 + eps*u(phi)^2

            Rewrite as first-order system:
                y = {u, u'} = {y[0], y[1]}

                y[0]' = y[1]
                y[1]' = 1 - y[0] + eps*y[0]^2
        */

        struct Case {
            std::string name;
            std::string filename;
            double eps;
            vec y0;
        };

        std::vector<Case> cases = {
            {
                "Newtonian circular orbit",
                "Task_B_circular.dat",
                0.0,
                vec{1.0, 0.0}
            },
            {
                "Newtonian elliptical orbit",
                "Task_B_ellipse.dat",
                0.0,
                vec{1.0, -0.5}
            },
            {
                "Relativistic precessing orbit",
                "Task_B_precession.dat",
                0.01,
                vec{1.0, -0.5}
            }
        };

        const double pi = std::acos(-1.0);
        const double phi_start = 0.0;
        const double phi_end   = 20.0 * 2.0 * pi; // 20 rotations

        const std::size_t Nplot = 4000; // same number of plotted intervals for all cases

        for(const auto& c : cases) {
            const double eps = c.eps;

            auto eq_motion = [eps](double phi, const vec& y) {
                (void)phi;

                double u  = y[0];
                double up = y[1];

                return vec{up, 1.0 - u + eps*u*u};
            };

            auto [phis, ys] = driver(
                eq_motion,
                phi_start,
                phi_end,
                c.y0,
                0.05,
                1e-6,
                1e-6
            );

            std::ofstream out(c.filename);

            out << "# " << c.name << "\n";
            out << "# eps = " << eps << "\n";
            out << "# phi u u_prime x y\n";

            std::size_t j = 0;

            for(std::size_t k = 0; k <= Nplot; ++k) {
                double phi = phi_start + (phi_end - phi_start) * k / Nplot;

                while(j + 1 < phis.size() && phis[j + 1] < phi) {
                    ++j;
                }

                double u;
                double up;

                if(j + 1 < phis.size()) {
                    double phi0 = phis[j];
                    double phi1 = phis[j + 1];

                    double t = (phi - phi0) / (phi1 - phi0);

                    u  = ys[j][0] + t * (ys[j + 1][0] - ys[j][0]);
                    up = ys[j][1] + t * (ys[j + 1][1] - ys[j][1]);
                } else {
                    u  = ys.back()[0];
                    up = ys.back()[1];
                }

                double r;
                double x;
                double y;

                if(std::abs(u) > 1e-14) {
                    r = 1.0 / u;
                    x = r * std::cos(phi);
                    y = r * std::sin(phi);
                } else {
                    r = std::numeric_limits<double>::quiet_NaN();
                    x = std::numeric_limits<double>::quiet_NaN();
                    y = std::numeric_limits<double>::quiet_NaN();
                }

                out << std::setprecision(16)
                    << phi << " "
                    << u   << " "
                    << up  << " "
                    << x   << " "
                    << y   << "\n";
            }

            std::cout << c.name << "\n";
            std::cout << "  eps = " << eps << "\n";
            std::cout << "  initial condition: u(0) = "
                    << c.y0[0] << ", u'(0) = " << c.y0[1] << "\n";
            std::cout << "  accepted points: " << phis.size() << "\n";
            std::cout << "  plot points: " << Nplot + 1 << "\n";
            std::cout << "  wrote: " << c.filename << "\n\n";
        }

        std::cout << "Plot orbit using columns 4 and 5, e.g.\n";
        std::cout << "plot 'Task_B_precession.dat' using 4:5 with lines notitle\n";
    }

    if(do_taskC) {
        std::cout << "\n\n------ TASK C ------\n";
        std::cout << "Newtonian three-body figure-8 orbit\n\n";

        /*
            Newtonian three-body problem with equal masses and G = 1.

            Equation:
                r_i'' = sum_{j != i} (r_j - r_i)/|r_j - r_i|^3

            State vector:
                z = {vx1, vy1, vx2, vy2, vx3, vy3,
                    x1,  y1,  x2,  y2,  x3,  y3}

            Therefore:
                x_i'  = vx_i
                y_i'  = vy_i
                vx_i' = ax_i
                vy_i' = ay_i
        */

        auto threebody = [](double t, const vec& z) {
            (void)t;

            vec dz(12);

            // Position derivatives
            dz[6]  = z[0];
            dz[7]  = z[1];
            dz[8]  = z[2];
            dz[9]  = z[3];
            dz[10] = z[4];
            dz[11] = z[5];

            double x[3] = {z[6], z[8], z[10]};
            double y[3] = {z[7], z[9], z[11]};

            for(int i = 0; i < 3; ++i) {
                double ax = 0.0;
                double ay = 0.0;

                for(int j = 0; j < 3; ++j) {
                    if(i == j) continue;

                    double dx = x[j] - x[i];
                    double dy = y[j] - y[i];

                    double r2 = dx*dx + dy*dy;
                    double r3 = r2 * std::sqrt(r2);

                    ax += dx / r3;
                    ay += dy / r3;
                }

                dz[2*i]     = ax;
                dz[2*i + 1] = ay;
            }

            return dz;
        };

        // Figure-8 initial conditions
        double x1 = -0.97000436;
        double y1 =  0.24308753;

        double x2 = -x1;
        double y2 = -y1;

        double x3 = 0.0;
        double y3 = 0.0;

        double vx3 = 0.93240737;
        double vy3 = 0.86473146;

        double vx1 = -0.5 * vx3;
        double vy1 = -0.5 * vy3;

        double vx2 = vx1;
        double vy2 = vy1;

        vec z0 = {
            vx1, vy1,
            vx2, vy2,
            vx3, vy3,
            x1,  y1,
            x2,  y2,
            x3,  y3
        };

        double a = 0.0;
        double T = 6.32591398;
        double b = T;

        double h   = 0.01;
        double acc = 1e-6;
        double eps = 1e-6;

        auto [ts, zs] = driver(
            threebody,
            a, b, z0,
            h, acc, eps
        );

        std::ofstream data("Task_C_figure8.dat");
        data << "# t x1 y1 x2 y2 x3 y3\n";

        for(std::size_t i = 0; i < ts.size(); ++i) {
            data << std::setprecision(16)
                << ts[i] << " "
                << zs[i][6]  << " " << zs[i][7]  << " "
                << zs[i][8]  << " " << zs[i][9]  << " "
                << zs[i][10] << " " << zs[i][11] << "\n";
        }

        data.close();

        std::cout << "Output file:\n";
        std::cout << "  Task_C_figure8.dat\n\n";

        std::cout << "Integration setup:\n";
        std::cout << "  Period T      = " << T << "\n";
        std::cout << "  initial h     = " << h << "\n";
        std::cout << "  acc           = " << acc << "\n";
        std::cout << "  eps           = " << eps << "\n";
        std::cout << "  output points = " << ts.size() << "\n";
    }

    return 0;
}