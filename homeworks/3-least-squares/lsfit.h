#pragma once
#include <vector>
#include <functional>
#include <tuple>
#include <cmath>
#include <cassert>

#include "../lib/matrix.h"
#include "../lib/qr.h"

namespace pp {

using fitfunc = std::function<double(double)>;

inline vector lsfit(
    const std::vector<fitfunc>& fs,
    const vector& x,
    const vector& y,
    const vector& dy
){
    assert(x.size() == y.size());
    assert(y.size() == dy.size());

    const size_t n = x.size();
    const size_t m = fs.size();
    assert(n >= m); // at least as many data points as fit parameters

    matrix A((int)n, (int)m);
    vector b(n);

    for(size_t i = 0; i < n; i++){
        assert(dy[i] != 0.0);
        b[i] = y[i] / dy[i];
        for(size_t k = 0; k < m; k++){
            A(i,k) = fs[k](x[i]) / dy[i];
        }
    }

    QRdec qr(A);
    return qr.solve(b);
}


inline matrix inverse_upper_triangular(const matrix& U){
    assert(U.size1() == U.size2());
    size_t n = U.size1();

    matrix Uinv((int)n, (int)n);

    for(size_t i = 0; i < n; i++){
        vector e(n);
        e[i] = 1.0;
        backsub(U, e);   // solves U x = e_i
        for(size_t j = 0; j < n; j++){
            Uinv(j,i) = e[j];   // column i
        }
    }
    return Uinv;
}

inline std::tuple<vector,matrix> lsfit2(
    const std::vector<fitfunc>& fs,
    const vector& x,
    const vector& y,
    const vector& dy
){
    assert(x.size() == y.size());
    assert(y.size() == dy.size());

    const size_t n = x.size();
    const size_t m = fs.size();
    assert(n >= m);

    matrix A((int)n, (int)m);
    vector b(n);

    for(size_t i = 0; i < n; i++){
        assert(dy[i] != 0.0);
        b[i] = y[i] / dy[i];
        for(size_t k = 0; k < m; k++){
            A(i,k) = fs[k](x[i]) / dy[i];
        }
    }

    QRdec qr(A);
    vector c = qr.solve(b);

    matrix Rinv = inverse_upper_triangular(qr.R);
    matrix Cov  = Rinv * Rinv.transpose();

    return {c, Cov};
}

inline vector fit_errors(const matrix& Cov){
    assert(Cov.size1() == Cov.size2());
    vector dc(Cov.size1());
    for(size_t i = 0; i < Cov.size1(); i++){
        dc[i] = std::sqrt(Cov(i,i));
    }
    return dc;
}

inline double fit_value(
    const std::vector<fitfunc>& fs,
    const vector& c,
    double z
){
    assert(fs.size() == c.size());
    double sum = 0.0;
    for(size_t k = 0; k < c.size(); k++){
        sum += c[k] * fs[k](z);
    }
    return sum;
}


} // namespace pp