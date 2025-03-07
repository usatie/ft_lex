delim     [ \t\n]
ws	      {delim}+

%%
{ws}      { printf(" "); }
.		  { printf("%c", *yytext); }
