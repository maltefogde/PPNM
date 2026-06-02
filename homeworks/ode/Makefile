CXX = clang++
CXXFLAGS = -std=c++23 -O2 -Wall -Wextra -Werror

.PHONY: all run clean

all: Out.txt \
     Task_A_harmonic.svg Task_A_friction.svg Task_A_lotka.svg \
     Task_B_orbits.svg \
     Task_C_figure8.svg

main: main.cc ode.h vec.h
	$(CXX) $(CXXFLAGS) main.cc -o main

Out.txt \
Task_A_harmonic.dat Task_A_friction.dat Task_A_lotka.dat \
Task_B_circular.dat Task_B_ellipse.dat Task_B_precession.dat \
Task_C_figure8.dat: main
	./main -taskA > Out.txt
	./main -taskB >> Out.txt
	./main -taskC >> Out.txt

Task_A_harmonic.svg: Task_A_harmonic.dat
	echo '\
		set terminal svg background rgb "white"; \
		set output "Task_A_harmonic.svg"; \
		set multiplot layout 2,1 title "Task A.3: Harmonic oscillator"; \
		set grid; \
		set key bottom left; \
		set xlabel "x"; \
		set ylabel "u"; \
		plot \
			"Task_A_harmonic.dat" using 1:2 with lines title "Numeric u", \
			"Task_A_harmonic.dat" using 1:3 with lines dashtype 2 title "sin(x)"; \
		set xlabel "x"; \
		set ylabel "error"; \
		plot \
			"Task_A_harmonic.dat" using 1:4 with lines title "u - sin(x)"; \
		unset multiplot; \
	' | gnuplot

Task_A_friction.svg: Task_A_friction.dat
	echo '\
		set terminal svg background rgb "white"; \
		set output "Task_A_friction.svg"; \
		set title "Task A.4.1: Damped pendulum"; \
		set grid; \
		set key top right; \
		set xlabel "t"; \
		set ylabel "Value"; \
		plot \
			"Task_A_friction.dat" using 1:2 with lines title "theta(t)", \
			"Task_A_friction.dat" using 1:3 with lines title "omega(t)"; \
	' | gnuplot

Task_A_lotka.svg: Task_A_lotka.dat
	echo '\
		set terminal svg background rgb "white"; \
		set output "Task_A_lotka.svg"; \
		set multiplot layout 2,1 title "Task A.4.1: Lotka-Volterra"; \
		set grid; \
		set key top right; \
		set xlabel "t"; \
		set ylabel "Value"; \
		plot \
			"Task_A_lotka.dat" using 1:2 with lines title "x", \
			"Task_A_lotka.dat" using 1:3 with lines title "y"; \
		set xlabel "x"; \
		set ylabel "y"; \
		plot \
			"Task_A_lotka.dat" using 2:3 with lines title "phase portrait"; \
		unset multiplot; \
	' | gnuplot

Task_B_orbits.svg: Task_B_circular.dat Task_B_ellipse.dat Task_B_precession.dat
	echo '\
		set terminal svg size 1200,500 enhanced background rgb "white"; \
		set output "Task_B_orbits.svg"; \
		set multiplot layout 1,3 title "Task B: Planetary orbits"; \
		set grid; \
		set size ratio -1; \
		unset key; \
		set xlabel "x"; \
		set ylabel "y"; \
		set tmargin 3; \
		set bmargin 5; \
		set lmargin 6; \
		set rmargin 2; \
		set xrange [-2.1:2.1]; \
		set yrange [-2.1:2.1]; \
		set title "Newtonian circular, eps=0, up0=0"; \
		plot \
			"Task_B_circular.dat" using 4:5 with lines; \
		set title "Newtonian elliptical, eps=0, up0=-0.5"; \
		plot \
			"Task_B_ellipse.dat" using 4:5 with lines; \
		set title "Relativistic precession, eps=0.01, up0=-0.5"; \
		plot \
			"Task_B_precession.dat" using 4:5 with lines; \
		unset multiplot; \
	' | gnuplot

Task_C_figure8.svg: Task_C_figure8.dat
	echo '\
		set terminal svg size 800,400 background rgb "white"; \
		set output "Task_C_figure8.svg"; \
		\
		stats "Task_C_figure8.dat" using 1 nooutput; \
		nplot = int(STATS_records*0.30); \
		\
		set title "Task C: Three-body figure-eight orbit"; \
		set xlabel "x"; \
		set ylabel "y"; \
		set size ratio -1; \
		set xrange [-1.2:1.2]; \
		set yrange [-0.45:0.45]; \
		set xtics 0.4; \
		set ytics 0.2; \
		set grid lw 0.5 lc rgb "#dddddd"; \
		set border lw 1.2; \
		set key outside right center vertical; \
		\
		set style line 1 lc rgb "#1f77b4" lw 5.0; \
		set style line 2 lc rgb "#d62728" lw 5.0; \
		set style line 3 lc rgb "#2ca02c" lw 5.0; \
		set style line 4 lc rgb "#1f77b4" pt 15 ps 1.2; \
		set style line 5 lc rgb "#d62728" pt 15 ps 1.2; \
		set style line 6 lc rgb "#2ca02c" pt 15 ps 1.2; \
		set style line 7 lc rgb "#000000" lw 1.5; \
		\
		plot \
			"Task_C_figure8.dat" using 2:3 with lines ls 7 title "Common orbit", \
			"Task_C_figure8.dat" using 2:3 every ::0::nplot with lines ls 1 title "body 1", \
			"Task_C_figure8.dat" using 4:5 every ::0::nplot with lines ls 2 title "body 2", \
			"Task_C_figure8.dat" using 6:7 every ::0::nplot with lines ls 3 title "body 3", \
			"Task_C_figure8.dat" using 2:3 every ::0::0 with points ls 4 title "start 1", \
			"Task_C_figure8.dat" using 4:5 every ::0::0 with points ls 5 title "start 2", \
			"Task_C_figure8.dat" using 6:7 every ::0::0 with points ls 6 title "start 3"; \
	' | gnuplot

run: main
	./main -taskA > Out.txt
	./main -taskB >> Out.txt
	./main -taskC >> Out.txt

clean:
	rm -f main Out.txt *.dat *.svg