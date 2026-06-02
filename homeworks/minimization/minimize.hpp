#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <functional>
#include <initializer_list>
#include <limits>
#include <string>

#include "../lib/qr.h"

namespace minimize {

using vector = pp::vector;
using matrix = pp::matrix;

enum class method {
    forward,
    central
};

struct result {
    vector x;
    double fx;
    double grad_norm;
    std::size_t iterations;
    std::size_t evaluations;
    bool converged;
    method derivative_method;
};

struct derivatives {
    vector g;
    matrix H;
};

inline vector make_vector(std::initializer_list<double> values) {
    return vector(values);
}

inline std::string method_name(method m) {
    return m == method::central ? "central" : "forward";
}

inline double dot(const vector& a, const vector& b) {
    double sum = 0.0;
    for(std::size_t i = 0; i < a.size(); ++i) {
        sum += a[i] * b[i];
    }
    return sum;
}

inline void symmetrize(matrix& A) {
    const std::size_t n = A.size1();

    for(std::size_t i = 0; i < n; ++i) {
        for(std::size_t j = i + 1; j < n; ++j) {
            const double aij = 0.5 * (A(i,j) + A(j,i));
            A(i,j) = aij;
            A(j,i) = aij;
        }
    }
}

template<class F>
vector gradient_forward(F phi, const vector& x) {
    const std::size_t n = x.size();

    const double phix = phi(x);

    vector g(n);
    vector z = x;

    for(std::size_t i = 0; i < n; ++i) {
        const double dx = (1.0 + std::abs(x[i])) * std::pow(2.0, -26.0);

        z[i] += dx;
        g[i] = (phi(z) - phix) / dx;
        z[i] = x[i];
    }

    return g;
}

template<class F>
matrix hessian_forward(F phi, const vector& x) {
    const std::size_t n = x.size();

    matrix H(static_cast<int>(n), static_cast<int>(n));

    const vector gx = gradient_forward(phi, x);

    vector z = x;

    for(std::size_t j = 0; j < n; ++j) {
        const double dx = (1.0 + std::abs(x[j])) * std::pow(2.0, -13.0);

        z[j] += dx;
        const vector gz = gradient_forward(phi, z);

        for(std::size_t i = 0; i < n; ++i) {
            H(i,j) = (gz[i] - gx[i]) / dx;
        }

        z[j] = x[j];
    }

    symmetrize(H);

    return H;
}

template<class F>
derivatives derivatives_forward(F phi, const vector& x) {
    return {
        gradient_forward(phi, x),
        hessian_forward(phi, x)
    };
}

template<class F>
derivatives derivatives_central(F phi, const vector& x) {
    const std::size_t n = x.size();

    const double eps13 = std::cbrt(std::numeric_limits<double>::epsilon());

    const double f0 = phi(x);

    vector h(n);
    vector g(n);
    matrix H(static_cast<int>(n), static_cast<int>(n));

    for(std::size_t i = 0; i < n; ++i) {
        h[i] = eps13 * (1.0 + std::abs(x[i]));
    }

    vector xp = x;
    vector xm = x;

    for(std::size_t i = 0; i < n; ++i) {
        xp[i] += h[i];
        xm[i] -= h[i];

        const double fp = phi(xp);
        const double fm = phi(xm);

        g[i] = (fp - fm) / (2.0 * h[i]);
        H(i,i) = (fp - 2.0 * f0 + fm) / (h[i] * h[i]);

        xp[i] = x[i];
        xm[i] = x[i];
    }

    for(std::size_t i = 0; i < n; ++i) {
        for(std::size_t j = i + 1; j < n; ++j) {
            vector xpp = x;
            vector xpm = x;
            vector xmp = x;
            vector xmm = x;

            xpp[i] += h[i];
            xpp[j] += h[j];

            xpm[i] += h[i];
            xpm[j] -= h[j];

            xmp[i] -= h[i];
            xmp[j] += h[j];

            xmm[i] -= h[i];
            xmm[j] -= h[j];

            const double Hij = (
                phi(xpp) - phi(xpm) - phi(xmp) + phi(xmm)
            ) / (4.0 * h[i] * h[j]);

            H(i,j) = Hij;
            H(j,i) = Hij;
        }
    }

    return {g, H};
}

template<class F>
derivatives evaluate_derivatives(F phi, const vector& x, method derivative_method) {
    if(derivative_method == method::central) {
        return derivatives_central(phi, x);
    }
    return derivatives_forward(phi, x);
}

inline vector negative(const vector& x) {
    vector y(x.size());
    for(std::size_t i = 0; i < x.size(); ++i) {
        y[i] = -x[i];
    }
    return y;
}

template<class F>
bool linesearch(F phi,
                const vector& x,
                const vector& Dx,
                double phix,
                vector& z) {
    double lambda = 1.0;

    while(lambda >= 1.0 / 1024.0) {
        z = x + lambda * Dx;

        const double phiz = phi(z);

        if(std::isfinite(phiz) && phiz < phix) {
            return true;
        }

        lambda /= 2.0;
    }

    return false;
}

template<class F>
result newton(F phi,
              vector x,
              double acc = 1e-3,
              method derivative_method = method::forward,
              std::size_t max_iter = 1000) {
    std::size_t evaluations = 0;

    auto f = [&](const vector& y) {
        ++evaluations;
        return phi(y);
    };

    double phix = f(x);

    for(std::size_t iter = 0; iter < max_iter; ++iter) {
        const derivatives d = evaluate_derivatives(f, x, derivative_method);

        const double grad_norm = d.g.norm();

        if(grad_norm < acc) {
            return {
                x,
                phix,
                grad_norm,
                iter,
                evaluations,
                true,
                derivative_method
            };
        }

        vector rhs = negative(d.g);
        vector Dx = rhs;

        bool newton_step_found = false;

        double regularization = 1e-6;

        for(std::size_t attempt = 0; attempt < 8; ++attempt) {
            matrix H = d.H;

            for(std::size_t i = 0; i < x.size(); ++i) {
                H(i,i) += regularization;
            }

            pp::QRdec QRH(H);
            Dx = QRH.solve(rhs);

            if(std::isfinite(Dx.norm()) && dot(Dx, d.g) < 0.0) {
                newton_step_found = true;
                break;
            }

            regularization *= 10.0;
        }

        if(!newton_step_found) {
            Dx = rhs;
        }

        vector z = x;

        bool accepted = linesearch(f, x, Dx, phix, z);

        if(!accepted && newton_step_found) {
            Dx = rhs;
            accepted = linesearch(f, x, Dx, phix, z);
        }

        if(!accepted) {
            return {
                x,
                phix,
                grad_norm,
                iter,
                evaluations,
                false,
                derivative_method
            };
        }

        x = z;
        phix = f(x);
    }

    const derivatives d = evaluate_derivatives(f, x, derivative_method);

    return {
        x,
        phix,
        d.g.norm(),
        max_iter,
        evaluations,
        false,
        derivative_method
    };
}

} // namespace minimize