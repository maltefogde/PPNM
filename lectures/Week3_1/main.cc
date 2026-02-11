#include<iostream>
#include<fstream>
#include<vector>
#include<cmath>
int main(){
	std::vector<double> x,y;
	double number1,number2;
	std::cout << "standard out stream\n";
	std::cerr << "standard error stream\n";
	while( std::cin >> number1 >> number2 ){
		x.push_back(number1);
		y.push_back(number2);
	}
	std::cout << "#  x, y, atan(x,y)\n";
	for(size_t i=0;i<x.size();i++){
		double xi=x[i];
		double yi=y[i];
		std::cout << xi <<" "<<yi<<" "<<std::atan2(xi,yi)<<"\n";
	}
	std::ifstream myinput("data.txt");
	std::ofstream myoutput("out.txt");
	while( myinput >> number1 >> number2 ){
		myoutput << number1 << " " << number2 << "\n";
	}
}
