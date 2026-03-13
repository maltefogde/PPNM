#pragma once
#include <cmath>
#include <cassert>
#include "matrix.h"

namespace pp {

struct QRdec{
    matrix Q, R;
    QRdec(const matrix& A);
    vector solve(const vector& b) const;
    double det() const;
    matrix inverse() const;
};

inline QRdec::QRdec(const matrix& A){
    int n = (int)A.size1();
    int m = (int)A.size2();
    assert(n >= m);

    Q = A;          // will be orthogonalized in-place
    R = matrix(m,m);

    for(int i=0; i<m; i++){
        double norm = Q[i].norm();
        assert(norm > 1e-12);      // columns must be linearly independent
        R(i,i) = norm;
        Q[i] /= norm;

        for(int j=i+1; j<m; j++){
            R(i,j) = Q[i] * Q[j];
            Q[j] -= Q[i] * R(i,j);
        }
    }
}

inline vector QRdec::solve(const vector& b) const {   // solve Ax=b
    vector x = Q.transpose() * b;                     // compute Q^T b
    backsub(R, x);                                    // solve Rx = Q^T b
    return x;
}

inline double QRdec::det() const {
    assert(R.size1() == R.size2());
    double d = 1.0;
    for(size_t i=0; i<R.size1(); i++) d *= R(i,i);
    return d;
}

inline matrix QRdec::inverse() const { // A^{-1} by solving A x_i = e_i
    assert(R.size1() == R.size2());
    int n = R.size1(); 
    matrix B(n,n);

    for(int i=0; i<n; i++){
        vector e(n);
        e[i] = 1.0;
        vector x = solve(e); // The solution x_i becomes column i of A^{-1}.

        for(int j=0; j<n; j++){
            B(j,i) = x[j];
        }
    }
    return B;
}

} // namespace pp