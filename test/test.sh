#!/bin/bash
lex -o lexcmp.yy.c scanner.l 
clang -o scanner_cmp lexcmp.yy.c -ll
ft_lex scanner.l
clang -o scanner_ft lex.yy.c -ll
diff -u lexcmp.yy.c lex.yy.c
echo "42+1337+(21*19)" | ./scanner_cmp | tee cmp.txt
echo "42+1337+(21*19)" | ./scanner_ft > ft.txt
diff -u cmp.txt ft.txt
