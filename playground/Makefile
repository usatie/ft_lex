CXX = c++
CC = cc

run: test

regex_compiler: regex_compiler.cpp
	$(CXX) -o regex_compiler regex_compiler.cpp

regex: regex.c
	$(CC) -o regex regex.c

test: regex
	./test_regex.sh
