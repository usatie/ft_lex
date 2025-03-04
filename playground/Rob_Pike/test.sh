#!/bin/bash

# Colors for output
GREEN="\033[0;32m"
RED="\033[0;31m"
RESET="\033[0m"

# Ensure the regex binary is compiled
make >/dev/null

run_test() {
	local pattern="$1"
	local input="$2"
	local expected=$3
	local description="$4"

	./regex "$pattern" "$input"
	local result=$?

	if [ "$result" -eq "$expected" ]; then
		echo -e "${GREEN}[PASS]${RESET} $description"
	else
		echo -e "${RED}[FAIL]${RESET} $description"
		echo "  Pattern: '$pattern'"
		echo "  Input: '$input'"
		echo "  Expected: $([ "$expected" -eq 0 ] && echo 'match' || echo 'no match')"
		echo "  Got: $([ "$result" -eq 0 ] && echo 'match' || echo 'no match')"
	fi
}

echo "=== Simple Regex Tests ==="

# Basic matching tests
run_test "a" "a" 0 "Simple character match"
run_test "a" "b" 1 "Simple character non-match"
run_test "abc" "abc" 0 "Multiple character match"
run_test "abc" "abd" 1 "Multiple character non-match"

# Wildcard tests
run_test "." "a" 0 "Wildcard matches any character"
run_test "a.c" "abc" 0 "Wildcard in the middle"
run_test "..." "abc" 0 "Multiple wildcards"
run_test "..." "ab" 1 "Multiple wildcards with insufficient input"

# Kleene star tests
run_test "a*" "" 0 "Star matches zero occurrences"
run_test "a*" "a" 0 "Star matches one occurrence"
run_test "a*" "aaa" 0 "Star matches multiple occurrences"
run_test "ab*c" "ac" 0 "Star with surrounding characters (zero)"
run_test "ab*c" "abc" 0 "Star with surrounding characters (one)"
run_test "ab*c" "abbbc" 0 "Star with surrounding characters (multiple)"

# Plus operator tests
run_test "a+" "a" 0 "Plus matches one occurrence"
run_test "a+" "aaa" 0 "Plus matches multiple occurrences"
run_test "a+" "" 1 "Plus doesn't match zero occurrences"
run_test "ab+c" "abc" 0 "Plus with surrounding characters (one)"
run_test "ab+c" "abbbc" 0 "Plus with surrounding characters (multiple)"
run_test "ab+c" "ac" 1 "Plus with surrounding characters (zero - should fail)"

# Combined operators
run_test "a.*b" "ab" 0 "Star with wildcard (minimum)"
run_test "a.*b" "axyzb" 0 "Star with wildcard (multiple)"
run_test "a.+b" "axb" 0 "Plus with wildcard (minimum)"
run_test "a.+b" "axyzb" 0 "Plus with wildcard (multiple)"
run_test "a.+b" "ab" 1 "Plus with wildcard (insufficient)"

# Partial matches
run_test "abc" "xabcy" 0 "Match substring anywhere"

echo "=== Tests completed ==="
