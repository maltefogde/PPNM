CXX      = c++
CXXFLAGS = -std=c++23 -Wall -O
LDLIBS   = -lm

main: main.o
	$(CXX) $(CXXFLAGS) $^ $(LDLIBS) -o $@

main.o: main.cc matrix.hpp

Out.txt: main
	./$< > $@

clean:
	$(RM) main *.o Out.txt
