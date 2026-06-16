CXX = clang++
CXXFLAGS = -std=c++23 -O2 -Wall -Wextra -Werror

.PHONY: all run clean

all: Out.txt erf.svg erf_acc.svg

main: main.cc integrator.h
	$(CXX) $(CXXFLAGS) main.cc -o main

Out.txt erf.dat erf_acc.dat: main comparison.py
	./main -taskA > Out.txt
	./main -taskB >> Out.txt
	./main -taskC >> Out.txt
	python3 comparison.py >> Out.txt

erf.svg: erf.dat
	echo '\
		set terminal svg background rgb "white"; \
		set output "erf.svg"; \
		set grid; \
		set xlabel "z"; \
		set ylabel "erf(z)"; \
		set key bottom right; \
		plot \
			"erf.dat" using 1:2 with lines title "integral representation", \
			"erf.dat" every 5 using 1:3 with points pointtype 7 title "std::erf"; \
	' | gnuplot

erf_acc.svg: erf_acc.dat
	echo '\
		set terminal svg background rgb "white"; \
		set output "erf_acc.svg"; \
		set grid; \
		set logscale xy; \
		set xlabel "acc"; \
		set ylabel "|erf(1) - std::erf(1)|"; \
		set key bottom right; \
		plot \
			"erf_acc.dat" using 1:2 with linespoints pointtype 7 title "actual error", \
			x with lines dashtype 2 title "error = acc"; \
	' | gnuplot

run: main
	./main -taskA > Out.txt
	./main -taskB >> Out.txt
	./main -taskC >> Out.txt

clean:
	rm -f main Out.txt *.dat *.svg