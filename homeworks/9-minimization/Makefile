CXX = clang++
CXXFLAGS = -std=c++23 -O2 -Wall -Wextra -Werror

.PHONY: all run clean

all: Out.txt higgs_fit.svg

main: main.cc minimize.hpp
	$(CXX) $(CXXFLAGS) main.cc -o main

Out.txt: main
	./main -taskA > Out.txt
	./main -taskB >> Out.txt
	./main -taskC >> Out.txt

higgs_fit.svg: Out.txt
	echo '\
		set terminal svg background rgb "white"; \
		set output "higgs_fit.svg"; \
		set grid; \
		set xlabel "Energy [GeV]"; \
		set ylabel "Signal"; \
		set title "Higgs signal fitted with Breit-Wigner function"; \
		set key top right; \
		plot \
			"higgs_data.dat" using 1:2:3 with yerrorbars pointtype 7 title "Data", \
			"higgs_fit.dat" using 1:2 with lines title "Fit"; \
	' | gnuplot

run: Out.txt
	cat Out.txt

clean:
	rm -f main Out.txt *.dat *.svg log