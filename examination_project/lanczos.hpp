#pragma once

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <vector>

#include "../homeworks/lib/matrix.h"

namespace lanczos {

using vector = pp::vector;
using matrix = pp::matrix;

struct result {
    matrix V;
    matrix T;
    std::size_t iterations;
    bool breakdown;
};

inline bool is_square(const matrix& A) {
    return A.size1() == A.size2();
}

inline bool is_symmetric(const matrix& A, double tol = 1e-12) {
    if(!is_square(A)) return false;

    const std::size_t n = A.size1();

    for(std::size_t i = 0; i < n; ++i) {
        for(std::size_t j = i + 1; j < n; ++j) {
            if(std::abs(A(i,j) - A(j,i)) > tol) return false;
        }
    }

    return true;
}

// Implementing the standard version of the Lanczos algorithm
inline result tridiagonalize(const matrix& A, vector q, std::size_t nsteps,
                             double tol = 1e-14) {
    assert(is_square(A));
    assert(is_symmetric(A));
    assert(q.size() == A.size1());

    const std::size_t N = A.size1();
    const std::size_t mmax = std::min(nsteps, N);
    assert(mmax > 0);

    const double qnorm = q.norm();
    assert(qnorm > tol);

    q /= qnorm; // Normalising the vector

    vector qprev(N);

    std::vector<vector> basis;
    std::vector<double> alpha;
    std::vector<double> beta;

    bool breakdown = false;

    for(std::size_t k = 0; k < mmax; ++k) {
        basis.push_back(q); // Storing the current basis vector

        vector w = A * q; // Applying matrix A (Power iteration)

        if(k > 0) {
            w -= beta[k - 1] * qprev; // The component along the previous vector is removed
        }

        const double a = q * w; // a = q^T * A * q
        alpha.push_back(a);

        w -= a * q; // The component along the current vector is removed
        // Note that we do stop here and do not do a full Gram-Schmidt like Arnoldi

        const double b = w.norm(); // Lanczos breakdown (also avoids dividing by zero)

        if(k + 1 == mmax) {
            break;
        }

        if(b < tol) {
            breakdown = true;
            break;
        }

        // In case of no Lanczos breakdown, a new q is formed:
        beta.push_back(b);

        qprev = q;
        q = w / b;
    }

    const std::size_t m = basis.size();

    // Constructing the Krylov basis matrix
    matrix V(static_cast<int>(N), static_cast<int>(m));
    for(std::size_t j = 0; j < m; ++j) {
        V[j] = basis[j];
    }

    // Constructing the tridiagonal matrix
    matrix T(static_cast<int>(m), static_cast<int>(m));
    for(std::size_t i = 0; i < m; ++i) {
        T(i,i) = alpha[i];
    }
    for(std::size_t i = 0; i + 1 < m; ++i) {
        T(i,i + 1) = beta[i];
        T(i + 1,i) = beta[i];
    }

    return {V, T, m, breakdown};
}

// To find V^T * A * V
inline matrix project(const matrix& A, const matrix& V) {
    return V.transpose() * A * V;
}


inline double max_abs_difference(const matrix& A, const matrix& B) {
    assert(A.size1() == B.size1());
    assert(A.size2() == B.size2());

    double error = 0.0;
    for(std::size_t i = 0; i < A.size1(); ++i) {
        for(std::size_t j = 0; j < A.size2(); ++j) {
            error = std::max(error, std::abs(A(i,j) - B(i,j)));
        }
    }
    return error;
}

inline double orthogonality_error(const matrix& V) {
    const std::size_t m = V.size2();

    matrix I(static_cast<int>(m), static_cast<int>(m));
    I.setid();

    return max_abs_difference(V.transpose() * V, I);
}

inline double tridiagonal_error(const matrix& T) {
    assert(is_square(T));

    double error = 0.0;
    for(std::size_t i = 0; i < T.size1(); ++i) {
        for(std::size_t j = 0; j < T.size2(); ++j) {
            if(i + 1 < j || j + 1 < i) {
                error = std::max(error, std::abs(T(i,j)));
            }
        }
    }
    return error;
}

} // namespace lanczos