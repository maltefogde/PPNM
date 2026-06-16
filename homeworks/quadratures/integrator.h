#pragma once

#include <cmath>
#include <limits>
#include <stdexcept>

struct integration_result {
    double value;
    double error;
};

// "integrate" written myself and improved using ChatGPT
template<class F>
integration_result integrate(
    F& f,
    double a,
    double b,
    double acc = 1e-3,
    double eps = 1e-3,
    double f2 = std::numeric_limits<double>::quiet_NaN(),
    double f3 = std::numeric_limits<double>::quiet_NaN(),
    int depth = 0
) {
    if(depth > 100000) {
        throw std::runtime_error("integrate: maximum recursion depth reached");
    }

    double h = b - a;

    if(h == 0.0) {
        return {0.0, 0.0};
    }

    // First call: no reusable points
    if(std::isnan(f2)) {
        f2 = f(a + 2.0*h/6.0);
        f3 = f(a + 4.0*h/6.0);
    }

    // New function evaluations
    double f1 = f(a + h/6.0);
    double f4 = f(a + 5.0*h/6.0);

    // Higher-order rule
    double Q = (2.0*f1 + f2 + f3 + 2.0*f4)/6.0 * h;

    // Lower-order rule
    double q = (f1 + f2 + f3 + f4)/4.0 * h;

    double err = std::abs(Q - q);
    double tol = acc + eps*std::abs(Q);

    if(err < tol) {
        return {Q, err};
    }

    double m = 0.5*(a + b);

    auto left = integrate(
        f,
        a,
        m,
        acc/std::sqrt(2.0),
        eps,
        f1,
        f2,
        depth + 1
    );

    auto right = integrate(
        f,
        m,
        b,
        acc/std::sqrt(2.0),
        eps,
        f3,
        f4,
        depth + 1
    );

    return {
        left.value + right.value,
        std::sqrt(left.error*left.error + right.error*right.error)
    };
}

// "integrate_cc_finite" written myself and improved using ChatGPT
template<class F>
integration_result integrate_cc_finite(
    F& f,
    double a,
    double b,
    double acc = 1e-3,
    double eps = 1e-3
) {
    const double pi = std::acos(-1.0);

    if(a == b) {
        return {0.0, 0.0};
    }

    double center = 0.5*(a + b);
    double halfwidth = 0.5*(b - a);

    auto g = [&f, center, halfwidth](double theta) {
        double x = center + halfwidth*std::cos(theta);
        return f(x)*std::sin(theta)*halfwidth;
    };

    return integrate(g, 0.0, pi, acc, eps);
}

// "integrate_cc" written myself and improved using ChatGPT
template<class F>
integration_result integrate_cc(
    F& f,
    double a,
    double b,
    double acc = 1e-3,
    double eps = 1e-3
) {
    if(a == b) {
        return {0.0, 0.0};
    }

    if(a > b) {
        auto result = integrate_cc(f, b, a, acc, eps);

        return {
            -result.value,
            result.error
        };
    }

    // Finite interval
    if(std::isfinite(a) && std::isfinite(b)) {
        return integrate_cc_finite(f, a, b, acc, eps);
    }

    // Integral from a to infinity
    if(std::isfinite(a) && std::isinf(b)) {
        auto g = [&f, a](double t) {
            if(t >= 1.0) {
                return 0.0;
            }

            double u = 1.0 - t;
            double x = a + t/u;

            return f(x)/(u*u);
        };

        return integrate_cc_finite(g, 0.0, 1.0, acc, eps);
    }

    // Integral from -infinity to b
    if(std::isinf(a) && std::isfinite(b)) {
        auto g = [&f, b](double t) {
            if(t <= 0.0) {
                return 0.0;
            }

            double x = b - (1.0 - t)/t;

            return f(x)/(t*t);
        };

        return integrate_cc_finite(g, 0.0, 1.0, acc, eps);
    }

    // Integral from -infinity to infinity
    if(std::isinf(a) && std::isinf(b)) {
        const double pi = std::acos(-1.0);

        auto g = [&f, pi](double t) {
            if(t <= 0.0 || t >= 1.0) {
                return 0.0;
            }

            double u = pi*(t - 0.5);
            double c = std::cos(u);
            double x = std::tan(u);

            return f(x)*pi/(c*c);
        };

        return integrate_cc_finite(g, 0.0, 1.0, acc, eps);
    }

    throw std::runtime_error("integrate_cc: invalid interval");
}