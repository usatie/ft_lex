delim     [ \t\n]
nondelim  [^ \t\n]
any       {nondelim}+

%%
{delim}   { printf("%c", *yytext); }
float     { printf("double"); }
{any}     { int c = yytext[yyleng]; yytext[yyleng] = '\0'; printf("%s", yytext); yytext[yyleng] = c; }
