#include<thread>
#include<string>
#include<iostream>
#include<vector>
#include<functional>
#include <algorithm>

struct data {int a, b; double sum;} ;

void harm(data& arg){
	int a=arg.a,b=arg.b;
	double sum=0;
	for(int i=a;i<b;i++)sum+=1.0/i;
	arg.sum=sum;
}

int main(int argc,char** argv){
	int nterms=(int)1e9,nthreads=1;
	for(int i=1;i<argc;i++){ // starting at i=1 to skip ./main
		std::string arg=argv[i];
		if(arg=="-terms" && i+1<argc)nterms=(int)std::stod(argv[++i]); // se om det i. element i argumentet er "-terms" og lad det efterfølgende argument være "nterms"
		if(arg=="-threads" && i+1<argc)nthreads=std::stoi(argv[++i]);
	}

	if(nterms < 1) { std::cerr << "nterms must be >= 1\n"; return 1; }
	nthreads = std::max(1, nthreads);
	nthreads = std::min(nthreads, nterms);

	std::vector<data> params(nthreads);
	for(int i=0;i<nthreads;++i){
		params[i].sum = 0;
		params[i].a = 1 + (nterms/nthreads)*i; // For nterms=100 and threads=4, this is 1, 26, 51, 76
		params[i].b = 1 + (nterms/nthreads)*(i+1); // 25, 50, 75, 100
	}
	params.back().b = nterms + 1;   // fix remainder before starting threads

	std::vector<std::thread> threads; // threads = [ thread1, thread2, thread3, ... ]
	threads.reserve(nthreads); // If not, the vector needs to reallokate multiple times when new threads are added
	for(int i=0;i<nthreads;++i){
		threads.emplace_back(harm, std::ref(params[i]));
	}

	for(auto &thread : threads)thread.join(); // for each thread in threads
	double total=0;
	for(auto &d : params)total+=d.sum; // Collecting results from each thread

	std::cout << "terms: " << nterms << "\n";
    std::cout << "threads: " << nthreads << "\n";
    std::cout << "total sum= " << (double)total << "\n";
}