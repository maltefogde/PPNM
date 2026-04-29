#pragma once
#include <vector>
#include <cassert>
#include <cmath>
#include <iomanip>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>

using vec=std::vector<double>;

int binsearch(const vec& x, double z){
    // The inserted vector has to be sorted!
    /* locates the interval for z by bisection */
    assert(z >= x[0] && z <= x[x.size()-1]);
    int i = 0, j = (int)x.size() - 1;
    while(j - i > 1){
        int mid = (i + j)/2;
        if(z > x[mid]) i = mid;
        else j = mid;
    }
    return i;
}

double linterp(const vec& x,const vec& y,double z){
    int i = binsearch(x, z);
    double dx = x[i+1] - x[i];
    assert(dx > 0);
    double dy = y[i+1] - y[i];
    return y[i] + dy/dx * (z - x[i]); // Equation 1.5, the linerpolation in the point z
}


double linterpInteg(const vec& x,const vec& y,double z){
    int i = binsearch(x, z); // x_i \leq z \leq x_{i+1}
    double sum = 0.0;

    // Integral from x_0 to x_i
    for(int k = 0; k < i; ++k){
        double dx = x[k+1] - x[k];
        assert(dx > 0);
        sum += 0.5 * (y[k+1] + y[k])*dx;
        // = y[k]*(x[k+1] - x[k]) + 0.5* ((y[k+1] - y[k])/(x[k+1] - x[k]))*(x[k+1] - x[k])^2, like Equation 1.8
        // = (y[k] + 0.5* (y[k+1] - y[k]))*(x[k+1] - x[k])
        // = 0.5 * (y[k+1] + y[k]))*(x[k+1] - x[k])
    }

    // The integral of the last step (not a full interval): From x_i to z
    double dx = x[i+1] - x[i];
    assert(dx > 0);
    double dy = y[i+1] - y[i];
    double h = z - x[i];
    sum += y[i]*h + 0.5 * (dy/dx)*h*h;

    return sum;
}

struct QuadraticSpline{
	const int n;
	vec x,y,b,c;
	QuadraticSpline(const vec& x, const vec& y): n(x.size()), x(x), y(y), b(n-1), c(n-1){// ctor
        assert(x.size() == y.size());
        assert(n >= 2);

        int i ; vec p(n-1), h(n-1);
        for (i=0; i<n-1; i ++){
            h[i] = x[i+1] - x[i];
            assert(h[i] > 0);
            p[i] = (y[i+1]-y[i])/h[i];
        }

        // calculating c
        c[0] = 0; // recursion up: Equation 1.13
        for (i=0; i<n-2; i++){
            c[i+1] = (p[i+1] - p[i] - c[i] * h[i])/h[i+1];
        }

        c[n-2]/=2; // recursion down: Equation 1.14
        for (i=n-3; i>=0; i--){
            c[i]=(p[i+1] - p[i] - c[i+1] * h[i+1])/h[i];
        }
        
        // calculating b, Equation 1.15
        for (i=0; i<n-1; i++){
            b[i]=p[i] - c[i] * h[i];
        }
	}

	int binsearch(double z){
        assert(z >= x[0] && z <= x[x.size()-1]);
        int i = 0, j = (int)x.size() - 1;
        while(j - i > 1){
            int mid = (i + j)/2;
            if(z > x[mid]) i = mid;
            else j = mid;
        }
        return i;
    };

	double eval(double z){
        assert(z>=x[0] && z<=x[n-1]);
        int i = binsearch(z);
        double h=z-x[i];
        return y[i] + h*(b[i] + h*c[i]);
    }


	double deriv(double z){
        assert(z>=x[0] && z<=x[n-1]);
        int i = binsearch(z);
        double h=z-x[i];
        return b[i] + 2*h*c[i];
    }

	double integ(double z){
        assert(z>=x[0] && z<=x[n-1]);
        int i = binsearch(z);
        double sum = 0.0;

        for(int k = 0; k < i; ++k){
            double dx = x[k+1] - x[k];
            assert(dx > 0);
            sum += dx*(y[k] + dx*(b[k]*0.5 + dx*c[k]/3));
        }

        // The integral of the last step (not a full interval): From x_i to z
        double h = z - x[i];
        assert(h >= 0);
        sum += h*(y[i] + h*(b[i]*0.5 + h*c[i]/3));

        return sum;
    }
};

void print_vec(const vec& v){
    for(double x : v) std::cout << x << " ";
    std::cout << "\n";
}

// Made test_spline using ChatGPT v5
void test_spline(const vec& x, const vec& y, const std::string& name, int mode){
    QuadraticSpline s(x,y);

    double max_err_val = 0.0;
    double max_err_der = 0.0;
    double max_err_int = 0.0;

    std::ofstream out(name + ".dat");
    for(double z = x.front(); z <= x.back() + 1e-12; z += 0.01){
        double exact_val, exact_der, exact_int;

        if(mode == 0){ // y = 1
            exact_val = 1.0;
            exact_der = 0.0;
            exact_int = z - 1.0;
        }
        else if(mode == 1){ // y = x
            exact_val = z;
            exact_der = 1.0;
            exact_int = (z*z - 1.0)/2.0;
        }
        else{ // y = x^2
            exact_val = z*z;
            exact_der = 2.0*z;
            exact_int = (z*z*z - 1.0)/3.0;
        }

        double sval = s.eval(z);
        double sder = s.deriv(z);
        double sint = s.integ(z);

        max_err_val = std::max(max_err_val, std::abs(sval - exact_val));
        max_err_der = std::max(max_err_der, std::abs(sder - exact_der));
        max_err_int = std::max(max_err_int, std::abs(sint - exact_int));

        out << z << " "
            << sval << " "
            << sder << " "
            << sint << "\n";
    }

    std::ofstream pts(name + "_points.dat");
    for(size_t i = 0; i < x.size(); ++i){
        pts << x[i] << " " << y[i] << "\n";
    }

    std::cout << "\n--- " << name << " ---\n";

    if(mode == 0){
        std::cout << "Analytical case: y = 1\n";
        std::cout << "Expected spline:      S(x) = 1\n";
        std::cout << "Expected derivative:  S'(x) = 0\n";
        std::cout << "Expected integral:    I(x) = x - 1\n";
        std::cout << "Expected b: 0 0 0 0\n";
        std::cout << "Expected c: 0 0 0 0\n";
    }
    else if(mode == 1){
        std::cout << "Analytical case: y = x\n";
        std::cout << "Expected spline:      S(x) = x\n";
        std::cout << "Expected derivative:  S'(x) = 1\n";
        std::cout << "Expected integral:    I(x) = (x^2 - 1)/2\n";
        std::cout << "Expected b: 1 1 1 1\n";
        std::cout << "Expected c: 0 0 0 0\n";
    }
    else{
        std::cout << "Analytical case: y = x^2\n";
        std::cout << "Expected spline:      S(x) = x^2\n";
        std::cout << "Expected derivative:  S'(x) = 2x\n";
        std::cout << "Expected integral:    I(x) = (x^3 - 1)/3\n";
        std::cout << "Expected b: 2 4 6 8\n";
        std::cout << "Expected c: 1 1 1 1\n";
    }

    std::cout << "Computed b: ";
    print_vec(s.b);
    std::cout << "Computed c: ";
    print_vec(s.c);

    std::cout << std::scientific << std::setprecision(6);
    std::cout << "max |S - S_exact|   = " << max_err_val << "\n";
    std::cout << "max |S' - S'_exact| = " << max_err_der << "\n";
    std::cout << "max |I - I_exact|   = " << max_err_int << "\n";
}


// Making CubicSpline using OPP style.
struct CubicSpline{
    int n;
    vec x, y, b, c, d;
    CubicSpline(const vec& x, const vec& y): n(x.size()), x(x), y(y), b(n), c(n-1), d(n-1){// ctor
        assert(x.size() == y.size());
        assert(n >= 2);
        
        vec p(n-1), h(n-1);
        for (int i=0; i<n-1; i ++){
            h[i] = x[i+1] - x[i];
            assert(h[i] > 0);
            p[i] = (y[i+1]-y[i])/h[i];
        };

        vec D(n), Q(n-1), B(n);

        // Build tridiagonal system
        D[0] = 2.0;
        for (int i = 0; i < n-2; ++i) {
            D[i+1] = 2.0 * h[i] / h[i+1] + 2.0;
        };
        D[n-1] = 2.0;

        Q[0] = 1.0;
        for (int i = 0; i < n-2; ++i) {
            Q[i+1] = h[i] / h[i+1];
        };
        B[0] = 3.0 * p[0];

        for (int i = 0; i < n-2; ++i) {
            B[i+1] = 3.0 * (p[i] + p[i+1] * h[i] / h[i+1]);
        }
        B[n-1] = 3.0 * p[n-2];

        // Gaussian elimination
        for (int i = 1; i < n; ++i) {
            D[i] -= Q[i-1] / D[i-1];
            B[i] -= B[i-1] / D[i-1];
        }

        // Back-substitution
        b[n-1] = B[n-1] / D[n-1];

        for (int i = n-2; i >= 0; --i) {
            b[i] = (B[i] - Q[i] * b[i+1]) / D[i];
        }

        for (int i = 0; i < n-1; ++i) {
            c[i] = (-2.0 * b[i] - b[i+1] + 3.0 * p[i]) / h[i];
            d[i] = (b[i] + b[i+1] - 2.0 * p[i]) / (h[i] * h[i]);
        }
    };

    int binsearch(double z){
        assert(z >= x[0] && z <= x[n-1]);
        int i = 0, j = n - 1;
        while(j - i > 1){
            int mid = (i + j)/2;
            if(z > x[mid]) i = mid;
            else j = mid;
        }
        return i;
    };

    double eval(double z){
        assert(z>=x[0] && z<=x[n-1]);
        int i = binsearch(z);
        double h=z-x[i];
        return y[i] + h * (b[i] + h * (c[i] + h * d[i]));
    }

    double deriv(double z){
        assert(z>=x[0] && z<=x[n-1]);
        int i = binsearch(z);
        double h=z-x[i];
        // d/dh(y[i] + h * b[i] + h *h * c[i] + h * h * h * d[i]) = 
        return b[i] + 2.0*h*c[i] + 3.0*h*h*d[i];
    }

    double integ(double z){
        assert(z>=x[0] && z<=x[n-1]);
        int i = binsearch(z);
        double sum = 0.0;

        for(int k = 0; k < i; ++k){
            double dx = x[k+1] - x[k];
            assert(dx > 0);
            // integral of (y[i] + h * b[i] + h *h * c[i] + h * h * h * d[i]) where h=dx
            // (y[i]*dx + dx^2 * b[i]*0.5 + dx^3 c[i] / 3.0 + h^4 * d[k] /4.0) 
            sum += dx*(y[k] + dx*(b[k]*0.5 + dx*(c[k]/3 + dx*d[k] /4.0)));
        }

        // The integral of the last step (not a full interval): From x_i to z
        double h = z - x[i];
        assert(h >= 0);
        sum += h*(y[i] + h*(b[i]*0.5 + h*(c[i]/3 + h*d[i] /4.0)));

        return sum;
    }
};

// Test taskC_cubic_gnuplot_comparison made using ChatGPT
void taskC_cubic_gnuplot_comparison(){
    // Sparse interpolation points
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