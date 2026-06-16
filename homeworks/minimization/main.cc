#include <cmath>
#include <cstddef>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "minimize.hpp"

// All functions before main are written using ChatGPT
struct higgs_data {
    std::vector<double> energy;
    std::vector<double> signal;
    std::vector<double> error;
};

double rosenbrock(const minimize::vector& x) {
    const double X = x[0];
    const double Y = x[1];

    return (1.0 - X) * (1.0 - X) + 100.0 * (Y - X * X) * (Y - X * X);
}

double himmelblau(const minimize::vector& x) {
    const double X = x[0];
    const double Y = x[1];

    return (X * X + Y - 11.0) * (X * X + Y - 11.0)
         + (X + Y * Y - 7.0) * (X + Y * Y - 7.0);
}

double breit_wigner(double E, double m, double Gamma, double A) {
    return A / ((E - m) * (E - m) + Gamma * Gamma / 4.0);
}

higgs_data read_higgs_data() {
    higgs_data data;

    const std::string table = R"(
101 -0.25 2.0
103 -0.30 2.0
105 -0.15 1.9
107 -1.71 1.9
109  0.81 1.9
111  0.65 1.9
113 -0.91 1.9
115  0.91 1.9
117  0.96 1.6
119 -2.52 1.6
121 -1.01 1.6
123  2.01 1.6
125  4.83 1.6
127  4.58 1.6
129  1.26 1.3
131  1.01 1.3
133 -1.26 1.3
135  0.45 1.3
137  0.15 1.3
139 -0.91 1.3
141 -0.81 1.1
143 -1.41 1.1
145  1.36 1.1
147  0.50 1.1
149 -0.45 1.1
151  1.61 1.1
153 -2.21 1.1
155 -1.86 0.9
157  1.76 0.9
159 -0.50 0.9
)";

    std::istringstream input(table);

    double E = 0.0;
    double s = 0.0;
    double ds = 0.0;

    while(input >> E >> s >> ds) {
        data.energy.push_back(E);
        data.signal.push_back(s);
        data.error.push_back(ds);
    }

    return data;
}

void print_vector(const minimize::vector& x) {
    std::cout << "{ ";

    for(std::size_t i = 0; i < x.size(); ++i) {
        std::cout << x[i];

        if(i + 1 < x.size()) {
            std::cout << ", ";
        }
    }

    std::cout << " }";
}

void print_result(const minimize::result& r) {
    std::cout << "method = " << minimize::method_name(r.derivative_method)
              << ", iterations = " << r.iterations
              << ", evals = " <<  r.evaluations
              << ", results:\n   f(x) = " << std::setw(16) << r.fx
              << "\n   |grad| = " << std::setw(14) << r.grad_norm
              << "\n   x = ";

    print_vector(r.x);

    if(!r.converged) {
        std::cout << "  not converged";
    }

    std::cout << "\n";
}

void write_higgs_data(const higgs_data& data) {
    std::ofstream file("higgs_data.dat");
    file << "# E signal error\n";

    for(std::size_t i = 0; i < data.energy.size(); ++i) {
        file << data.energy[i] << " "
             << data.signal[i] << " "
             << data.error[i] << "\n";
    }
}

void write_higgs_fit(const minimize::result& fit) {
    std::ofstream file("higgs_fit.dat");
    file << "# E fit\n";

    for(double E = 100.0; E <= 160.0; E += 0.1) {
        file << E << " "
             << breit_wigner(E, fit.x[0], fit.x[1], fit.x[2]) << "\n";
    }
}

// "main" written myself and improved using ChatGPT
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

    std::cout << std::setprecision(12);

    const higgs_data data = read_higgs_data();

    const auto deviation = [&data](const minimize::vector& p) {
        const double m = p[0];
        const double Gamma = p[1];
        const double A = p[2];

        double sum = 0.0;

        for(std::size_t i = 0; i < data.energy.size(); ++i) {
            const double fit = breit_wigner(data.energy[i], m, Gamma, A);
            const double residual = (fit - data.signal[i]) / data.error[i];

            sum += residual * residual;
        }

        return sum;
    };

    if(do_taskA) {
        std::cout << "\n------ TASK A ------\n\n";

        const minimize::result rosen = minimize::newton(
            rosenbrock,
            minimize::make_vector({-1.2, 1.0}),
            1e-6,
            minimize::method::forward
        );

        std::cout << "Rosenbrock valley function\n";
        print_result(rosen);
        std::cout << "Expected minimum = { 1, 1 }\n\n";

        const minimize::result himmel = minimize::newton(
            himmelblau,
            minimize::make_vector({2.5, 2.0}),
            1e-6,
            minimize::method::forward
        );

        std::cout << "Himmelblau function\n";
        print_result(himmel);
        std::cout << "One exact minimum = { 3, 2 }\n";
    }

    if(do_taskB) {
        std::cout << "\n\n------ TASK B ------\n\n";

        const minimize::result higgs = minimize::newton(
            deviation,
            minimize::make_vector({125.0, 2.0, 12.0}),
            1e-6,
            minimize::method::forward
        );

        print_result(higgs);

        std::cout << "\nBest-fit mass m              = " << higgs.x[0] << " GeV\n";
        std::cout << "Best-fit width Gamma         = " << std::abs(higgs.x[1]) << " GeV\n";
        std::cout << "Best-fit scale A             = " << higgs.x[2] << "\n";
        std::cout << "Minimum deviation D          = " << higgs.fx << "\n\n";

        write_higgs_data(data);
        write_higgs_fit(higgs);

        std::cout << "Data written to higgs_data.dat and higgs_fit.dat\n";
        std::cout << "Plot can be found in higgs_fit.svg\n";
    }

    if(do_taskC) {
        std::cout << "\n\n------ TASK C ------\n\n";

        const minimize::result rosen_forward = minimize::newton(
            rosenbrock,
            minimize::make_vector({-1.2, 1.0}),
            1e-6,
            minimize::method::forward
        );

        const minimize::result rosen_central = minimize::newton(
            rosenbrock,
            minimize::make_vector({-1.2, 1.0}),
            1e-6,
            minimize::method::central
        );

        std::cout << "Rosenbrock comparison\n";
        print_result(rosen_forward);
        print_result(rosen_central);
        std::cout << "\n";

        const minimize::result himmel_forward = minimize::newton(
            himmelblau,
            minimize::make_vector({2.5, 2.0}),
            1e-6,
            minimize::method::forward
        );

        const minimize::result himmel_central = minimize::newton(
            himmelblau,
            minimize::make_vector({2.5, 2.0}),
            1e-6,
            minimize::method::central
        );

        std::cout << "Himmelblau comparison\n";
        print_result(himmel_forward);
        print_result(himmel_central);
        std::cout << "\n";

        const minimize::result higgs_forward = minimize::newton(
            deviation,
            minimize::make_vector({125.0, 2.0, 12.0}),
            1e-6,
            minimize::method::forward
        );

        const minimize::result higgs_central = minimize::newton(
            deviation,
            minimize::make_vector({125.0, 2.0, 12.0}),
            1e-6,
            minimize::method::central
        );

        std::cout << "Higgs comparison\n";
        print_result(higgs_forward);
        print_result(higgs_central);
        std::cout << "\n";

        std::cout << "Conclusion:\n";
        std::cout << "Central finite differences are better in this test because they give the same minima\n";
        std::cout << "with more accurate derivatives and, in this implementation, fewer total function evaluations.\n";
    }

    return 0;
}