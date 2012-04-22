%{
/*
** vim: ts=4 sw=4 expandtab
*/

/*
**  Grammar for PLI12 programs.
*/

#include    <stdio.h>
#include    <ctype.h>
#include    "pli12c.h"
#include    "missing.h"

extern  char    *pli12yytext;

extern  void    pli12yyerror(const char *s);
%}

%union
{
    char    *Ustr;
    int     Uint;
    bool    Ubool;
    float   Ureal;
    Type    Utype;
    Funcs   Ufunc;
    Params  Uparams;
    Stmts   Ustmts;
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
%token SEMICOLON

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

%token  <Uint>  INT_CONST
%token  <Ureal> REAL_CONST
%token  <Ubool> BOOL_CONST
%token  <Ustr>  STRING_CONST

%token  <UStr>  IDENT

%token  <UStr>  TYPE

%nonassoc OR 
%nonassoc AND
%nonassoc NOT
%nonassoc EQ NE LT LE GT GE
%left ADD SUB
%left MUL DIV
%nonassoc UMINUS

%type <Ufunc> function
%type <Uparams> params
%type <Ustmts> stmtlist

%start programme

%%

/**********************************************************************/

programme
    : function programme
        { 
            parsed_prog.f_rest = parsed_prog.f_first;
            parsed_prog.first = $1;
        }
    | function
        { 
            parsed_prog.f_rest = parsed_prog.f_first;
            parsed_prog.first = $1;
        }
    ;

function
    : FUNCTION IDENT LPAREN params RPAREN RETURNS TYPE BEGIN stmtlist END
        {
            $$ = new_function($2, $4, $7, $9);
        }
    ;

params
    : IDENT COLON TYPE COMMA
    | IDENT COLON TYPE
    | { $$ = NULL; }
    ;

stmtlist
    : stmtlist stmt
    | stmt
    ;

stmt
    : IDENT ASSIGN expr SEMICOLON
    | READ IDENT SEMICOLON
    | WRITE expr SEMICOLON
    | IF expr THEN stmtlist ENDIF
    | IF expr THEN stmtlist ELSE stmtlist ENDIF
    | WHILE expr DO stmtlist ENDWHILE
    | RETURN expr SEMICOLON
    ;

exprlist
    : exprlist COMMA expr
    | expr
    ;

expr
    : IDENT
    | const
    | LPAREN expr RPAREN
    | expr binop expr
    | SUB expr %prec UMINUS
    | NOT expr
    | IDENT LPAREN exprlist RPAREN
    ;

binop
    : OR
    | AND 
    | NOT
    | EQ
    | NE 
    | LT 
    | GT 
    | GE
    | ADD
    | SUB
    | MUL
    | DIV
    ;

const 
    : INT_CONST 
    | REAL_CONST 
    | STRING_CONST 
    | BOOL_CONST
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
