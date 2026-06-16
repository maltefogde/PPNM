#include <iostream>
#include <iomanip>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <vector>
#include <algorithm>
#include <cassert>
#include <fstream>

#include "matrix.h"
#include "qr.h"
#include "lsfit.h"

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

matrix random_tall_matrix(size_t n, size_t m) {
    matrix A((int)n, (int)m);
    assert(n > m);
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < m; ++j) {
            double x = 2.0*rand()/double(RAND_MAX) - 1.0;
            A(i,j) = x;
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
        std::cout << std::boolalpha;
        std::cout << "\n------ TASK A ------\n";
        std::cout << "Test that QR works for tall matrices: generating a tall matrix and testing\n";

        size_t n = 8;
        size_t m = 5;
        matrix A_taskA = random_tall_matrix(n, m);
        QRdec QR_taskA(A_taskA);
        std::cout << "Q R = A:            " << approx(QR_taskA.Q*QR_taskA.R, A_taskA) << "\n\n";

        std::cout << "Proof that delta(ln y) = delta y / y:\n"
          << "For g(y)=ln(y), standard error propagation gives "
          << "delta g = |g'(y)| delta y.\n"
          << "Since g'(y)=1/y, we get delta(ln y)=delta y/y.\n\n";

        auto fs = std::vector<pp::fitfunc>{
            [](double){ return 1.0; },
            [](double t){ return t; }
        };

        pp::vector t  = {1,2,3,4,6,9,10,13,15};
        pp::vector y  = {117,100,88,72,53,29.5,25.2,15.2,11.1};
        pp::vector dy = {6,5,4,4,4,3,3,2,2};

        pp::vector logy(y.size());
        pp::vector dlogy(y.size());

        for(size_t i=0; i<y.size(); i++){
            logy[i]  = std::log(y[i]);
            dlogy[i] = dy[i]/y[i];
        }

        pp::vector c = pp::lsfit(fs, t, logy, dlogy);

        // ln(y) = c[0] + c[1] t
        // y = exp(c[0]) * exp(c[1] t) = a * exp(-lambda t)
        double a      = std::exp(c[0]);
        double lambda = -c[1];
        double T12    = std::log(2.0)/lambda;

        std::cout << "Investigate the law of radioactive decay:\n";
        std::cout << "Results of the fit:\n";
        std::cout << "a       = " << a << "\n";
        std::cout << "lambda  = " << lambda << "\n";
        std::cout << "\nT_{1/2} from fit  = " << T12 << " days\n";

        double modern_T12 = 3.631; // http://www.lnhb.fr/nuclides/Ra-224_com.pdf
        double dev_T12 = (T12-modern_T12)*100/modern_T12;
        std::cout <<   "Excpected T_{1/2} = " << modern_T12 << " days\n";
        std::cout <<   "Deviation         = " << dev_T12 << " %\n";

        std::ofstream fitfile("thx_fit.dat");
        std::ofstream logfile("thx_logfit.dat");

        fitfile << std::setprecision(10);
        logfile << std::setprecision(10);

        for(size_t i = 0; i < t.size(); i++){
            double yfit   = a * std::exp(-lambda * t[i]);
            double logfit = c[0] + c[1] * t[i];

            fitfile << t[i] << " " << y[i] << " " << dy[i] << " " << yfit << "\n";
            logfile << t[i] << " " << logy[i] << " " << dlogy[i] << " " << logfit << "\n";
        }

        std::ofstream curvefile("thx_fit_curve.dat");
        curvefile << std::setprecision(10);

        double tmin = t[0];
        double tmax = t[0];
        for(size_t i = 1; i < t.size(); i++){
            if(t[i] < tmin) tmin = t[i];
            if(t[i] > tmax) tmax = t[i];
        }

        int Ncurve = 200;
        for(int j = 0; j < Ncurve; j++){
            double tj = tmin + (tmax - tmin) * j / (Ncurve - 1.0);
            double yfit = a * std::exp(-lambda * tj);
            curvefile << tj << " " << yfit << "\n";
        }

        std::cout << "See thx_fit.svg or thxlog_fit.svg\n";
    }


    if(do_taskB) {
        std::cout << "\n\n------ TASK B ------\n";

        auto fs = std::vector<pp::fitfunc>{
            [](double){ return 1.0; },
            [](double t){ return t; }
        };

        pp::vector t  = {1,2,3,4,6,9,10,13,15};
        pp::vector y  = {117,100,88,72,53,29.5,25.2,15.2,11.1};
        pp::vector dy = {6,5,4,4,4,3,3,2,2};

        pp::vector logy(y.size());
        pp::vector dlogy(y.size());

        for(size_t i = 0; i < y.size(); i++){
            logy[i]  = std::log(y[i]);
            dlogy[i] = dy[i]/y[i];
        }

        auto [c, Cov] = pp::lsfit2(fs, t, logy, dlogy);
        pp::vector dc = pp::fit_errors(Cov);

        // ln(y) = c0 + c1 t = ln(a) - lambda t
        double lna         = c[0];
        double sigma_lna   = dc[0];

        double lambda      = -c[1];
        double sigma_lambda= dc[1];

        double a           = std::exp(lna);
        double sigma_a     = a * sigma_lna;

        double T12         = std::log(2.0)/lambda;
        double sigma_T12   = std::log(2.0)/(lambda*lambda) * sigma_lambda;

        std::cout << "Result of new fitting function including covariance matrix:\n";

        std::cout << "c0 = ln(a) = " << lna
                << " +/- " << sigma_lna << "\n";
        std::cout << "c1 = -lambda = " << c[1]
                << " +/- " << dc[1] << "\n\n";

        std::cout << "a = " << a
                << " +/- " << sigma_a << "\n";
        std::cout << "lambda = " << lambda
                << " +/- " << sigma_lambda << " 1/day\n";
        std::cout << "T_1/2 = " << T12
                << " +/- " << sigma_T12 << " days\n\n";

        std::cout << "Covariance matrix:";
        Cov.print();

        double modern_T12 = 3.631; // http://www.lnhb.fr/nuclides/Ra-224_com.pdf
        std::cout <<   "\nExcpected T_{1/2} = " << modern_T12 << " days\n";

        double diff = std::abs(T12 - modern_T12);
        std::cout << "Difference = " << diff << " days\n";

        if(diff <= sigma_T12){
            std::cout << "Agreement: Yes, the expected value of T_1/2 is within the estimated 1-sigma fit uncertainty.\n";
        } else {
            std::cout << "Agreement: No, the expected value of T_1/2 is not within the estimated 1-sigma fit uncertainty.\n";
        }
    }

    if(do_taskC) {
        std::cout << "\n\n------ TASK C ------\n";

        auto fs = std::vector<pp::fitfunc>{
            [](double){ return 1.0; },
            [](double t){ return t; }
        };

        pp::vector t  = {1,2,3,4,6,9,10,13,15};
        pp::vector y  = {117,100,88,72,53,29.5,25.2,15.2,11.1};
        pp::vector dy = {6,5,4,4,4,3,3,2,2};

        pp::vector logy(y.size());
        pp::vector dlogy(y.size());

        for(size_t i = 0; i < y.size(); i++){
            logy[i]  = std::log(y[i]);
            dlogy[i] = dy[i]/y[i];
        }

        auto [c, Cov] = pp::lsfit2(fs, t, logy, dlogy);
        pp::vector dc = pp::fit_errors(Cov);

        std::cout << "Best-fit coefficients:\n";
        std::cout << "c0 = " << c[0] << " +/- " << dc[0] << "\n";
        std::cout << "c1 = " << c[1] << " +/- " << dc[1] << "\n";

        double tmin = t[0];
        double tmax = t[0];
        for(size_t i = 1; i < t.size(); i++){
            if(t[i] < tmin) tmin = t[i];
            if(t[i] > tmax) tmax = t[i];
        }

        std::ofstream f("thx_taskC.dat");
        f << std::setprecision(10);

        int Ncurve = 300;
        for(int i = 0; i < Ncurve; i++){
            double ti = tmin + (tmax - tmin) * i / (Ncurve - 1.0);

            double log_best = c[0] + c[1]*ti;

            double log_mm = (c[0] - dc[0]) + (c[1] - dc[1])*ti;
            double log_mp = (c[0] - dc[0]) + (c[1] + dc[1])*ti;
            double log_pm = (c[0] + dc[0]) + (c[1] - dc[1])*ti;
            double log_pp = (c[0] + dc[0]) + (c[1] + dc[1])*ti;

            double y_best = std::exp(log_best);
            double y_mm   = std::exp(log_mm);
            double y_mp   = std::exp(log_mp);
            double y_pm   = std::exp(log_pm);
            double y_pp   = std::exp(log_pp);

            f << ti << " "
            << y_best << " "
            << y_mm   << " "
            << y_mp   << " "
            << y_pm   << " "
            << y_pp   << "\n";
        }

        std::ofstream datafile("thx_data.dat");
        datafile << std::setprecision(10);
        for(size_t i = 0; i < t.size(); i++){
            datafile << t[i] << " " << y[i] << " " << dy[i] << "\n";
        }

        std::cout << "\nSee thx_taskC.svg";
    }

    return 0;
}