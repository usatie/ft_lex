#!/bin/bash

GREEN='\033[0;32m'
RED='\033[0;31m'
RESET='\033[0m'

expect_output() {
	input="$1"
	expected="$2"

	result=$(./shunting-yard "$input" | grep "Output:" | cut -d "'" -f 2)

	if [ "$result" = "$expected" ]; then
		echo -e "${GREEN}OK${RESET}: '$input' converted to '$result'"
	else
		echo -e "${RED}NG${RESET}: '$input' should convert to '$expected', got '$result'"
	fi
}

expect_error() {
	input="$1"

	if ./shunting-yard "$input" | grep -q "Error"; then
		echo -e "${GREEN}OK${RESET}: Error correctly detected for '$input'"
	else
		echo -e "${RED}NG${RESET}: Expected error for '$input' but got success"
	fi
}

# Basic tests
expect_output "a+b" "ab+"
expect_output "a+b*c" "abc*+"
expect_output "a*b+c" "ab*c+"

# Precedence tests
expect_output "a+b*c+d" "abc*+d+"
expect_output "a*b+c*d" "ab*cd*+"
expect_output "a+b+c+d" "ab+c+d+"
expect_output "a*b*c*d" "ab*c*d*"

# Parentheses tests
expect_output "(a+b)*c" "ab+c*"
expect_output "a*(b+c)" "abc+*"
expect_output "(a+b)*(c+d)" "ab+cd+*"

# Right associative operators
expect_output "a=b=c" "abc=="
expect_output "!a" "a!"
expect_output "!!a" "a!!"

# Error cases
expect_error ")a+b("
expect_error "(a+b"
expect_error "a+b)"
expect_error "a@b"

echo -e "\nAll tests completed"
