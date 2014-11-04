# Method std::to_string() requires --std=c++0x compilation flag
CXXFLAGS=-std=c++0x -Wall -Werror -O2
OPTIONS=-Wall -Werror -O2

test: test.cc test.hh

.PHONY: clean

clean:
	@rm -rf test
	@rm -rf *.log
	@rm -rf *.res
	@rm -rf *.html
	@rm -rf *.txt
	@# rm -rf *.css
	@# rm -rf *.js
