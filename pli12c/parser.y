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
    Type        Utype;
	Function	Ufunc;
}

%token FUNCTION

%token LPAREN
%token RPAREN

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

%token NOT
%token OR
%token AND

%token ASSIGN
%token EQ
%token NE
%token LT
%token LE
%token GT
%token GE

%token ADD 
%token SUB
%token MUL
%token DIV

%token	<Uint>	INT_CONST
%token	<Ureal>	REAL_CONST
%token	<Ubool>	BOOL_CONST
%token	<Ustr>	STRING_CONST

%token  <UStr>  IDENTIFIER

%token  <UStr>  TYPE

%nonassoc OR 
%nonassoc AND
%nonassoc NOT
%nonassoc EQ NE LT LE GT GE
%left ADD SUB
%left MUL DIV
%nonassoc UMINUS

%start programme

%%

programme   :   function programme
    | function
    ;

function    :   header BEGIN stmtlist END;

header      :   FUNCTION IDENTIFIER LPAREN args RPAREN RETURNS TYPE;

args        :   IDENTIFIER COLON TYPE COMMA
    | IDENTIFIER COLON TYPE
    |
    ;

stmtlist    :  stmt stmtlist
    | stmt
    ;

stmt        : IDENTIFIER ASSIGN expr SEMICOLON
    |   READ IDENTIFIER SEMICOLON
    |   WRITE expr SEMICOLON
    |   IF expr THEN stmtlist ENDIF
    |   IF expr THEN stmtlist ELSE stmtlist ENDIF
    |   WHILE expr DO stmtlist ENDWHILE
    |   RETURN expr SEMICOLON
    ;

exprlist    : expr COMMA exprlist
    | expr
    ;

expr    :   IDENTIFIER
    |   const
    |   LPAREN expr RPAREN
    |   expr op expr
    |   unop expr
    |   IDENTIFIER LPAREN exprlist RPAREN

exprlist    :   expr COMMA exprlist
    | expr
    ;

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
