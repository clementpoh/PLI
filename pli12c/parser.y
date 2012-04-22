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
    Const   Uconst;

    Params  Uparams;
    Param   Uparam;

    Decls   Udecls;
    Decl    Udecl;

    Stmts   Ustmts;
    Stmt    Ustmt;

    Exprs   Uexprs;
    Expr    Uexpr;
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

%token  <Ustr>  IDENT

%token  <Ustr>  TYPE

%nonassoc OR 
%nonassoc AND
%nonassoc NOT
%nonassoc EQ NE LT LE GT GE
%left ADD SUB
%left MUL DIV
%nonassoc UMINUS

%type <Ufunc>   function

%type <Uparams> params
%type <Uparam>  param

%type <Udecls>  decls
%type <Udecl>   decl

%type <Ustmts>  stmtlist
%type <Ustmt>   stmt

%type <Uexprs>  exprlist
%type <Uexpr>   expr

%type <Uconst>  const

%start programme

%%

/**********************************************************************/

programme
    : programme function
        { 
            parsed_prog->f_rest = parsed_prog->f_first;
            parsed_prog->f_first = $2;
        }
    | function
        { 
            parsed_prog->f_rest = parsed_prog->f_first;
            parsed_prog->f_first = $1;
        }
    ;

function
    : FUNCTION IDENT LPAREN params RPAREN RETURNS TYPE BEGIN decls stmtlist END
        { $$ = new_function($2, $4, $7, $9, $10); }
    ;

params
    : param 
        { $$ = param_node($1, NULL); }
    | params COMMA param
        { $$ = param_node($3, $1); }
    |   { $$ = NULL; }
    ;

param 
    : IDENT COLON TYPE
        { $$ = new_param($1, $3); }
    ;   

decls
    : decls decl
        { $$ = decl_node($2, $1); }
    |   { $$ = NULL; }
    ;

decl
    : DECLARE IDENT TYPE SEMICOLON
        { $$ = new_decl($2, $3, NULL); }
    | DECLARE IDENT TYPE INITIALIZE TO const
        { $$ = new_decl($2, $3, $6); }
    ;

stmtlist
    : stmtlist stmt
        { $$ = stmt_node($2, $1); }
    | stmt
        { $$ = stmt_node($1, NULL); }
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
        { $$ = expr_node($3, $1); }
    | expr
        { $$ = expr_node($1, NULL); }
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
