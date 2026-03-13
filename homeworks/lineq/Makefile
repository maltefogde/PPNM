CXXFLAGS += -std=c++23 -I. -Wall -Wextra -Werror
LDLIBS += -lm

all: Out.txt out.times.data times.svg

main: main.cc matrix.h qr.h
	$(CXX) $(CXXFLAGS) main.cc -o main $(LDLIBS)

Out.txt: main
	./main > Out.txt

out.times.data: main
	> $@
	for N in $$(seq 100 50 500); do \
		gtime --format "$$N %e" --output $@ --append \
		./$< -size $$N 1>out 2>err ; \
	done

times.svg: out.times.data
	echo '\
		set terminal svg size 700,500; \
		set output "$@"; \
		set xlabel "N"; \
		set ylabel "time (s)"; \
		f(x)=a*x**3; \
		a=1e-8; \
		fit f(x) "$<" using 1:2 via a; \
		plot "$<" using 1:2 with points pt 7 title "data", \
		     f(x) with lines lw 2 title "fit ~ N^3"; \
	' | gnuplot

clean:
	$(RM) main Out.txt out.times.data times.svg out err