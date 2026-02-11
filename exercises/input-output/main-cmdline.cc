#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <cstdlib>


int main (int argc, char* argv[]) {
	std::vector<double> numbers;
	for(int i=0;i<argc;++i){
		std::string arg=argv[i];
		if(arg=="-n" && i+1<argc)
			numbers.push_back(std::stod(argv[i+1]));
	}
for(auto n: numbers)
	std::cout << n <<" "<< std::sin(n) <<" "<< std::cos(n) <<std::endl;
exit(EXIT_SUCCESS);
}