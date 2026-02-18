CXXFLAGS = -Wall -Werror -Wextra -Wall -std=c++23 -O
LDLIBS += -lstdc++ -lm

Out.txt: main
	./$< > $@

clean:
	$(RM) [Oo]ut* *.o main
