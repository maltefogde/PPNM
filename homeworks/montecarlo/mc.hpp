#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <random>
#include <stdexcept>
#include <vector>

namespace mc {

struct result {
    double value;
    double error;
};

class lcg {
private:
    std::uint64_t seed;
    std::uint64_t a;
    std::uint64_t c;
    std::uint64_t m;

public:
    lcg(
        std::uint64_t seed_,
        std::uint64_t a_ = 1664525,
        std::uint64_t c_ = 1013904223,
        std::uint64_t m_ = 4294967296 // 2^32
    )
        : seed(seed_), a(a_), c(c_), m(m_) {}

    double operator()() {
        seed = (a * seed + c) % m;
        return static_cast<double>(seed + 1) / static_cast<double>(m + 1); // static_cast converts to doubles to such that we do not obtain 0
    }
};


class std_rng {
private:
    std::mt19937 gen;
    std::uniform_real_distribution<double> dist;

public:
    explicit std_rng(unsigned int seed = 1) : gen(seed), dist(0.0, 1.0) {}

    double operator()() {
        return dist(gen);
    }
};

template<class F, class RNG>
result plainmc(F f, const std::vector<double>& a, const std::vector<double>& b,
               std::size_t N, RNG& rng) {
    if(a.size() != b.size()) {
        throw std::invalid_argument("plainmc: a and b must have same size");
    }

    const std::size_t dim = a.size();

    double V = 1.0;
    for(std::size_t i = 0; i < dim; ++i) {
        V *= b[i] - a[i]; // Side length * Side length * ...
    }

    double sum = 0.0;
    double sum2 = 0.0;
    std::vector<double> x(dim);

    for(std::size_t i = 0; i < N; ++i) {
        for(std::size_t k = 0; k < dim; ++k) {
            x[k] = a[k] + rng() * (b[k] - a[k]);
        } // A random point inside the integration volume between a[k] and b[k]

        const double fx = f(x);
        sum += fx;
        sum2 += fx * fx;
    }

    const double mean = sum / static_cast<double>(N);
    double variance = sum2 / static_cast<double>(N) - mean * mean;
    variance = std::max(variance, 0.0);

    return {
        mean * V, // result of integration
        std::abs(V) * std::sqrt(variance / static_cast<double>(N)) // statistical estimate of the error, not strict bound
    };
}

inline std::vector<int> prime_numbers(std::size_t n) {
    std::vector<int> primes;
    int candidate = 2;
    while(primes.size() < n) {
        bool is_prime = true;
        for(const int p : primes) {
            if(p * p > candidate) {
                break;
            }
            if(candidate % p == 0) {
                is_prime = false;
                break;
            }
        }
        if(is_prime) {
            primes.push_back(candidate);
        }
        ++candidate;
    }
    return primes;
}

inline double corput(std::size_t n, int base) {
    double q = 0.0;
    double bk = 1.0 / static_cast<double>(base);

    while(n > 0) {
        q += static_cast<double>(n % static_cast<std::size_t>(base)) * bk;
        n /= static_cast<std::size_t>(base);
        bk /= static_cast<double>(base);
    }
    // Explained in the book

    return q;
}

class halton {
private:
    std::vector<int> bases;

public:
    explicit halton(std::size_t dim, std::size_t skip = 0) {
        const std::vector<int> all_primes = prime_numbers(dim + skip);

        for(std::size_t i = skip; i < all_primes.size(); ++i) {
            bases.push_back(all_primes[i]);
        }
    }

    std::vector<double> point(std::size_t n) const {
        std::vector<double> x(bases.size());

        for(std::size_t k = 0; k < bases.size(); ++k) {
            x[k] = corput(n, bases[k]);
        }

        return x;
    }
};

template<class F>
result quasimc(F f, const std::vector<double>& a, const std::vector<double>& b,
               std::size_t N) {
    if(a.size() != b.size()) {
        throw std::invalid_argument("quasimc: a and b must have same size");
    }
    const std::size_t dim = a.size();

    double V = 1.0;
    for(std::size_t k = 0; k < dim; ++k) {
        V *= b[k] - a[k];
    }

    halton h1(dim, 0); // bases 2, 3, 5, ...
    halton h2(dim, 1); // bases 3, 5, 7, ... used for error estimate: "The error could be estimated by using two different sequences"

    double sum1 = 0.0;
    double sum2 = 0.0;
    std::vector<double> x(dim);

    for(std::size_t i = 0; i < N; ++i) {
        std::vector<double> u = h1.point(i + 1);

        for(std::size_t k = 0; k < dim; ++k) {
            x[k] = a[k] + u[k] * (b[k] - a[k]);
        }

        sum1 += f(x);

        u = h2.point(i + 1);

        for(std::size_t k = 0; k < dim; ++k) {
            x[k] = a[k] + u[k] * (b[k] - a[k]);
        }

        sum2 += f(x);
    }

    const double integral1 = V * sum1 / static_cast<double>(N);
    const double integral2 = V * sum2 / static_cast<double>(N);

    return {
        0.5 * (integral1 + integral2),
        std::abs(integral1 - integral2)
    };
}

template<class F, class RNG>
result stratified(F f, const std::vector<double>& a, const std::vector<double>& b,
                  std::size_t N, RNG& rng, std::size_t nmin = 64) {
    if(a.size() != b.size()) {
        throw std::invalid_argument("stratified: a and b must have same size");
    }

    const std::size_t dim = a.size();

    if(N < 2 * nmin) { // If N<nmin return N-point plain Monte Carlo estimate of integral and variance;
        return plainmc(f, a, b, N, rng);
    }

    // Sample nmin points and estimate the integral and the variance;
    std::vector<double> mid(dim);

    for(std::size_t k = 0; k < dim; ++k) {
        mid[k] = 0.5 * (a[k] + b[k]);
    }

    std::vector<std::size_t> nleft(dim, 0);
    std::vector<std::size_t> nright(dim, 0);

    std::vector<double> sumleft(dim, 0.0);
    std::vector<double> sumright(dim, 0.0);
    std::vector<double> sum2left(dim, 0.0);
    std::vector<double> sum2right(dim, 0.0);

    std::vector<double> x(dim);

    for(std::size_t i = 0; i < nmin; ++i) {
        for(std::size_t k = 0; k < dim; ++k) {
            x[k] = a[k] + rng() * (b[k] - a[k]);
        }

        const double fx = f(x);

        for(std::size_t k = 0; k < dim; ++k) {
            if(x[k] < mid[k]) {
                ++nleft[k];
                sumleft[k] += fx;
                sum2left[k] += fx * fx;
            } else {
                ++nright[k];
                sumright[k] += fx;
                sum2right[k] += fx * fx;
            }
        }
    }

    std::size_t kdiv = 0;
    double best_score = -1.0;

    for(std::size_t k = 0; k < dim; ++k) {
        const double meanleft = nleft[k] > 0 ? sumleft[k] / static_cast<double>(nleft[k]) : 0.0;
        const double meanright = nright[k] > 0 ? sumright[k] / static_cast<double>(nright[k]) : 0.0;

        // Split along the dimension where the two half-means differ most.
        const double score = std::abs(meanleft - meanright);

        if(score > best_score) {
            best_score = score;
            kdiv = k;
        }
    }

    const auto variance = [](std::size_t n, double sum, double sum2) {
        if(n == 0) {
            return 0.0;
        }

        const double mean = sum / static_cast<double>(n);
        return std::max(sum2 / static_cast<double>(n) - mean * mean, 0.0);
    };

    // Find the dimension with largest sub-variance;
    const double varleft = variance(nleft[kdiv], sumleft[kdiv], sum2left[kdiv]);
    const double varright = variance(nright[kdiv], sumright[kdiv], sum2right[kdiv]);

    const std::size_t remaining = N - nmin;

    double sigleft = std::sqrt(varleft);
    double sigright = std::sqrt(varright);

    // Divide the remaning points between the two sub-volumes proportional to sub-variances;
    std::size_t Nleft = remaining / 2;

    if(sigleft + sigright > 0.0) { // More points are sent to the half with the larger estimated standard deviation.
        Nleft = static_cast<std::size_t>(
            remaining * sigleft / (sigleft + sigright)
        );
    }

    Nleft = std::max<std::size_t>(1, std::min(Nleft, remaining - 1));
    const std::size_t Nright = remaining - Nleft;

    std::vector<double> bleft = b;
    std::vector<double> aright = a;

    bleft[kdiv] = mid[kdiv];
    aright[kdiv] = mid[kdiv];

    // Dispatch two recursive calls on the sub-volumes; Estimate the grand integral and grand error;
    const result left = stratified(f, a, bleft, Nleft, rng, nmin);
    const result right = stratified(f, aright, b, Nright, rng, nmin);

    // Return the grand integral and the grand error;
    return {
        left.value + right.value,
        std::sqrt(left.error * left.error + right.error * right.error)
    };
}

} // namespace mc
