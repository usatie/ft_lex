#!/bin/bash

GREEN='\033[0;32m'
RED='\033[0;31m'
RESET='\033[0m'

expect_match() {
	pattern=$1
	string=$2
	if ./regex "$pattern" "$string"; then
		echo -e "$GREEN""OK""$RESET: $pattern in $string"
	else
		echo -e "$RED""NG""$RESET: $pattern not in $string"
	fi
}

expect_no_match() {
	pattern=$1
	string=$2
	if ./regex "$pattern" "$string"; then
		echo -e "$RED""NG""$RESET: $pattern in $string"
	else
		echo -e "$GREEN""OK""$RESET: $pattern not in $string"
	fi
}

# Normal Characters
expect_match "a" "a"
expect_no_match "a" "b"
expect_match "b" "abc"
expect_match "abc" "abc"
expect_match "abc" "abcabc"
expect_match "abc" "zzzabczzz"
expect_match "abc" "zzzabc"
expect_no_match "abc" "zzzab"
expect_no_match "abc" "azzzbzzzc"

# Wildcard
expect_match "a.c" "abc"
expect_match "a.c" "azc"
expect_match "." "a"
expect_no_match "." ""

# Kleene Star
expect_match "a*" ""
expect_match "a*" "a"
expect_match "a*" "aa"
expect_match "a*" "b"
expect_match "a*b" "b"
expect_no_match "a*b" "c"
expect_match "aab*cc" "aabcc"
expect_match "aab*cc" "aacc"
expect_match "aab*cc" "aaabcc"
expect_match "aab*cc" "aabbbbbcc"
expect_match "aab*cc" "aacaacaacc"
expect_no_match "aab*cc" "aacaacaac"

expect_match ".*" "abc"
expect_match "a.*c" "a0123456789c"
expect_match "a.*c" "ac"
