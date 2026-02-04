#include<cmath>
#include<numbers> // c++23
#include<limits>
#include"sfuns.h"
namespace sfuns{
constexpr double PI = std::numbers::pi; // c++23
// constexpr double PI = 3.14159265358979323846; // pre- c++23
constexpr double NaN = std::numeric_limits<double>::quiet_NaN();

inline double stirling_lnfgamma(double x) {
    return x*std::log(x+1/(12*x-1/x/10))-x+std::log(2*PI/x)/2;
}

double fgamma(double x){
	if(x <= 0) return PI/std::sin(PI*x)/fgamma(1-x);
	if(x < 9)return fgamma(x+1)/x;
	return std::exp(stirling_lnfgamma(x));
	}

// The gamma-function overflows very easily, so the logarithm of the gamma function, lngamma, is often a more useful function. Figure out how to modify the above formula to calculate lngamma. For simplicity you should only allow positive arguments for your lngamma:
double lnfgamma(double x){
	if(x <= 0) return NaN;
	if(x < 9) return lnfgamma(x+1) - std::log(x);
	return stirling_lnfgamma(x);
}
}
