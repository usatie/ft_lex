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

expect_match "a" "a"
expect_no_match "a" "b"
expect_match "b" "abc"
expect_match "abc" "abc"
expect_match "abc" "abcabc"
expect_match "abc" "zzzabczzz"
expect_match "abc" "zzzabc"
expect_no_match "abc" "zzzab"
expect_match "a.c" "abc"
expect_match "a.c" "azc"
expect_match "." "a"
expect_no_match "." ""
