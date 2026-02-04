#include<iostream>
#include<cstdio>
#include<math.h>
#include"sfuns.h"
int main(){
	double sqrt2=std::sqrt(2.0);
	std::cout << "sqrt(2) = " << sqrt2 <<  "\n";
	double x5th=std::pow(2.0,1.0/5.0);
	std::cout << "2^(1/5) = " << x5th <<  "\n";
	double e = std::exp(1);
	std::cout << "e^pi = " << std::pow(e,sfuns::PI) << "\n";
	std::cout << "pi^e = " << std::pow(sfuns::PI,e) << "\n\n";

	for(double x=1;x<=10;x+=1){ // Skal det være mindre end x+=1, så brug 1/(2^n) så det er eksakt repræsenteret binært - altså ikke 0.1
		std::cout 
			<< "fgamma(" << x << ")=" << sfuns::fgamma(x)
			<< "\t tgamma(" << x << ")=" << std::tgamma(x) 
			<< "\t rel diff=" << std::abs((sfuns::fgamma(x)-std::tgamma(x))/std::tgamma(x)) << "\n";
	}
	std::cout << "\n=";
	for(double x=1;x<=10;x+=1){
		std::cout 
			<< "lnfgamma(" << x << ")=" << sfuns::lnfgamma(x)
			<< "\t ln(tgamma(" << x << "))=" << std::log(std::tgamma(x))
			<< "\t rel diff=" << std::abs((sfuns::lnfgamma(x)-std::log(std::tgamma(x)))/std::log(std::tgamma(x))) << "\n";
	}
	std::cout << "\ntgamma(-1)=" << std::tgamma(-1.0) << "\n";
	return 0;
}
