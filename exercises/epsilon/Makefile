CXXFLAGS = -Wall -Wextra -Werror -std=c++23 -O
LDLIBS += -lstdc++ -lm 

Out.txt : main
	./main > Out.txt
	cat Out.txt

main: main.o approx.o
main.o: main.cc approx.h
approx.o: approx.cc approx.h

.PHONY:clean
clean:
	$(RM) main main.o Out.txt *.o

test:
	@echo CXX
	@echo $(CXX)
	@echo $(RM)
	@echo $(CC)
	@echo $(SHELL)