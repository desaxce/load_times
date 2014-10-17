CXXFLAGS=-std=c++0x -Wall -Werror -O2
OPTIONS=-Wall -Werror -O2

test: test.cc

.PHONY: clean

clean:
	@rm -rf test
	@rm -rf *.log
	@rm -rf *.res
	@rm -rf *.html
	@rm -rf 198.50.151.105
	@rm -rf 161.106.2.57
	@rm -rf localhost
	@rm -rf 172.20.36.138
	@rm -rf wired
	@rm -rf 3G
	@rm -rf output
	@# rm -rf *.css
	@# rm -rf *.js
