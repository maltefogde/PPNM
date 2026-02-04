#include <iostream>
#include <cmath>
#include "vec.h"

// ===== member operators =====

vec& vec::operator+=(const vec& other){
    x += other.x;
    y += other.y;
    z += other.z;
    return *this;
}

vec& vec::operator-=(const vec& other){
    x -= other.x;
    y -= other.y;
    z -= other.z;
    return *this;
}

vec& vec::operator*=(double n){
    x *= n;
    y *= n;
    z *= n;
    return *this;
}

vec& vec::operator/=(double n){
    x /= n;
    y /= n;
    z /= n;
    return *this;
}

// ===== non-member operators =====

vec operator+(const vec& a, const vec& b){
    vec r = a;
    r += b;
    return r;
}

vec operator-(const vec& a){
    return vec(-a.x, -a.y, -a.z);
}

vec operator-(const vec& a, const vec& b){
    vec r = a;
    r -= b;
    return r;
}

vec operator*(const vec& a, double n){
    vec r = a;
    r *= n;
    return r;
}

vec operator*(double n, const vec& a){
    return a * n;
}

vec operator/(const vec& a, double n){
    vec r = a;
    r /= n;
    return r;
}

// ===== approx =====

static bool approx(double a, double b, double acc, double eps){
    if (std::fabs(a - b) < acc) return true;
    if (std::fabs(a - b) < eps * (std::fabs(a) + std::fabs(b))) return true;
    return false;
}

bool approx(const vec& a, const vec& b, double acc, double eps){
    if (!approx(a.x, b.x, acc, eps)) return false;
    if (!approx(a.y, b.y, acc, eps)) return false;
    if (!approx(a.z, b.z, acc, eps)) return false;
    return true;
}



// ===== stream output =====

std::ostream& operator<<(std::ostream& os, const vec& v){
    return os << "{ " << v.x << ", " << v.y << ", " << v.z << " }";
}

// ===== utility =====

void vec::print(const std::string& s) const {
    std::cout << s << x << " " << y << " " << z << std::endl;
}

// ===== vector algebra =====

double vec::dot(const vec& other) const {
    return x*other.x + y*other.y + z*other.z;
}

vec vec::cross(const vec& other) const {
    return vec(
        y*other.z - z*other.y,
        z*other.x - x*other.z,
        x*other.y - y*other.x
    );
}

double vec::norm() const {
    return std::sqrt(x*x + y*y + z*z);
}
