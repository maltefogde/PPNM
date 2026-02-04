CXXFLAGS = -Wall -Wextra -Werror -std=c++23 -O
LDLIBS += -lstdc++ -lm 

Out.txt : main
	./main > Out.txt
	cat Out.txt

main.o: sfuns.h
sfuns.o: sfuns.h
main : main.o sfuns.o

.PHONY:clean
clean:
	$(RM) main main.o Out.txt *.o

test:
	@echo CXX
	@echo $(CXX)
	@echo $(RM)
	@echo $(CC)
	@echo $(SHELL)