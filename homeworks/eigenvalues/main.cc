#include <iostream>
#include <iomanip>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <vector>
#include <algorithm>

#include "matrix.h"
#include "evd.h"
#include "hydrogen_swave.h"

namespace pp {

matrix diag(const vector& w) {
    matrix D((int)w.size(), (int)w.size());
    for (size_t i = 0; i < w.size(); ++i) D(i,i) = w[i];
    return D;
}

matrix random_symmetric_matrix(size_t n) {
    matrix A((int)n, (int)n);
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = i; j < n; ++j) {
            double x = 2.0*rand()/double(RAND_MAX) - 1.0;
            A(i,j) = x;
            A(j,i) = x;
        }
    }
    return A;
}

matrix identity(size_t n) {
    matrix I((int)n, (int)n);
    I.setid();
    return I;
}

inline bool approx(double x, double y, double acc=1e-6, double eps=1e-6){
    if(std::abs(x-y) < acc) return true;
    if(std::abs(x-y) < eps*std::max(std::abs(x),std::abs(y))) return true;
    return false;
}

inline bool approx(const matrix& A, const matrix& B, double acc=1e-6, double eps=1e-6){
    if(A.size1() != B.size1() || A.size2() != B.size2()) return false;
    for(size_t i=0; i<A.size1(); i++)
        for(size_t j=0; j<A.size2(); j++)
            if(!approx(A(i,j), B(i,j), acc, eps)) return false;
    return true;
}

double max_abs_diff(const vector& a, const vector& b){
    double m = 0;
    for(size_t i = 0; i < a.size(); i++){
        double d = std::abs(a[i] - b[i]);
        if(d > m) m = d;
    }
    return m;
}

void sort_vector(vector& v){
    std::sort(v.data.begin(), v.data.end());
}

} // namespace pp

int main(int argc, char** argv) {
    using namespace pp;

    std::srand(1); // fixed seed for reproducible timing/correctness

    bool do_taskA = false;
    bool do_taskB = false;
    bool do_taskC = false;

    bool run_slow = false;
    bool run_fast = false;

    double rmax = 10.0;
    double dr   = 0.1;
    int N = 50;

    for(int i = 1; i < argc; ++i) {
        if(std::strcmp(argv[i], "-taskA") == 0) {
            do_taskA = true;
        } else if(std::strcmp(argv[i], "-taskB") == 0) {
            do_taskB = true;
        } else if(std::strcmp(argv[i], "-taskC") == 0) {
            do_taskC = true;
        } else if(std::strcmp(argv[i], "-slow") == 0) {
            run_slow = true;
        } else if(std::strcmp(argv[i], "-fast") == 0) {
            run_fast = true;
        } else if(std::strcmp(argv[i], "-rmax") == 0 && i+1 < argc) {
            rmax = std::atof(argv[++i]);
        } else if(std::strcmp(argv[i], "-dr") == 0 && i+1 < argc) {
            dr = std::atof(argv[++i]);
        } else if(std::strcmp(argv[i], "-n") == 0 && i+1 < argc) {
            N = std::atoi(argv[++i]);
        }
    }

    if(!do_taskA && !do_taskB && !do_taskC) {
        do_taskA = do_taskB = do_taskC = true;
    }

    if(do_taskA) {
        std::cout << "\n------ TASK A ------\n";

        size_t n = 5;
        matrix A_taskA = random_symmetric_matrix(n);
        std::cout << "Generating symmetric matrix A for n=5:\n";
        A_taskA.print("A");

        EVD_slow evd(A_taskA);
        matrix V = evd.V;
        vector w = evd.w;
        matrix D = diag(w);

        matrix VT = V.T();

        matrix VTAV = VT * A_taskA * V;
        matrix VDVT = V * D * VT;
        matrix VTV  = VT * V;
        matrix VVT  = V * VT;
        matrix I    = identity(n);

        std::cout << "\nEigenvalues:\n";
        w.print("w = ");

        std::cout << "\nDiagonal matrix:\n";
        D.print("D = ");

        std::cout << "\nEigenvector matrix:\n";
        V.print("V = ");

        std::cout << "\nChecking if the implementation works as intended:\n";
        std::cout << std::boolalpha;
        std::cout << "V^T A V = D:          " << approx(VTAV,D) << "\n";
        std::cout << "V D V^T = A:          " << approx(VDVT,A_taskA) << "\n";
        std::cout << "V^T V = I:            " << approx(VTV, I) << "\n";
        std::cout << "V V^T = I:            " << approx(VVT, I) << "\n";
    }

    if(do_taskB) {
        std::cout << "\n------ TASK B ------\n";

        std::cout << "Proving f(r -> 0) = r-r^2 -> 0\n";
        std::cout << "Assume f(r) can be approximated by a power expansion such that:\n";
        std::cout << "f(r) = a_0 + a_1 r + a_2 r^2 + ...\n";
        std::cout << "f''(r) = 2 a_2 + 6 a_3 r + 12 a_4 r^2 + ...\n";
        std::cout << "The s-wave radial Schrodinger equation is:\n";
        std::cout << "-(1/2) f'' -(1/r) f = e f\n";
        std::cout << "or equivalently:\n";
        std::cout << "f'' + (2/r)f + 2ef = 0\n";
        std::cout << "Inserting the power series gives a singular term proportional to a_0/r,\n";
        std::cout << "so regularity at r=0 requires a_0 = 0.\n";
        std::cout << "The constant term then gives 2a_2 + 2a_1 = 0, hence a_2 = -a_1.\n";
        std::cout << "Therefore f(r) = a_1(r-r^2+...) and so f(r->0)->0.\n";

        std::cout << "\nPart 2: Numerical hydrogen Hamiltonian\n";
        std::cout << "Using rmax = " << rmax << " and dr = " << dr << "\n";
        std::cout << "See wavefunctions.svg.\n";

        vector r;
        matrix H = hydrogen_hamiltonian(rmax, dr, r);

        EVD_slow hevd(H);
        vector hw = hevd.w;
        matrix HV = hevd.V;

        print_hydrogen_energies(hw, 3);
        save_hydrogen_wavefunctions("hydrogen_wavefunctions.dat", r, HV, hw, dr, 3);

        std::cout << "\nSaved wavefunctions to hydrogen_wavefunctions.dat\n";

        std::vector<double> dr_values   = {0.5, 0.3, 0.2, 0.15, 0.1, 0.08, 0.05};
        std::vector<double> rmax_values = {4, 6, 8, 10, 12, 15, 20};

        save_convergence_vs_dr("hydrogen_conv_dr.dat", 10.0, dr_values);
        save_convergence_vs_rmax("hydrogen_conv_rmax.dat", 0.05, rmax_values);

        std::cout << "Saved convergence data to hydrogen_conv_dr.dat and hydrogen_conv_rmax.dat\n";
        std::cout << "See conv_dr.svg and conv_rmax.svg\n";
    }

    if(do_taskC) {
        matrix A_taskC = random_symmetric_matrix(N);

        if(run_slow) {
            EVD_slow slow(A_taskC);
            return 0;
        }

        if(run_fast) {
            EVD_fast fast(A_taskC);
            return 0;
        }

        // default: correctness check
        std::cout << "\n------ TASK C ------\n";

        EVD_slow slow(A_taskC);
        EVD_fast fast(A_taskC);

        vector wslow = slow.w;
        vector wfast = fast.w;
        sort_vector(wslow);
        sort_vector(wfast);

        std::cout << std::setprecision(10);
        std::cout << "Compare EVD_slow and EVD_fast\n";
        std::cout << "N = " << N << "\n";
        std::cout << "max |w_slow - w_fast| = " << max_abs_diff(wslow, wfast) << "\n";
        std::cout << "See taskC.svg";
    }

    return 0;
}