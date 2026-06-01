CXX = clang++
CXXFLAGS = -std=c++23 -O2 -Wall -Wextra -Werror

.PHONY: all run clean

all: Out.txt B_hydrogen_wave.svg B_convergence_rmax.svg B_convergence_rmin.svg B_convergence_acc.svg B_convergence_eps.svg

main: main.cc roots.hpp
	$(CXX) $(CXXFLAGS) main.cc -o main

Out.txt B_hydrogen_wave.dat B_convergence_rmax.dat B_convergence_rmin.dat B_convergence_acc.dat B_convergence_eps.dat: main
	./main -taskA > Out.txt
	./main -taskB >> Out.txt
	./main -taskC >> Out.txt

B_hydrogen_wave.svg: B_hydrogen_wave.dat
	echo '\
		set terminal svg size 1000,700 enhanced background rgb "white"; \
		set output "B_hydrogen_wave.svg"; \
		set grid; \
		set xlabel "r"; \
		set ylabel "f(r)"; \
		set title "Hydrogen ground-state wave-function"; \
		set key top right; \
		plot \
			"B_hydrogen_wave.dat" using 1:2 with lines title "numerical", \
			"B_hydrogen_wave.dat" using 1:3 with lines dashtype 2 title "exact r exp(-r)"; \
	' | gnuplot

B_convergence_rmax.svg: B_convergence_rmax.dat
	echo '\
		set terminal svg size 1000,700 enhanced background rgb "white"; \
		set output "B_convergence_rmax.svg"; \
		set grid; \
		set logscale y; \
		set xlabel "rmax"; \
		set ylabel "|E0 + 1/2|"; \
		set title "Convergence with rmax"; \
		set key top right; \
		plot "B_convergence_rmax.dat" using 1:3 with linespoints pointtype 7 title "energy error"; \
	' | gnuplot

B_convergence_rmin.svg: B_convergence_rmin.dat
	echo '\
		set terminal svg size 1000,700 enhanced background rgb "white"; \
		set output "B_convergence_rmin.svg"; \
		set grid; \
		set logscale xy; \
		set xlabel "rmin"; \
		set ylabel "|E0 + 1/2|"; \
		set title "Convergence with rmin"; \
		set key top right; \
		plot "B_convergence_rmin.dat" using 1:3 with linespoints pointtype 7 title "energy error"; \
	' | gnuplot

B_convergence_acc.svg: B_convergence_acc.dat
	echo '\
		set terminal svg size 1000,700 enhanced background rgb "white"; \
		set output "B_convergence_acc.svg"; \
		set grid; \
		set logscale xy; \
		set xlabel "ODE acc"; \
		set ylabel "|E0 + 1/2|"; \
		set title "Convergence with ODE acc"; \
		set key top right; \
		plot "B_convergence_acc.dat" using 1:3 with linespoints pointtype 7 title "energy error"; \
	' | gnuplot

B_convergence_eps.svg: B_convergence_eps.dat
	echo '\
		set terminal svg size 1000,700 enhanced background rgb "white"; \
		set output "B_convergence_eps.svg"; \
		set grid; \
		set logscale xy; \
		set xlabel "ODE eps"; \
		set ylabel "|E0 + 1/2|"; \
		set title "Convergence with ODE eps"; \
		set key top right; \
		plot "B_convergence_eps.dat" using 1:3 with linespoints pointtype 7 title "energy error"; \
	' | gnuplot

run: main
	./main -taskA > Out.txt
	./main -taskB >> Out.txt
	./main -taskC >> Out.txt

clean:
	rm -f main Out.txt *.dat *.svg
