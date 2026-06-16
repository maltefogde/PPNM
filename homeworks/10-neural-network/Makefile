CXX = clang++
CXXFLAGS = -std=c++23 -O2 -Wall -Wextra -Werror

.PHONY: all run clean

all: Out.txt A_interpolation.svg B_derivatives.svg C_ode.svg

main: main.cc ann.hpp
	$(CXX) $(CXXFLAGS) main.cc -o main

Out.txt A_interpolation.dat B_derivatives.dat C_ode.dat: main
	./main -taskA > Out.txt
	./main -taskB >> Out.txt
	./main -taskC >> Out.txt

A_interpolation.svg: A_interpolation.dat
	echo '\
		set terminal svg background rgb "white"; \
		set output "A_interpolation.svg"; \
		set grid; \
		set xlabel "x"; \
		set ylabel "y"; \
		set title "Task A: neural-network interpolation"; \
		set key top right; \
		plot \
			"A_interpolation.dat" using 1:2 with lines title "exact", \
			"A_interpolation.dat" using 1:3 with lines dashtype 2 title "network"; \
	' | gnuplot

B_derivatives.svg: B_derivatives.dat
	echo '\
		set terminal svg background rgb "white"; \
		set output "B_derivatives.svg"; \
		set grid; \
		set xlabel "x"; \
		set ylabel "Derivative value"; \
		set title "Task B: analytic derivatives"; \
		set key outside bottom center horizontal; \
		plot \
			"B_derivatives.dat" using 1:4 with lines lw 3 title "g′(x)", \
			"B_derivatives.dat" using 1:5 with lines lw 3 dashtype 2 title "F_p′(x)", \
			"B_derivatives.dat" using 1:6 with lines lw 3 title "g′′(x)", \
			"B_derivatives.dat" using 1:7 with lines lw 3 dashtype 2 title "F_p′′(x)"; \
	' | gnuplot

C_ode.svg: C_ode.dat
	echo '\
		set terminal svg background rgb "white"; \
		set output "C_ode.svg"; \
		set grid; \
		set xlabel "x"; \
		set ylabel "y"; \
		set title "Task C: neural-network solution of y second + y = 0"; \
		set key top right; \
		set xrange [-0.1:3.3]; \
		set yrange [-0.05:1.05]; \
		plot \
			"C_ode.dat" using 1:2 with lines lw 3 title "sin(x)", \
			"C_ode.dat" using 1:3 with lines lw 3 dashtype 2 title "network"; \
	' | gnuplot

run: main
	./main -taskA > Out.txt
	./main -taskB >> Out.txt
	./main -taskC >> Out.txt

clean:
	rm -f main Out.txt *.dat *.svg