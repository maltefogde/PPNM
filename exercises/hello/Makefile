# Out.txt depends on hello
Out.txt: hello
	./hello > Out.txt

# hello depends on hello.cc
hello: hello.cc
	c++ -o hello hello.cc

clean:
	rm -f Out.txt hello
