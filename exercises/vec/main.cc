#include <iostream>
#include <cmath>
#include "vec.h"

static bool approx_double(double a, double b, double acc=1e-9, double eps=1e-9){
    if(std::fabs(a-b) < acc) return true;
    if(std::fabs(a-b) < eps*(std::fabs(a)+std::fabs(b))) return true;
    return false;
}

static void test(bool ok, const std::string& name){
    std::cout << (ok ? "[PASS] " : "[FAIL] ") << name << "\n";
}

int main(){
    // --- constructors + print + operator<< ---
    vec a;                 // default
    vec b(1,2,3);           // parameterized
    vec c = b;              // copy
    vec d = std::move(b);   // move

    a.print("a = ");
    c.print("c = ");
    d.print("d = ");
    std::cout << "ostream c = " << c << "\n\n";

    // --- assignment ---
    vec e; e = c;                 // copy assignment
    vec f; f = std::move(c);      // move assignment
    e.print("e = ");
    f.print("f = ");
    std::cout << "\n";

    // --- arithmetic operators ---
    vec u(1,2,3), v(4,5,6);

    std::cout << "u = " << u << "\n";
    std::cout << "v = " << v << "\n";
    std::cout << "u+v = " << (u+v) << "\n";
    std::cout << "u-v = " << (u-v) << "\n";
    std::cout << "-u  = " << (-u)  << "\n";
    std::cout << "2*u = " << (2.0*u) << "\n";
    std::cout << "u*2 = " << (u*2.0) << "\n";
    std::cout << "u/2 = " << (u/2.0) << "\n\n";

    // --- compound operators ---
    vec t = u; t += v; test( approx(t, u+v), "t=u; t+=v equals u+v");
    t = u; t -= v;         test( approx(t, u-v), "t=u; t-=v equals u-v");
    t = u; t *= 2.0;       test( approx(t, u*2.0), "t=u; t*=2 equals u*2");
    t = u; t /= 2.0;       test( approx(t, u/2.0), "t=u; t/=2 equals u/2");
    std::cout << "\n";

    // --- vector algebra tests ---
    vec ex(1,0,0), ey(0,1,0), ez(0,0,1);

    // dot
    test( approx_double(ex.dot(ey), 0.0), "dot: ex·ey = 0");
    test( approx_double(ex.dot(ex), 1.0), "dot: ex·ex = 1");
    test( approx_double(u.dot(v), 1*4 + 2*5 + 3*6), "dot: u·v matches component formula");

    // cross (right-hand rule)
    test( approx(ex.cross(ey), ez), "cross: ex×ey = ez");
    test( approx(ey.cross(ex), -ez), "cross: ey×ex = -ez");
    test( approx(ex.cross(ex), vec(0,0,0)), "cross: ex×ex = 0");

    // orthogonality: u·(u×v)=0 and v·(u×v)=0
    vec uxv = u.cross(v);
    test( approx_double(u.dot(uxv), 0.0), "orthogonality: u·(u×v)=0");
    test( approx_double(v.dot(uxv), 0.0), "orthogonality: v·(u×v)=0");

    // norm
    test( approx_double(ex.norm(), 1.0), "norm: |ex| = 1");
    test( approx_double(vec(3,4,0).norm(), 5.0), "norm: |(3,4,0)| = 5");
    test( approx_double(u.norm(), std::sqrt(u.dot(u))), "norm: |u| = sqrt(u·u)");

    std::cout << "\nDone.\n";
    return 0;
}
