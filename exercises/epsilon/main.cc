#include <iostream>
#include <cstdio>
#include <limits>
#include <iomanip>
#include"approx.h"

int main() {
	// Machine epsilon
    float f = 1.0f;
    while (1.0f + f != 1.0f) f /= 2.0f;
    f *= 2.0f;

    double d = 1.0;
    while (1.0 + d != 1.0) d /= 2.0;
    d *= 2.0;

    long double l = 1.0L;
    while (1.0L + l != 1.0L) l /= 2.0L;
    l *= 2.0L;

	std::cout << std::setprecision(30);
	std::printf(		 "Machine epsilon results:\n");
    std::printf(         "        float eps (loop) =   %.17g\n",  f);
    std::cout <<         "        float eps (limits) = " << std::numeric_limits<float>::epsilon() << "\n";
	std::cout <<         "  Exp. (f): 2.0^{-23} =      " << std::pow(2.0,-23) << "\n";
	std::printf(		 "\n");

    std::printf(         "       double eps (loop) =   %.30g\n", d);
	std::cout <<         "       double eps (limits) = " << std::numeric_limits<double>::epsilon() << "\n";
	std::cout <<         "  Exp. (d): 2.0^{-52} =      " << std::pow(2.0,-52) << "\n";
	std::printf(		 "\n");

    std::printf(         "  long double eps (loop) =   %.30Lg\n", l);
    std::cout <<         "  long double eps (limits) = " << std::numeric_limits<long double>::epsilon() << "\n";
	
	// Non-commutativity of addition
	double epsilon=std::pow(2,-52);
	double tiny=epsilon/2;
	double a=1+tiny+tiny;
	double b=tiny+tiny+1;
	std::printf("\nNon-commutativity of addition:\n");
	std::cout << "  a==b ? " << (a==b ? "true":"false") << "\n";
	std::cout << "  a>1  ? " << (a>1  ? "true":"false") << "\n";
	std::cout << "  b>1  ? " << (b>1  ? "true":"false") << "\n";

	std::printf("\nExplanation:\n");
	std::printf("  The addition is not commutative in floating point arithmetic because of rounding errors.\n");
	std::printf("  When computing a = 1 + tiny + tiny, the first addition (1 + tiny) is exact,\n");
	std::printf("  but the second addition (result + tiny) may be inexact due to rounding.\n");
	std::printf("  In contrast, b = tiny + tiny + 1 computes the sum in a different order,\n");
	std::printf("  which may yield a different result due to the same rounding issues.\n");

	std::printf("\n");
	std::cout << std::fixed << std::setprecision(17);
	std::cout << "         tiny=" << tiny << "\n";
	std::cout << "  1+tiny+tiny=" << a << "\n";
	std::cout << "  tiny+tiny+1=" << b << "\n";

	// Comparing doubles: introduction
	std::printf("\nComparing doubles:\n");
	double d1 = 0.1+0.1+0.1+0.1+0.1+0.1+0.1+0.1;
	double d2 = 8*0.1;
	std::cout << "  d1==d2? " << (d1==d2 ? "true":"false") << "\n"; 

	std::cout << std::fixed << std::setprecision(17);
	std::cout << "  d1=" << d1 << "\n";
	std::cout << "  d2=" << d2 << "\n";

	// Comparing doubles: the task
	std::printf("\nComparing doubles: the task\n");
	std::cout << "  Is d1 approximately equal to d2? " << (approx(d1,d2) ? "true":"false") << "\n";
	return 0;
}
