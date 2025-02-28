#!/bin/bash
expect_match() {
	pattern=$1
	string=$2
	if ./regex "$pattern" "$string"; then
		echo "OK: $pattern in $string"
	else
		echo "NG: $pattern not in $string"
	fi
}

expect_no_match() {
	pattern=$1
	string=$2
	if ./regex "$pattern" "$string"; then
		echo "NG: $pattern in $string"
	else
		echo "OK: $pattern not in $string"
	fi
}

expect_match "a" "a"
expect_no_match "a" "b"
expect_match "b" "abc"
expect_match "abc" "abc"
expect_match "abc" "abcabc"
expect_match "abc" "zzzabczzz"
expect_match "abc" "zzzabc"
expect_no_match "abc" "zzzab"
