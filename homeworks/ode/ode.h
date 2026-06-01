#pragma once

#include <tuple>
#include <vector>
#include <functional>
#include <cmath>
#include <algorithm>
#include <stdexcept>
#include <limits>

#include "../lib/vec.h"


inline std::tuple<vec, vec> rkstep12( // Stepper
    std::function<vec(double, const vec&)> f,
    double x,           /* the current value of the variable */
    const vec& y,       /* the current value y(x) of the sought function */
    double h            /* the step to be taken */
) {
    vec k0 = f(x, y);                   /* embedded lower order formula (Euler) */
    vec k1 = f(x + h/2, y + k0*(h/2));  /* higher order formula (midpoint) */

    vec yh = y + k1 * h;                /* y(x+h) estimate */
    vec dy = (k1 - k0) * h;             /* error estimate */

    return std::make_tuple(yh, dy);
}


inline std::tuple<std::vector<double>, std::vector<vec>> driver( // Driver
    std::function<vec(double, const vec&)> f,   /* the f from dy/dx=f(x,y) */
    double a, double b,                         /* initial-point,final-point */
    const vec& yinit,                           /* y(initial-point) */
    double h = 0.125,                           /* initial step-size */
    double acc = 0.01,                          /* absolute accuracy goal */
    double eps = 0.01                           /* relative accuracy goal */
) {
    // If there is no difference between initial point and final point
    if (a == b) {
        return std::make_tuple(std::vector<double>{a}, std::vector<vec>{yinit});
    }

    // If step-size is zero: Error
    if (h == 0.0) {
        throw std::runtime_error("Initial step size h must be non-zero");
    }

    double x = a;
    vec y = yinit;

    std::vector<double> xlist;
    std::vector<vec> ylist;

    xlist.push_back(x);
    ylist.push_back(y);

    const double direction = (b > a) ? 1.0 : -1.0; // ? is a compact if-else
    const double interval = std::abs(b - a);

    h = std::abs(h) * direction;

    while ((b - x) * direction > 0) {
        if ((x + h - b) * direction > 0) {
            h = b - x;
        } /* last step should end at b */

        auto [yh, dy] = rkstep12(f, x, y, h);

        double err = dy.norm();
        double tol = (acc + eps * yh.norm()) * std::sqrt(std::abs(h) / interval);

        if (err <= tol || err == 0.0) {
            x += h;
            y = yh;

            xlist.push_back(x);
            ylist.push_back(y);
        }

        if (err > 0.0) {
            double factor = 0.95 * std::pow(tol / err, 0.25);
            factor = std::min(factor, 2.0);
            factor = std::max(factor, 0.1);
            h *= factor;
        } else { // Stepsize is increased if err <= 0.0
            h *= 2.0;
        }

        if (std::abs(h) < 10 * std::numeric_limits<double>::epsilon() * std::max(1.0, std::abs(x))) {
            throw std::runtime_error("Step size became too small");
        } // Numerical limit. Maybe tol is too small, the ODE is too stiff or something
    }

    return std::make_tuple(xlist, ylist);
}