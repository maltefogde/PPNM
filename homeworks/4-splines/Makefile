CXX = clang++
CXXFLAGS = -std=c++23 -O2 -Wall -Wextra -Werror
TARGET = main

all: plots

$(TARGET): main.cc
	$(CXX) $(CXXFLAGS) main.cc -o $(TARGET) $(LDLIBS)

run: $(TARGET)
	./$(TARGET) > Out.txt

plots: run Task_A1.svg Task_B1.svg Task_B_debug.svg Task_C_cubic_comparison.svg

Task_A1.dat Task_A1_points.dat: $(TARGET)
	./$(TARGET) -taskA

Task_B1.dat Task_B1_points.dat: $(TARGET)
	./$(TARGET) -taskB

const.dat const_points.dat linear.dat linear_points.dat quadratic.dat quadratic_points.dat: $(TARGET)
	./$(TARGET) > Out.txt

Task_A1.svg: Task_A1.dat Task_A1_points.dat
	echo '\
		set terminal svg background rgb "white"; \
		set output "Task_A1.svg"; \
		set title "Linear spline, cosine function"; \
		set xlabel "x"; \
		set ylabel "y"; \
		set ytics nomirror; \
		set y2tics; \
		set grid; \
		set key outside bottom center horizontal; \
		set bmargin 6; \
		plot \
			"Task_A1.dat" using 1:2 with lines lw 4 lc rgb "#1f77b4" title "Linear Interpolant S(x)", \
			"Task_A1_points.dat" using 1:2 with points pt 7 ps 1 lc rgb "red" title "Data points", \
			"Task_A1.dat" using 1:3 with lines dashtype 2 lw 4 lc rgb "#ff7f0e" title "Anti-derivative"; \
	' | gnuplot

Task_B1.svg: Task_B1.dat Task_B1_points.dat
	echo '\
		set terminal svg background rgb "white"; \
		set output "Task_B1.svg"; \
		set title "OOP: Quadratic Spline, cosine function"; \
		set xlabel "x"; \
		set ylabel "y"; \
		set ytics nomirror; \
		set y2tics; \
		set grid; \
		set key outside bottom center horizontal; \
		set bmargin 6; \
		plot \
			"Task_B1.dat" using 1:2 with lines lw 4 lc rgb "#1f77b4" title "Quadratic Interpolant S(x)", \
			"Task_B1_points.dat" using 1:2 with points pt 7 ps 1 lc rgb "red" title "Data points", \
			"Task_B1.dat" using 1:3 with lines dashtype 2 lw 4 lc rgb "#ff7f0e" title "Derivative", \
			"Task_B1.dat" using 1:4 with lines dashtype 2 lw 4 lc rgb "#16ff0e" title "Anti-derivative"; \
	' | gnuplot

Task_B_debug.svg: const.dat const_points.dat linear.dat linear_points.dat quadratic.dat quadratic_points.dat
	echo '\
		set terminal svg size 700,900 enhanced background rgb "white"; \
		set output "Task_B_debug.svg"; \
		set multiplot layout 3,1 title "Quadratic spline test cases"; \
		set grid; \
		set key top left; \
		set xrange [1:5]; \
		set ytics nomirror; \
		\
		set title "Test 1: y = 1"; \
		set xlabel ""; \
		set ylabel "y"; \
		plot \
			"const.dat" using 1:2 with lines lw 4 lc rgb "#1f77b4" title "Spline S(x)", \
			"const_points.dat" using 1:2 with points pt 7 ps 1 lc rgb "red" title "Data points", \
			"const.dat" using 1:3 with lines dt 2 lw 4 lc rgb "#ff7f0e" title "Derivative", \
			"const.dat" using 1:4 with lines dt 3 lw 4 lc rgb "#2ca02c" title "Anti-derivative"; \
		\
		set title "Test 2: y = x"; \
		set xlabel ""; \
		set ylabel "y"; \
		plot \
			"linear.dat" using 1:2 with lines lw 4 lc rgb "#1f77b4" title "Spline S(x)", \
			"linear_points.dat" using 1:2 with points pt 7 ps 1 lc rgb "red" title "Data points", \
			"linear.dat" using 1:3 with lines dt 2 lw 4 lc rgb "#ff7f0e" title "Derivative", \
			"linear.dat" using 1:4 with lines dt 3 lw 4 lc rgb "#2ca02c" title "Anti-derivative"; \
		\
		set title "Test 3: y = x^2"; \
		set xlabel "x"; \
		set ylabel "y"; \
		plot \
			"quadratic.dat" using 1:2 with lines lw 4 lc rgb "#1f77b4" title "Spline S(x)", \
			"quadratic_points.dat" using 1:2 with points pt 7 ps 1 lc rgb "red" title "Data points", \
			"quadratic.dat" using 1:3 with lines dt 2 lw 4 lc rgb "#ff7f0e" title "Derivative", \
			"quadratic.dat" using 1:4 with lines dt 3 lw 4 lc rgb "#2ca02c" title "Anti-derivative"; \
		unset multiplot; \
	' | gnuplot

Task_C_cubic_points.dat Task_C_cubic_own.dat: $(TARGET)
	./$(TARGET) -taskC

Task_C_cubic_comparison.svg: Task_C_cubic_points.dat Task_C_cubic_own.dat
	echo '\
		set terminal svg background rgb "white"; \
		set output "Task_C_cubic_comparison.svg"; \
		set title "Cubic spline: own implementation vs gnuplot smooth csplines"; \
		set xlabel "x"; \
		set ylabel "y"; \
		set grid; \
		set key top center; \
		set samples 1000; \
		plot \
			"Task_C_cubic_own.dat" using 1:2 with lines lw 3 title "Own CubicSpline", \
			"Task_C_cubic_points.dat" using 1:2 smooth csplines with lines dashtype 2 lw 3 title "gnuplot smooth csplines", \
			"Task_C_cubic_points.dat" using 1:2 with points pt 7 ps 1.2 title "Interpolation points"; \
	' | gnuplot

clean:
	$(RM) $(TARGET) Out.txt \
		Task_A1.dat Task_A1_points.dat Task_A1.svg \
		Task_B1.dat Task_B1_points.dat Task_B1.svg \
		const.dat const_points.dat \
		linear.dat linear_points.dat \
		quadratic.dat quadratic_points.dat \
		Task_B_debug.svg \
		Task_C_cubic_points.dat Task_C_cubic_own.dat \
		Task_C_cubic_comparison.svg