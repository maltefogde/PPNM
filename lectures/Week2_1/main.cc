#include<iostream>
#include<cstdio>
#include<math.h>
#include"hello.h"
#include"sfuns.h"
int main(){
	hello();
	double x=1;
	double y = sfuns::fgamma(x);
	std::cout << "fgamma(1)=" << y << "\n";
	std::printf("fgamma(1)=%g\n",y);
	for(double x=1;x<=9;x+=1){ // Skal det være mindre end x+=1, så brug 1/(2^n) så det er eksakt repræsenteret binært - altså ikke 0.1
		std::cout << "fgamma(" << x << ")=" << sfuns::fgamma(x)
			<< "\t tgamma(" << x << ")=" << std::tgamma(x) << "\n";
	}
	std::cout << "tgamma(-1)=" << std::tgamma(-1.0) << "\n";
	return 0;
}
