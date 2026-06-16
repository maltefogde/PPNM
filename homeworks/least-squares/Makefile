CXX      = clang++
CXXFLAGS = -std=c++23 -O2 -I. -Wall -Wextra -Werror
LDLIBS   = -lm

TARGET = main

all: $(TARGET) run plots

$(TARGET): main.cc ../lib/matrix.h ../lib/qr.h lsfit.h
	$(CXX) $(CXXFLAGS) main.cc -o $(TARGET) $(LDLIBS)

run: $(TARGET)
	./$(TARGET) > Out.txt

plots: run thx_fit.svg thx_logfit.svg thx_taskC.svg

thx_fit.svg: thx_fit.dat thx_fit_curve.dat
	echo '\
		set terminal svg background rgb "white"; \
		set output "thx_fit.svg"; \
		set xlabel "Time t (days)"; \
		set ylabel "Activity"; \
		set grid; \
		set title "ThX radioactive decay: Data and exponential fit"; \
		plot "thx_fit.dat" using 1:2:3 with yerrorbars pt 7 ps 1 lw 1 title "Data", \
		     "thx_fit_curve.dat" using 1:2 with lines lw 2 title "Best fit"; \
	' | gnuplot

thx_logfit.svg: thx_logfit.dat
	echo '\
		set terminal svg background rgb "white"; \
		set output "thx_logfit.svg"; \
		set xlabel "Time t (days)"; \
		set ylabel "ln(activity)"; \
		set grid; \
		set title "Linearized fit: ln(y)=ln(a)-lambda*t"; \
		plot "thx_logfit.dat" using 1:2:3 with yerrorbars pt 7 ps 1 lw 1 title "ln(data)", \
		     "thx_logfit.dat" using 1:4 with lines lw 2 title "Linear fit"; \
	' | gnuplot

thx_taskC.svg: thx_taskC.dat thx_data.dat
	echo '\
		set terminal svg size 900,700 enhanced background rgb "white"; \
		set output "thx_taskC.svg"; \
		set xlabel "time t (days)"; \
		set ylabel "activity"; \
		set grid; \
		set title "Task C: best fit and fits with shifted coefficients"; \
		plot "thx_data.dat" using 1:2:3 with yerrorbars pt 7 ps 1 lw 1 title "Data", \
		     "thx_taskC.dat" using 1:2 with lines lw 3 title "Best fit", \
		     "thx_taskC.dat" using 1:3 with lines dt 2 lw 2 title "(c0-dc0,c1-dc1)", \
		     "thx_taskC.dat" using 1:4 with lines dt 2 lw 2 title "(c0-dc0,c1+dc1)", \
		     "thx_taskC.dat" using 1:5 with lines dt 2 lw 2 title "(c0+dc0,c1-dc1)", \
		     "thx_taskC.dat" using 1:6 with lines dt 2 lw 2 title "(c0+dc0,c1+dc1)"; \
	' | gnuplot

clean:
	$(RM) $(TARGET) Out.txt \
	      thx_fit.dat thx_fit_curve.dat thx_logfit.dat \
	      thx_data.dat thx_taskC.dat \
	      thx_fit.svg thx_logfit.svg thx_taskC.svg