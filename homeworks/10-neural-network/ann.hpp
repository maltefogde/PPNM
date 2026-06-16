#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <vector>

namespace annlib {

using vector = std::vector<double>;

struct minimize_result {
    double cost;
    double grad_norm;
    std::size_t iterations;
    bool converged;
};

inline double activation(double x) {
    return x * std::exp(-x * x);
}

inline double activation_d(double x) {
    return (1.0 - 2.0 * x * x) * std::exp(-x * x);
}

inline double activation_dd(double x) {
    return (4.0 * x * x * x - 6.0 * x) * std::exp(-x * x);
}

inline double activation_int(double x) {
    return -0.5 * std::exp(-x * x);
}

inline double norm(const vector& v) {
    double sum = 0.0;

    for(const double vi : v) {
        sum += vi * vi;
    }

    return std::sqrt(sum);
}

inline void project_widths(vector& p) {
    for(std::size_t i = 1; i < p.size(); i += 3) {
        if(std::abs(p[i]) < 0.05) {
            p[i] = (p[i] < 0.0) ? -0.05 : 0.05;
        }
    }
}

// Improved "minimize" using ChatGPT
template<class CostGradient>
minimize_result minimize(vector& p, CostGradient cost_gradient,
                         double acc = 1e-6,
                         std::size_t max_iter = 20000) {
    vector g(p.size());
    double cost = cost_gradient(p, g);
    double gnorm = norm(g);
    double step = 1e-2;

    for(std::size_t iter = 0; iter < max_iter; ++iter) {
        if(gnorm < acc) {
            return {cost, gnorm, iter, true};
        }

        bool accepted = false;
        vector trial(p.size());
        vector gt(p.size());

        for(std::size_t lines = 0; lines < 50; ++lines) {
            for(std::size_t i = 0; i < p.size(); ++i) {
                trial[i] = p[i] - step * g[i];
            }

            project_widths(trial);

            const double trial_cost = cost_gradient(trial, gt);

            if(trial_cost < cost) {
                p = trial;
                g = gt;
                cost = trial_cost;
                gnorm = norm(g);
                step *= 1.2;
                accepted = true;
                break;
            }

            step *= 0.5;
        }

        if(!accepted) {
            return {cost, gnorm, iter + 1, false};
        }
    }

    return {cost, gnorm, max_iter, gnorm < acc};
}

// Improved "network" using ChatGPT
struct network {
    int n;
    vector p;
    minimize_result last_result{0.0, 0.0, 0, false};

    explicit network(int n_) : n(n_), p(3 * static_cast<std::size_t>(n_)) {
        set_interval(-1.0, 1.0);
    }

    void set_interval(double a, double b) {
        for(int i = 0; i < n; ++i) {
            const double t = (n == 1) ? 0.0 : static_cast<double>(i) / (n - 1);

            p[3 * i + 0] = a + (b - a) * t;
            p[3 * i + 1] = (b - a) / static_cast<double>(n);
            p[3 * i + 2] = 0.1;
        }
    }

    double response(double x) const {
        double sum = 0.0;

        for(int i = 0; i < n; ++i) {
            const double a = p[3 * i + 0];
            const double b = p[3 * i + 1];
            const double w = p[3 * i + 2];
            const double z = (x - a) / b;

            sum += w * activation(z);
        }

        return sum;
    }

    double derivative(double x) const {
        double sum = 0.0;

        for(int i = 0; i < n; ++i) {
            const double a = p[3 * i + 0];
            const double b = p[3 * i + 1];
            const double w = p[3 * i + 2];
            const double z = (x - a) / b;

            sum += w * activation_d(z) / b;
        }

        return sum;
    }

    double second_derivative(double x) const {
        double sum = 0.0;

        for(int i = 0; i < n; ++i) {
            const double a = p[3 * i + 0];
            const double b = p[3 * i + 1];
            const double w = p[3 * i + 2];
            const double z = (x - a) / b;

            sum += w * activation_dd(z) / (b * b);
        }

        return sum;
    }

    double antiderivative(double x) const {
        double sum = 0.0;

        for(int i = 0; i < n; ++i) {
            const double a = p[3 * i + 0];
            const double b = p[3 * i + 1];
            const double w = p[3 * i + 2];
            const double z = (x - a) / b;

            sum += w * b * activation_int(z);
        }

        return sum;
    }

    void train(const vector& xs, const vector& ys,
               double acc = 1e-4,
               std::size_t max_iter = 30000) {
        auto cost_gradient = [this, &xs, &ys](const vector& par, vector& grad) {
            std::fill(grad.begin(), grad.end(), 0.0);

            double cost = 0.0;

            for(std::size_t k = 0; k < xs.size(); ++k) {
                double F = 0.0;

                for(int i = 0; i < n; ++i) {
                    const double a = par[3 * i + 0];
                    const double b = par[3 * i + 1];
                    const double w = par[3 * i + 2];
                    const double z = (xs[k] - a) / b;

                    F += w * activation(z);
                }

                const double e = F - ys[k];
                cost += e * e;

                for(int i = 0; i < n; ++i) {
                    const double a = par[3 * i + 0];
                    const double b = par[3 * i + 1];
                    const double w = par[3 * i + 2];
                    const double z = (xs[k] - a) / b;
                    const double f = activation(z);
                    const double fp = activation_d(z);

                    grad[3 * i + 0] += 2.0 * e * (-w * fp / b);
                    grad[3 * i + 1] += 2.0 * e * (-w * fp * z / b);
                    grad[3 * i + 2] += 2.0 * e * f;
                }
            }

            return cost;
        };

        last_result = minimize(p, cost_gradient, acc, max_iter);
    }

    template<class Phi>
    void train_de(Phi phi, double a, double b, double c,
                  double yc, double ypc,
                  double alpha = 100.0,
                  double beta = 100.0,
                  std::size_t points = 80,
                  double acc = 1e-2,
                  std::size_t max_iter = 12000) {
        auto cost_only = [this, phi, a, b, c, yc, ypc, alpha, beta, points](const vector& par) {
            const vector old = p;
            const_cast<network*>(this)->p = par;

            double cost = 0.0;
            const double dx = (b - a) / static_cast<double>(points);

            for(std::size_t k = 0; k < points; ++k) {
                const double x = a + (k + 0.5) * dx;
                const double r = phi(second_derivative(x), derivative(x), response(x), x);

                cost += r * r * dx;
            }

            const double bc0 = response(c) - yc;
            const double bc1 = derivative(c) - ypc;

            cost += alpha * bc0 * bc0 + beta * bc1 * bc1;

            const_cast<network*>(this)->p = old;

            return cost;
        };

        auto cost_gradient = [&cost_only](const vector& par, vector& grad) {
            const double cost = cost_only(par);

            for(std::size_t i = 0; i < par.size(); ++i) {
                vector trial = par;
                const double dx = std::max(1e-6, std::abs(par[i]) * 1e-6);

                trial[i] += dx;

                grad[i] = (cost_only(trial) - cost) / dx;
            }

            return cost;
        };

        last_result = minimize(p, cost_gradient, acc, max_iter);
    }
};

} // namespace annlib