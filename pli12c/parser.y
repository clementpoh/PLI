%{
/*
** vim: ts=4 sw=4 expandtab
*/

/*
**	Grammar for PLI12 programs.
*/

#include	<stdio.h>
#include	<ctype.h>
#include	"pli12c.h"
#include	"missing.h"

extern	char	*pli12yytext;

extern	void	pli12yyerror(const char *s);
%}

%union
{
	char        *Ustr;
	int         Uint;
	bool        Ubool;
	float       Ureal;
	Function	Ufunc;
}

%token FUNCTION

%token COLON
%token COMMA

%token INT
%token REAL
%token BOOL
%token STRING

%token RETURNS

%token BEGIN
%token END

%token DECLARE

%token INITIALIZE
%token TO

%token READ
%token WRITE

%token IF
%token THEN
%token ELSE
%token ENDIF

%token WHILE
%token DO
%token ENDWHILE

%token RETURN

%token OR
%token AND
%token NOT

%token ASSIGN
%token EQ
%token NE
%token LT
%token LE

%token PLUS
%token MINUS
%token TIMES
%token DIV

%token	<Uint>	INT_CONST
%token	<Ureal>	REAL_CONST
%token	<Ubool>	BOOL_CONST
%token	<Ustr>	STRING_CONST

%%

%%

void pli12yyerror(const char *s)
{
	char buf[80];

	if (pli12yychar <= 0) {
		sprintf(buf, "premature EOF");
		pli12yylinenum--;
	} else if (pli12yytext[0] == '\n' || pli12yytext[0] == '\f') {
		sprintf(buf, "%s at end of line", s);
	} else if (isprint(pli12yytext[0])) {
		sprintf(buf, "%s at symbol `%s'", s, pli12yytext);
	} else {
		sprintf(buf, "%s at \\%o", s, pli12yytext[0]);
    }

	printf("%s, %d: %s\n", pli12yyfile, pli12yylinenum, buf);
}
