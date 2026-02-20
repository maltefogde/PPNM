#include<iostream>
#include<cmath>
#include<string>
#include<iomanip>
#include<vector>
#include<numbers>
#include<limits>

double erf_approx(double x){
	// single precision error function (Abramowitz and Stegun, from Wikipedia)
	if(x<0) return -erf_approx(-x);
	std::vector<double> a {0.254829592,-0.284496736,1.421413741,-1.453152027,1.061405429};
	double t=1/(1+0.3275911*x);
	double sum=t*(a[0]+t*(a[1]+t*(a[2]+t*(a[3]+t*a[4]))));/* the right thing */
	return 1-sum*std::exp(-x*x);
} 

double sgamma(double x){
	const double PI = std::numbers::pi;
	if(x<0)return PI/std::sin(PI*x)/sgamma(1-x);
	if(x<9)return sgamma(x+1)/x;
	double lnsgamma=std::log(2*PI)/2+(x-0.5)*std::log(x)-x
		+(1.0/12)/x-(1.0/360)/(x*x*x)+(1.0/1260)/(x*x*x*x*x);
	return std::exp(lnsgamma);
}

double lngamma(double x){
	const double NaN = std::numeric_limits<double>::quiet_NaN();
    const double PI  = std::numbers::pi;
	if(x<=0) return NaN;
	if(x<9) return lngamma(x + 1) - std::log(x);
	return x*std::log(x+1/(12*x-1/x/10))-x+std::log(2*PI/x)/2;
}


int main(int argc,char** argv){
	double xmin=-4.9,xmax=4.9,dx=0.125;
	for(int i=0;i<argc;i++){
		std::string arg=argv[i];
		std::cerr<<"i= "<<i<<" arg="<<arg<<"\n";
		if(arg=="-xmin" && i+1<argc)xmin=std::stod(argv[i+1]);
		if(arg=="-xmax" && i+1<argc)xmax=std::stod(argv[i+1]);
		if(arg=="-dx" && i+1<argc)dx=std::stod(argv[i+1]);
		}
	std::cerr<<"xmin= "<<xmin<<"\n";
	std::cerr<<"xmax= "<<xmax<<"\n";
	std::cerr<<"dx= "<<dx<<"\n";
	std::cout<<std::scientific;
	for(double x=xmin; x<=xmax; x+=dx){
        std::cout
            << x << " "
            << std::erf(x) << " " << erf_approx(x) << " "
            << std::tgamma(x) << " " << sgamma(x) << " "
            << std::lgamma(x) << " " << lngamma(x)
            << "\n";
    }
	return 0;
}