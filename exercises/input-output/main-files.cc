#include <iostream>
#include <fstream>
#include <string>
#include <cmath>

int main (int argc, char *argv[]) {
	std::string infile="", outfile="";
	for(int i=0;i<argc;i++){
		std::string arg=argv[i];
		if(arg=="--input" && i+1 < argc) infile=argv[i+1];
		if(arg=="--output" && i+1 < argc) outfile=argv[i+1];
	}
std::ifstream myinput(infile);
std::ofstream myoutput(outfile);
double x;
if( myinput.is_open() && myoutput.is_open() ){
	while( myinput >> x ){
		myoutput << x <<" "<<std::sin(x)<<" "<<std::cos(x)<<std::endl;
		}
	}
else{
	std::cerr << "Error opening files: " << infile << outfile << std::endl;
	return EXIT_FAILURE;
    }
myinput.close();
myoutput.close();
exit(EXIT_SUCCESS);
}