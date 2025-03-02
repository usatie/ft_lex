all:
	make -C Ken_Thompson
	make -C Rob_Pike
	make -C Russ_Cox

test: regex
	./test_regex.sh

clean:
	make -C Ken_Thompson clean
	make -C Rob_Pike clean
	make -C Russ_Cox clean
