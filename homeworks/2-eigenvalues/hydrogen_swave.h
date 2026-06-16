#pragma once
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cmath>
#include <vector>
#include <algorithm>
#include <stdexcept>

#include "../lib/matrix.h"
#include "evd.h"

namespace pp {

inline vector hydrogen_grid(double rmax, double dr) {
    int npoints = (int)(rmax/dr) - 1;
    if(npoints < 1) throw std::runtime_error("hydrogen_grid: too few grid points");
    vector r(npoints);
    for(int i = 0; i < npoints; ++i) r[i] = dr*(i+1);
    return r;
}

inline matrix hydrogen_hamiltonian(double rmax, double dr, vector& r) {
    r = hydrogen_grid(rmax, dr);
    int npoints = (int)r.size();

    matrix H(npoints, npoints);
    double c = -0.5/(dr*dr);

    for(int i = 0; i < npoints-1; ++i) {
        H(i,i)     = -2*c;
        H(i,i+1)   = 1*c;
        H(i+1,i)   = 1*c;
    }
    H(npoints-1,npoints-1)=-2*(-0.5/dr/dr);
    for(int i=0;i<npoints;i++)H(i,i)+=-1/r[i];

    return H;
}

inline double hydrogen_exact_energy(int n) {
    return -0.5/(n*n);
}

inline std::vector<int> sort_indices(const vector& w) {
    std::vector<int> idx(w.size());
    for(size_t i = 0; i < w.size(); ++i) idx[i] = (int)i;
    std::sort(idx.begin(), idx.end(),
        [&](int a, int b){ return w[a] < w[b]; });
    return idx;
}

inline double hydrogen_exact_f(int n, double r) {
    if(n == 1) return 2*r*std::exp(-r);
    if(n == 2) {
        return r*(2-r)*std::exp(-r/2)/(2*std::sqrt(2));
    }
    if(n == 3) {
        return 2*r*(27 - 18*r + 2*r*r)*std::exp(-r/3)/(81*std::sqrt(3));
    }
    return 0.0;
}

inline void print_hydrogen_energies(const vector& w, int nstates=3) {
    auto idx = sort_indices(w);
    std::cout << "\nLowest hydrogen s-wave energies (atomic units):\n";
    for(int k = 0; k < nstates && k < (int)idx.size(); ++k) {
        int j = idx[k];
        int n = k + 1;
        double exact = hydrogen_exact_energy(n);
        std::cout << "n=" << n
                  << "  E_num=" << w[j]
                  << "  E_exact=" << exact
                  << "  error=" << (w[j] - exact)
                  << "\n";
    }
}

inline void save_hydrogen_wavefunctions(
    const char* filename,
    const vector& r,
    const matrix& V,
    const vector& w,
    double dr,
    int nstates=3
){
    auto idx = sort_indices(w);
    int m = std::min(nstates, (int)idx.size());

    double c = 1.0/std::sqrt(dr);

    std::vector<double> sign(m, 1.0);

    // Eigenvectors have arbitrary sign. This flips them to match the analytical functions.
    for(int k = 0; k < m; ++k) {
        double overlap = 0.0;
        for(size_t i = 0; i < r.size(); ++i) {
            double fnum = c*V(i, idx[k]);
            double fex  = hydrogen_exact_f(k+1, r[i]);
            overlap += fnum*fex*dr;
        }
        if(overlap < 0) sign[k] = -1.0;
    }

    std::ofstream out(filename);
    out << std::setprecision(10);
    out << "# r";
    for(int k = 0; k < m; ++k) {
        out << " f" << (k+1) << "_num"
            << " f" << (k+1) << "_exact";
    }
    out << "\n";

    for(size_t i = 0; i < r.size(); ++i) {
        out << r[i];

        for(int k = 0; k < m; ++k) {
            double fnum = sign[k]*c*V(i, idx[k]);
            double fex  = hydrogen_exact_f(k+1, r[i]);
            out << " " << fnum << " " << fex;
        }

        out << "\n";
    }
}

inline double lowest_hydrogen_energy(double rmax, double dr) {
    vector r;
    matrix H = hydrogen_hamiltonian(rmax, dr, r);
    EVD_slow evd(H);
    auto idx = sort_indices(evd.w);
    return evd.w[idx[0]];
}

inline void save_convergence_vs_dr(
    const char* filename,
    double rmax,
    const std::vector<double>& dr_values
){
    std::ofstream out(filename);
    out << std::setprecision(10);
    out << "# dr E0 E_exact error\n";
    for(double dr : dr_values) {
        double E0 = lowest_hydrogen_energy(rmax, dr);
        double Eexact = hydrogen_exact_energy(1);
        out << dr << " " << E0 << " " << Eexact << " " << (E0 - Eexact) << "\n";
    }
}

inline void save_convergence_vs_rmax(
    const char* filename,
    double dr,
    const std::vector<double>& rmax_values
){
    std::ofstream out(filename);
    out << std::setprecision(10);
    out << "# rmax E0 E_exact error\n";
    for(double rmax : rmax_values) {
        double E0 = lowest_hydrogen_energy(rmax, dr);
        double Eexact = hydrogen_exact_energy(1);
        out << rmax << " " << E0 << " " << Eexact << " " << (E0 - Eexact) << "\n";
    }
}

} // namespace pp