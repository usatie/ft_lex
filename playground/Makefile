CXX = c++
CC = cc

regex_compiler: regex_compiler.cpp
	$(CXX) -o regex_compiler regex_compiler.cpp

regex: regex.c
	$(CC) -o regex regex.c
	./regex abc aaa
	./regex abc abc
	./regex abc zzzabcdefgzzz
