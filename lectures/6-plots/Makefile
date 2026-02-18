CXXFLAGS += -Wall -std=c++23
LDLIBS += -lm

all: fig1.svg fig2.png fig3.svg

test.txt: erf.data Makefile
	>$@
	cat erf.data | while read a b c; do echo $$a $$c >> $@; done

fig3.svg: test.txt Makefile
	graph \
		--x-label "x" \
		--y-label "y" \
		--output-format svg \
		$< > $@

fig2.png: erf.data Makefile
	echo '\
	set terminal png ;\
	set output "$@" ;\
	set tics out ;\
	set xlabel "x" ;\
	set ylabel "y" ;\
	plot [][-2:2] \
		"$<" index 1 using 1:2 with linespoints \
		,"$<" index 1 using 1:3 with lines ;\
	' | tee log.fig2.gpi | gnuplot

fig1.svg: fig1.gpi erf.data
	gnuplot $<

erf.data: main Makefile
	./$< -xmin -2 -xmax 2 dx 0.5 > $@ 2>log.erf
	echo "\n\n" >> $@
	./$< -xmin 0 -xmax 10 dx 0.255 >> $@ 2>>log.erf

.PHONY: all clean

clean:
	$(RM) main *.o \
	      erf.data test.txt \
	      fig1.svg fig2.png fig3.svg \
	      log.erf log.fig2.gpi