 int foo = 0; /* This line starts with a whitespace, and is therefore a program fragment, which will be copied to lex.yy.c. */
	int bar = 42; /* This line starts with a horizontal tab, and is therefore a program fragment, which will be copied to lex.yy.c. */
%{
/* Lines surrounded by %{ and %} only lines are also program fragments, and will be copied to lex.yy.c. */
int foobar = 42424242;
int foofoo = 4242;
int barbar = 424242;
%}
%%
. ;
