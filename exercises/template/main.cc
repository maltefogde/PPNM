#include <iostream>
#include <cmath>
#include <complex>
#include <utility>
#include <string>
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
    // -------- double --------
    {
        vec<double> u(1,2,3), v(4,5,6);
        test( approx(u+v, vec<double>(5,7,9)), "double: u+v" );
        test( approx(u-v, vec<double>(-3,-3,-3)), "double: u-v" );
        test( approx_double(u.dot(v), 32.0), "double: dot(u,v)=32" );
        test( approx(u.cross(v), vec<double>(-3,6,-3)), "double: cross(u,v)" );
        test( approx_double(vec<double>(3,4,0).norm(), 5.0), "double: norm(3,4,0)=5" );
    }

    // -------- float --------
    {
        vec<float> a(1.f,2.f,3.f), b(0.5f,0.5f,0.5f);
        auto c = a + b;
        test( approx(c, vec<float>(1.5f,2.5f,3.5f), 1e-6f, 1e-6f), "float: add" );
        test( approx_double(double(a.dot(a)), 14.0), "float: dot(a,a)=14 (cast)" );
        test( approx_double(double(a.norm()), std::sqrt(14.0)), "float: norm" );
    }

    // -------- int --------
    {
        vec<int> p(1,2,3), q(4,5,6);
        auto s = p + q;
        test( s.x==5 && s.y==7 && s.z==9, "int: p+q exact" );
        test( p.dot(q) == 32, "int: dot exact" );

        auto r = p.cross(q);
        test( r.x==-3 && r.y==6 && r.z==-3, "int: cross exact" );

        // norm() should be sqrt(14) now (because we fixed norm to return floating)
        test( approx_double(double(p.norm()), std::sqrt(14.0)), "int: norm sqrt(14)" );
    }

    // -------- complex<double> --------
    {
        using cd = std::complex<double>;
        vec<cd> p(cd(1,2), cd(0,-1), cd(3,0));
        vec<cd> q(cd(1,2), cd(0,-1), cd(3,0));

        test( approx(p, q), "complex: approx(p,q)" );
        test( approx_double(double(p.norm()), double(q.norm())), "complex: norm matches" );

        cd dp = p.dot(p); // Hermitian => imag should be 0
        test( approx_double(std::imag(dp), 0.0), "complex: imag(p·p)=0" );
    }

    std::cout << "\nDone.\n";
    return 0;
}
