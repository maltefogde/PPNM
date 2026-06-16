CXXFLAGS += -std=c++23 -I. -Wall -Wextra -Werror
LDLIBS += -lm
TIME := $(shell if command -v gtime >/dev/null 2>&1; then echo gtime; else echo /usr/bin/time; fi)

all: Out.txt out.times.data times.svg

main: main.cc matrix.h qr.h
	$(CXX) $(CXXFLAGS) main.cc -o main $(LDLIBS)

Out.txt: main
	./main > Out.txt

out.times.data: main
	> $@
	for N in $$(seq 100 50 500); do \
		$(TIME) --format "$$N %e" --output $@ --append \
		./$< -size $$N 1>out 2>err ; \
	done

times.svg: out.times.data
	echo '\
		set terminal svg background rgb "white"; \
		set output "$@"; \
		set xlabel "N"; \
		set ylabel "Time (s)"; \
		set xrange [90:510]; \
		set key top left; \
		f(x)=a*x**3; \
		a=1e-8; \
		set grid; \
		fit f(x) "$<" using 1:2 via a; \
		plot "$<" using 1:2 with points pt 7 title "Data", \
		     f(x) with lines lw 2 title "N^3 fit"; \
	' | gnuplot

clean:
	$(RM) main Out.txt out.times.data times.svg out err