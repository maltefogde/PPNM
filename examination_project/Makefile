CXX = clang++
CXXFLAGS = -std=c++23 -O2 -Wall -Wextra -Werror

.PHONY: all run clean

all: Out.txt hydrogen_convergence.svg hydrogen_error.svg

main: main.cc lanczos.hpp ../homeworks/lib/matrix.h
	$(CXX) $(CXXFLAGS) main.cc -o main

Out.txt: main
	./main > Out.txt

hydrogen_convergence.dat: main
	./main > Out.txt

hydrogen_convergence.svg: hydrogen_convergence.dat
	gnuplot -e "set terminal svg background 'white'; \
	set output 'hydrogen_convergence.svg'; \
	set title 'Lanczos convergence for hydrogen ground state'; \
	set xlabel 'Lanczos dimension n'; \
	set ylabel 'E_0(T_n)'; \
	set yrange [-0.504:-0.38]; \
	set xrange [0:50]; \
	set ytics 0.02; \
	set grid; \
	set border lw 1.5; \
	set key top right spacing 1.2; \
	plot 'hydrogen_convergence.dat' using 1:2 with linespoints lw 2 pt 7 ps 0.7 title 'Lanczos', \
	     -0.5 with lines lw 2 dashtype 1 title 'Exact continuum -0.5', \
	     -0.4987562112091555 with lines lw 2 dashtype 2 title 'E_0(H), full matrix'"

hydrogen_error.svg: hydrogen_convergence.dat
	gnuplot -e "set terminal svg background 'white'; \
	set output 'hydrogen_error.svg'; \
	set title 'Lanczos error for hydrogen ground state'; \
	set xlabel 'Lanczos dimension n'; \
	set ylabel '|E_0(T_n)-E_0(H)|'; \
	set logscale y; \
	set xrange [0:102]; \
	set format y '10^{%L}'; \
	set mytics 10; \
	set grid; \
	set border lw 1.5; \
	set key top right; \
	plot 'hydrogen_convergence.dat' using 1:3 with linespoints lw 2 pt 7 ps 0.7 title 'Lanczos error'"

run: main
	./main > Out.txt
	cat Out.txt

clean:
	rm -f main Out.txt *.dat *.svg