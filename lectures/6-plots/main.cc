#include<iostream>
#include<cmath>
#include<string>
#include<iomanip>
int main(int argc,char** argv){
	double xmin=0,xmax=10,dx=0.125;
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
	for(double x=xmin;x<=xmax;x+=dx){
		std::cout<<x<<" "<<std::erf(x)<<" "<<std::sin(x)<<"\n";
	}
	return 0;
}