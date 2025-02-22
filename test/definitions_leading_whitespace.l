 int foo = 0; /* This line starts with a whitespace, and is therefore a program fragment, which will be copied to lex.yy.c. */
	int bar = 42; /* This line starts with a horizontal tab, and is therefore a program fragment, which will be copied to lex.yy.c. */
%%
. ;
