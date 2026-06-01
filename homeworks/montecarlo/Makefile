CXX = clang++
CXXFLAGS = -std=c++23 -O2 -Wall -Wextra -Werror

.PHONY: all run clean

all: Out.txt A_circle_errors.svg B_scaling.svg

main: main.cc mc.hpp
	$(CXX) $(CXXFLAGS) main.cc -o main

Out.txt A_circle_errors.dat B_scaling.dat: main
	./main -taskA > Out.txt
	./main -taskB >> Out.txt
	./main -taskC >> Out.txt

A_circle_errors.svg: A_circle_errors.dat
	echo '\
		set terminal svg size 1000,700 enhanced background rgb "white"; \
		set output "A_circle_errors.svg"; \
		set grid; \
		set logscale xy; \
		set xlabel "N"; \
		set ylabel "error"; \
		set title "Plain Monte Carlo error for unit circle"; \
		set key bottom left; \
		plot \
			"A_circle_errors.dat" using 1:3 with linespoints pointtype 7 title "estimated error", \
			"A_circle_errors.dat" using 1:4 with linespoints pointtype 7 title "actual error", \
			"A_circle_errors.dat" using 1:5 with lines dashtype 2 title "1/sqrt(N) reference"; \
	' | gnuplot

B_scaling.svg: B_scaling.dat
	echo '\
		set terminal svg size 1000,700 enhanced background rgb "white"; \
		set output "B_scaling.svg"; \
		set grid; \
		set logscale xy; \
		set xlabel "N"; \
		set ylabel "actual error"; \
		set title "Pseudo-random vs quasi-random Monte Carlo"; \
		set key bottom left; \
		plot \
			"B_scaling.dat" using 1:2 with linespoints pointtype 7 title "LCG", \
			"B_scaling.dat" using 1:3 with linespoints pointtype 7 title "std::mt19937", \
			"B_scaling.dat" using 1:4 with linespoints pointtype 7 title "Halton"; \
	' | gnuplot

run: main
	./main -taskA > Out.txt
	./main -taskB >> Out.txt
	./main -taskC >> Out.txt

clean:
	rm -f main Out.txt *.dat *.svg