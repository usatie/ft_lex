%{
	/* definitions of manifest constants
	LT, LE, EQ, NE, GT, GE,
	IF, THEN, ELSE, WHILE, ID, NUMBER, RELOP */
%}

/* regular definitions */
delim     [ \t\n]
ws	      {delim}+
letter    [_A-Za-z]
digit     [0-9]
id        {letter}({letter}|{digit})*
number    {digit}+(\.{digit}+)?(E[+-]?{digit}+)?
dq        \"
esc_dq    \\\"
string    {dq}([^"]|{esc_dq})*{dq}


%%

{ws}      { /* no action and no return */ }
if        {return(IF);}
then      {return(THEN);}
else      {return(ELSE);}
while     {return(WHILE);}
{id}      {yylval = (int) installID(); return(ID);}
{number}  {yylval = (int) installNum(); return(NUMBER);}
{string}  {yylval = (int) installString(); return(STRING);}
"<"       {yylval = LT; return(RELOP);}
"<="      {yylval = LE; return(RELOP);}
"=="      {yylval = EQ; return(RELOP);}
"!="      {yylval = NE; return(RELOP);}
">"       {yylval = GT; return(RELOP);}
">="      {yylval = GE; return(RELOP);}

%%

int installID() {/* function to install the lexeme, whose
                    first character is pointed to by yytext,
					and whose length is yyleng, into the
					symbol table and return a pointer
					thereto */
}

int installNum() {/* similar to installID, but puts numer-
                     ical constants into a separate table */
}

int installString() {
}
