CXXFLAGS += -Wall -std=c++23
LDLIBS += -lm

all: fig1.svg fig2.svg fig3.svg

test.txt: erf.data Makefile
	>$@
	cat erf.data | while read a b c; do echo $$a $$c >> $@; done

fig3.svg: fig3.gpi erf.data
	gnuplot $<

fig2.svg: fig2.gpi erf.data
	gnuplot $<

fig1.svg: fig1.gpi erf.data erf_tabulated.txt
	gnuplot $<

erf.data: main Makefile
	./$< -xmin -4.9 -xmax 4.9 dx 0.125 > $@ 2>log.erf

.PHONY: all clean

clean:
	$(RM) main *.o \
	      erf.data test.txt \
	      fig1.svg fig2.svg fig3.svg \
	      log.erf