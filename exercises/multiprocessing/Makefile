CXXFLAGS += -std=c++23 -Wall -O3
LDLIBS   = -pthread
TIME     = gtime --append --output $@ --format "$$nthreads %e %U"

N=1e9

all: times.svg

main: main.o
	$(CXX) $^ -o $@ $(LDLIBS)

main.o: main.cc
	$(CXX) $(CXXFLAGS) -c $<

out.times: main Makefile
	> $@
	> log.main
	for nthreads in 1 2 3 4 5 6 7 8; do \
		$(TIME) ./main -terms $(N) -threads $$nthreads >> log.main ; \
	done

times.svg: out.times Makefile
	echo '\
		set terminal svg ;\
		set output "$@" ;\
		set xlabel "number of threads" ;\
		set ylabel "real time (s)" ;\
		a=1; b=10;\
		f(x)=a+b/x;\
		T0=0.25; s=0.05;\
		f2(x)=T0*(s + (1-s)/x) ;\
		fit f(x) "$<" using 1:2 via a,b ;\
		fit f2(x) "$<" using 1:2 via T0,s ;\
		plot \
			"$<" using 1:2 with linespoints title "times", \
			f(x)  with lines lw 3 lc rgb "blue"  title "fit: a+b/n", \
		    f2(x) with lines lw 3 dt 2 lc rgb "red" title "fit: Amdahl" ;\
	'| tee log.times |gnuplot

clean:
	$(RM) log* [Oo]ut* main *.svg fit.log