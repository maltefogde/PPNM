#pragma once
#include <iostream>
#include <string>
#include <cmath>
#include <complex>
#include <type_traits>

// real_type_t<T>: T for real, R for complex<R>
template<class T> struct real_type { using type = T; };
template<class R> struct real_type<std::complex<R>> { using type = R; };
template<class T> using real_type_t = typename real_type<T>::type;

template<typename T>
struct vec {
    T x, y, z;

    vec(T x, T y, T z) : x(x), y(y), z(z) {}
    vec() : vec(T{}, T{}, T{}) {}
    vec(const vec&) = default;
    vec(vec&&) = default;
    ~vec() = default;

    vec& operator=(const vec&) = default;
    vec& operator=(vec&&) = default;

    vec& operator+=(const vec& o){ x+=o.x; y+=o.y; z+=o.z; return *this; }
    vec& operator-=(const vec& o){ x-=o.x; y-=o.y; z-=o.z; return *this; }
    vec& operator*=(T k){ x*=k; y*=k; z*=k; return *this; }
    vec& operator/=(T k){ x/=k; y/=k; z/=k; return *this; }

    void set(T a, T b, T c){ x=a; y=b; z=c; }
    void print(const std::string& s="") const { std::cout << s << x << " " << y << " " << z << "\n"; }

    // dot: Hermitian for complex, ordinary for real
    T dot(const vec& o) const {
        using R = real_type_t<T>;
        if constexpr (std::is_same_v<T, std::complex<R>>) {
            using std::conj;
            return conj(x)*o.x + conj(y)*o.y + conj(z)*o.z;
        } else {
            return x*o.x + y*o.y + z*o.z;
        }
    }

    vec cross(const vec& o) const {
        return vec(
            y*o.z - z*o.y,
            z*o.x - x*o.z,
            x*o.y - y*o.x
        );
    }

    // norm: return a floating type (works nicely for int/float/double/complex)
    auto norm() const {
        using R0 = real_type_t<T>;
        using R  = std::common_type_t<R0, double>;

        using std::sqrt;
        if constexpr (std::is_same_v<T, std::complex<R0>>) {
            using std::norm; // |z|^2
            return sqrt(R(norm(x) + norm(y) + norm(z)));
        } else {
            return sqrt(R(x)*R(x) + R(y)*R(y) + R(z)*R(z));
        }
    }

};

// stream output (requires only that T is streamable)
template<typename T>
std::ostream& operator<<(std::ostream& os, const vec<T>& v){
    return os << "{ " << v.x << ", " << v.y << ", " << v.z << " }";
}

// free operators
template<typename T> vec<T> operator+(vec<T> a, const vec<T>& b){ a+=b; return a; }
template<typename T> vec<T> operator-(const vec<T>& v){ return vec<T>(-v.x, -v.y, -v.z); }
template<typename T> vec<T> operator-(vec<T> a, const vec<T>& b){ a-=b; return a; }
template<typename T> vec<T> operator*(vec<T> v, T k){ v*=k; return v; }
template<typename T> vec<T> operator*(T k, vec<T> v){ v*=k; return v; }
template<typename T> vec<T> operator/(vec<T> v, T k){ v/=k; return v; }

// approx (works for real + complex via std::abs)
template<typename T>
inline bool approx_scalar(T a, T b, real_type_t<T> acc, real_type_t<T> eps){
    using std::abs;
    auto d = abs(a - b);
    if (d < acc) return true;
    if (d < eps * (abs(a) + abs(b))) return true;
    return false;
}

template<typename T>
bool approx(const vec<T>& a, const vec<T>& b,
            real_type_t<T> acc = real_type_t<T>(1e-6),
            real_type_t<T> eps = real_type_t<T>(1e-6)){
    if (!approx_scalar(a.x, b.x, acc, eps)) return false;
    if (!approx_scalar(a.y, b.y, acc, eps)) return false;
    if (!approx_scalar(a.z, b.z, acc, eps)) return false;
    return true;
}
