CXX      = clang++
CXXFLAGS = -std=c++23 -O2 -I. -Wall -Wextra -Werror
LDLIBS   = -lm

TARGET = main

all: $(TARGET) run plots

$(TARGET): main.cc matrix.h evd.h hydrogen_swave.h
	$(CXX) $(CXXFLAGS) main.cc -o $(TARGET) $(LDLIBS)

run: $(TARGET)
	./$(TARGET) -rmax 10 -dr 0.1 > Out.txt

plots: wavefunctions.svg conv_dr.svg conv_rmax.svg taskC.svg

wavefunctions.svg: hydrogen_wavefunctions.dat
	echo '\
		set terminal svg background rgb "white"; \
		set output "wavefunctions.svg"; \
		set xlabel "r [a_0]"; \
		set ylabel "f(r)"; \
		set grid; \
		set title "Hydrogen s-wave reduced radial wavefunctions"; \
		plot "hydrogen_wavefunctions.dat" using 1:2 with lines lw 2 title "n=1", \
		     "hydrogen_wavefunctions.dat" using 1:3 with lines lw 2 title "n=2", \
		     "hydrogen_wavefunctions.dat" using 1:4 with lines lw 2 title "n=3"; \
	' | gnuplot

conv_dr.svg: hydrogen_conv_dr.dat
	echo '\
		set terminal svg background rgb "white"; \
		set output "conv_dr.svg"; \
		set grid; \
		set key top left; \
		set xlabel "dr"; \
		set ylabel "E_0"; \
		set xrange [0.00:0.51]; \
		set yrange [-0.502:-0.470]; \
		set title "Convergence of ground state energy with respect to dr"; \
		Eexact = -0.5; \
		plot "hydrogen_conv_dr.dat" using 1:2 with linespoints pt 7 lw 2 title "numerical", \
		     Eexact with lines lw 2 title "exact"; \
	' | gnuplot

conv_rmax.svg: hydrogen_conv_rmax.dat
	echo '\
		set terminal svg background rgb "white"; \
		set output "conv_rmax.svg"; \
		set xlabel "rmax"; \
		set ylabel "E_0"; \
		set yrange [-0.502:-0.482]; \
		set grid; \
		set title "Convergence of ground state energy with respect to rmax"; \
		Eexact = -0.5; \
		plot "hydrogen_conv_rmax.dat" using 1:2 with linespoints pt 7 lw 2 title "numerical", \
		     Eexact with lines lw 2 title "exact"; \
	' | gnuplot

# ---------------- TASK C ----------------
GTIME = gtime --append --output $@ --format "%e"

taskC-check: $(TARGET)
	./$(TARGET) -taskC -n 50

taskC-times.dat: main Makefile
	> slow.tmp
	> fast.tmp
	for N in $$(seq 20 10 120); do \
		LC_ALL=C gtime --append --output slow.tmp --format "$$N %e" sh -c 'for i in $$(seq 1 20); do ./main -taskC -slow -n '"$$N"' > /dev/null; done'; \
		LC_ALL=C gtime --append --output fast.tmp --format "$$N %e" sh -c 'for i in $$(seq 1 20); do ./main -taskC -fast -n '"$$N"' > /dev/null; done'; \
	done
	paste slow.tmp fast.tmp | LC_ALL=C awk 'NF>=4 { \
		print $$1, $$2/20, $$4/20, $$2/$$4; \
	}' > $@
	rm -f slow.tmp fast.tmp

taskC.svg: taskC-times.dat
	echo '\
		set terminal svg background rgb "white"; \
		set output "taskC.svg"; \
		set xlabel "N"; \
		set ylabel "Time (s)"; \
		set xrange [15:125]; \
		set grid; \
		set key top left; \
		set title "Task C: EVD slow vs EVD fast"; \
		plot "taskC-times.dat" using 1:2 with linespoints pt 7 lw 2 title "EVD slow", \
		     "taskC-times.dat" using 1:3 with linespoints pt 7 lw 2 title "EVD fast"; \
	' | gnuplot

taskC: taskC-check taskC-times.dat taskC.svg

clean:
	$(RM) $(TARGET) Out.txt \
	      hydrogen_wavefunctions.dat hydrogen_conv_dr.dat hydrogen_conv_rmax.dat \
	      wavefunctions.svg conv_dr.svg conv_rmax.svg \
	      taskC-times.dat taskC.svg