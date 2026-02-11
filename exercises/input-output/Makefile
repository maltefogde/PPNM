CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++23 -O

all: main-cmdline main-stdin main-files

main-cmdline: main-cmdline.o
	$(CXX) $(CXXFLAGS) $^ -o $@

main-stdin: main-stdin.o
	$(CXX) $(CXXFLAGS) $^ -o $@

main-files: main-files.o
	$(CXX) $(CXXFLAGS) $^ -o $@

clean:
	$(RM) main-cmdline main-stdin main-files *.o out.txt Out.txt input.txt inputfile my_input_file.txt my_output_file.txt
