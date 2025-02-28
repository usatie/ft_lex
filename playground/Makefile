CXX = c++
CC = cc

run: test

regex_compiler_thompson: regex_compiler_thompson.cpp
	$(CXX) -o regex_compiler_thompson regex_compiler_thompson.cpp

regex_rob_pike: regex_rob_pike.c
	$(CC) -o regex_rob_pike regex_rob_pike.c

regex_compiler: regex_compiler.c
	$(CXX) -o regex regex_compiler.c

test: regex
	./test_regex.sh

clean:
	$(RM) a.out regex_compiler_thompson regex_rob_pike regex regex_compiler
