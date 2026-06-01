#pragma once

#include <vector>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <initializer_list>

struct vec {
    std::vector<double> data;

    vec() = default;

    explicit vec(std::size_t n) : data(n) {}

    vec(std::initializer_list<double> values) : data(values) {}

    std::size_t size() const {
        return data.size();
    }

    double& operator[](std::size_t i) {
        return data[i];
    }

    const double& operator[](std::size_t i) const {
        return data[i];
    }

    vec& operator+=(const vec& other) {
        check_size(other);
        for (std::size_t i = 0; i < size(); ++i) {
            data[i] += other[i];
        }
        return *this;
    }

    vec& operator-=(const vec& other) {
        check_size(other);
        for (std::size_t i = 0; i < size(); ++i) {
            data[i] -= other[i];
        }
        return *this;
    }

    vec& operator*=(double c) {
        for (double& x : data) {
            x *= c;
        }
        return *this;
    }

    vec& operator/=(double c) {
        for (double& x : data) {
            x /= c;
        }
        return *this;
    }

    double norm() const {
        double sum = 0.0;
        for (double x : data) {
            sum += x * x;
        }
        return std::sqrt(sum);
    }

private:
    void check_size(const vec& other) const {
        if (size() != other.size()) {
            throw std::runtime_error("vec size mismatch");
        }
    }
};


// ---------- non-member operators ----------

inline vec operator+(vec a, const vec& b) {
    a += b;
    return a;
}

inline vec operator-(vec a, const vec& b) {
    a -= b;
    return a;
}

inline vec operator-(vec a) {
    for (std::size_t i = 0; i < a.size(); ++i) {
        a[i] = -a[i];
    }
    return a;
}

inline vec operator*(vec a, double c) {
    a *= c;
    return a;
}

inline vec operator*(double c, vec a) {
    a *= c;
    return a;
}

inline vec operator/(vec a, double c) {
    a /= c;
    return a;
}

inline std::ostream& operator<<(std::ostream& os, const vec& v) {
    os << "{ ";
    for (std::size_t i = 0; i < v.size(); ++i) {
        os << v[i];
        if (i + 1 < v.size()) os << ", ";
    }
    os << " }";
    return os;
}