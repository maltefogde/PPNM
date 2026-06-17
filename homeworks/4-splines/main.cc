#include <iostream>
#include <vector>
#include <fstream>
#include <iomanip>
#include <cmath>
#include <cstring>
#include "splines.h"

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
        std::vector<double> x, y;
        for(double xi = 0.0; xi <= 9.0 + 1e-12; xi += 0.5){
            x.push_back(xi);
            y.push_back(std::cos(xi));
        }

        std::ofstream out("Task_A1.dat");
        for(double z = x.front(); z <= x.back(); z += 0.01){
            out << z << " "
                << linterp(x,y,z) << " "
                << linterpInteg(x,y,z) << "\n";
        }

        std::ofstream pts("Task_A1_points.dat");
        for(size_t i = 0; i < x.size(); ++i){
            pts << x[i] << " " << y[i] << "\n";
        }

        std::cout << "Wrote Task_A1.dat\n";
        std::cout << "See plot Task_A1.svg where the spline behaves as expected.\n";
    }

    if(do_taskB) {
        std::cout << "\n\n------ TASK B ------\n";

        std::vector<double> x, y;
        for(double xi = 0.0; xi <= 9.0 + 1e-12; xi += 0.5){
            x.push_back(xi);
            y.push_back(std::cos(xi));
        }

        std::ofstream out("Task_B1.dat");
        QuadraticSpline myspline(x,y);
        for(double z = x.front(); z <= x.back(); z += 0.01){
            out << z << " "
                << myspline.eval(z) << " "
                << myspline.deriv(z) << " "
                << myspline.integ(z) << "\n";
        }

        std::ofstream pts("Task_B1_points.dat");
        for(size_t i = 0; i < x.size(); ++i){
            pts << x[i] << " " << y[i] << "\n";
        }

        std::cout << "Wrote Task_B1.dat\n";
        std::cout << "See Task_B1.svg and Task_B_debug.svg\n";

        vec x2 = {1,2,3,4,5};
        test_spline(x2, {1,1,1,1,1}, "const", 0);
        test_spline(x2, {1,2,3,4,5}, "linear", 1);
        test_spline(x2, {1,4,9,16,25}, "quadratic", 2);
    }

    if(do_taskC) {
        // main.cc for Task C written by ChatGPT
        std::cout << "\n\n------ TASK C ------\n";
            vec x = {0, 1, 2, 3, 4, 5, 6};
        vec y(x.size());

        for(size_t i = 0; i < x.size(); ++i){
            y[i] = std::cos(x[i]);
        }

        CubicSpline s(x, y);

        // Write original interpolation points
        std::ofstream pts("Task_C_cubic_points.dat");
        for(size_t i = 0; i < x.size(); ++i){
            pts << x[i] << " " << y[i] << "\n";
        }

        // Write my cubic spline evaluated on a dense grid
        std::ofstream own("Task_C_cubic_own.dat");

        int N = 1000;
        double xmin = x.front();
        double xmax = x.back();

        for(int k = 0; k <= N; ++k){
            double z = xmin + (xmax - xmin)*k/N;
            own << z << " " << s.eval(z) << "\n";
        }

        std::cout << "\n--- Task C: Cubic spline comparison ---\n";
        std::cout << "Generated files:\n";
        std::cout << "  Task_C_cubic_points.dat\n";
        std::cout << "  Task_C_cubic_own.dat\n";
        std::cout << "Compare my CubicSpline with gnuplot's built-in smooth csplines in Task_C_cubic_comparison.svg\n";
    }
}