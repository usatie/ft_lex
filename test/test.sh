#!/bin/bash -x

# Create a test utility function to run the above test, taking the .l file and the test cases as arguments
test_lex() {
	local file="$1"
	shift
	local test_cases=("$@")

	local cmp_c_file="${file}.cmp.yy.c"
	local ft_c_file="lex.yy.c"
	local cmp_exe="${file}.cmp"
	local ft_exe="${file}.ft"

	# Generate using standard lex
	lex -o "$cmp_c_file" "$file"
	clang -o "$cmp_exe" "$cmp_c_file" -ll

	# Generate using ft_lex
	../ft_lex "$file"
	clang -o "$ft_exe" "$ft_c_file" -ll

	# Run test cases
	for test_case in "${test_cases[@]}"; do
		echo "$test_case" | ./"$cmp_exe" >cmp.txt
		echo "$test_case" | ./"$ft_exe" >ft.txt
		diff -u cmp.txt ft.txt
	done

	# Clean up
	rm -f "$cmp_c_file" "$cmp_exe" "$ft_c_file" "$ft_exe" cmp.txt ft.txt
}

# Unit tests
## Empty .l file
## Minimal .l file
test_lex "minimum.l" "" "non empty input"

## Definition unit tests
## Rule unit tests
## Rule unit tests with actions

# Simple yet integrated test
test_lex "scanner.l" "42+1337+(21*19)"
