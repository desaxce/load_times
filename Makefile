CXXFLAGS=-std=c++0x
OPTIONS=-Wall -Werror -O2

test: test.cc

.PHONY: clean

clean:
	@rm -rf test
	@rm -rf *.log
	@rm -rf *results
	@rm -rf *.html
	@# rm -rf *.css
	@# rm -rf *.js
