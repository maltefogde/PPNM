CXX = g++
CXXFLAGS = -std=c++23 -Wall -Wextra
LDLIBS = -lm

all: Out.txt
	cat $<

Out.txt: main
	./$< > $@

main: main.o hello.o
main.o: hello.h vec.h

.PHONY: clean
clean:
	$(RM) main *.o Out.txt
