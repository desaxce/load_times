CXXFLAGS=-std=c++0x

test: test.cc

.PHONY: clean

clean:
	rm -rf test
	rm -rf *log
