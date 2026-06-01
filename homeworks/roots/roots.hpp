#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <initializer_list>
#include <vector>

#include "../lib/qr.h"
#include "../ode/ode.h"

namespace roots {

using vector = pp::vector;
using matrix = pp::matrix;

struct newton_result {
    vector x;
    double fx_norm;
    std::size_t iterations;
    bool converged;
};

struct hydrogen_result {
    std::vector<double> r;
    std::vector<double> f;
};

inline vector make_vector(std::initializer_list<double> values) {
    return vector(values);
}

inline vector default_dx(const vector& x) {
    vector dx(x.size());

    const double step = std::pow(2.0, -26.0);

    for(std::size_t i = 0; i < x.size(); ++i) {
        dx[i] = std::max(std::abs(x[i]), 1.0) * step;
    }

    return dx;
}

template<class F>
void jacobian(F f, const vector& x, const vector& fx,
              const vector& dx, matrix& J) {
    const std::size_t n = x.size();
    vector z = x;

    for(std::size_t j = 0; j < n; ++j) {
        z[j] += dx[j];

        const vector fz = f(z);

        for(std::size_t i = 0; i < n; ++i) {
            J(i,j) = (fz[i] - fx[i]) / dx[j];
        }

        z[j] = x[j];
    }
}

template<class F>
newton_result newton(F f, vector x, double acc = 1e-2,
                     double alpha_min = 1e-3,
                     std::size_t max_iter = 100,
                     const vector& user_dx = vector(),
                     bool quadratic_linesearch = false) {
    const std::size_t n = x.size();

    vector fx = f(x);
    double fx_norm = fx.norm();

    matrix J(static_cast<int>(n), static_cast<int>(n));

    for(std::size_t iter = 0; iter < max_iter; ++iter) {
        if(fx_norm < acc) {
            return {x, fx_norm, iter, true};
        }

        const vector dx = (user_dx.size() == 0) ? default_dx(x) : user_dx;

        jacobian(f, x, fx, dx, J);

        vector rhs(n);

        for(std::size_t i = 0; i < n; ++i) {
            rhs[i] = -fx[i];
        }

        pp::QRdec QRJ(J);
        const vector Dx = QRJ.solve(rhs);

        if(Dx.norm() < dx.norm()) {
            return {x, fx_norm, iter, false};
        }

        double alpha = 1.0;
        vector z(n);
        vector fz(n);
        double fz_norm = fx_norm;

        while(true) {
            z = x + alpha * Dx;
            fz = f(z);
            fz_norm = fz.norm();

            if(!quadratic_linesearch && fz_norm < fx_norm) {
                break;
            }

            if(quadratic_linesearch && fz_norm < (1.0 - alpha / 2.0) * fx_norm) {
                break;
            }

            if(alpha < alpha_min) {
                break;
            }

            if(quadratic_linesearch) {
                const double denominator = fz_norm - (1.0 - alpha) * fx_norm;

                if(denominator > 0.0) {
                    double alpha_new = alpha * alpha * fx_norm / (2.0 * denominator);

                    alpha_new = std::max(alpha / 4.0, alpha_new);
                    alpha_new = std::min(alpha / 2.0, alpha_new);

                    alpha = alpha_new;
                } else {
                    alpha /= 2.0;
                }
            } else {
                alpha /= 2.0;
            }
        }

        x = z;
        fx = fz;
        fx_norm = fz_norm;

        if((alpha * Dx).norm() < dx.norm()) {
            return {x, fx_norm, iter + 1, fx_norm < acc};
        }
    }

    return {x, fx_norm, max_iter, fx_norm < acc};
}

inline double hydrogen_M(double E, double rmin, double rmax,
                         double acc = 1e-6,
                         double eps = 1e-6,
                         double h = 0.01) {
    auto schrodinger = [E](double r, const vec& y) {
        const double f = y[0];
        const double fp = y[1];

        const double fpp = -2.0 * (E + 1.0 / r) * f;

        return vec{fp, fpp};
    };

    const vec yinit{
        rmin - rmin * rmin,
        1.0 - 2.0 * rmin
    };

    const auto [rlist, ylist] = driver(
        schrodinger,
        rmin,
        rmax,
        yinit,
        h,
        acc,
        eps
    );

    (void)rlist;

    return ylist.back()[0];
}

inline hydrogen_result hydrogen_wavefunction(double E, double rmin, double rmax,
                                             double acc = 1e-6,
                                             double eps = 1e-6,
                                             double h = 0.01) {
    auto schrodinger = [E](double r, const vec& y) {
        const double f = y[0];
        const double fp = y[1];

        const double fpp = -2.0 * (E + 1.0 / r) * f;

        return vec{fp, fpp};
    };

    const vec yinit{
        rmin - rmin * rmin,
        1.0 - 2.0 * rmin
    };

    const auto [rlist, ylist] = driver(
        schrodinger,
        rmin,
        rmax,
        yinit,
        h,
        acc,
        eps
    );

    hydrogen_result result;
    result.r = rlist;
    result.f.resize(ylist.size());

    for(std::size_t i = 0; i < ylist.size(); ++i) {
        result.f[i] = ylist[i][0];
    }

    return result;
}

inline double exact_hydrogen_f(double r) {
    return r * std::exp(-r);
}

} // namespace roots