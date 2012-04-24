%{
/*
** vim: ts=4 sw=4 expandtab foldmethod=indent
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
    Func    Ufunc;
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

%token  <Ustr>  ID

%token  <Utype> TYPE

%nonassoc OR 
%nonassoc AND
%nonassoc NOT
%nonassoc EQ NE LT LE GT GE
%left ADD SUB
%left MUL DIV
%nonassoc UMINUS

%type <Ufunc>   function

%type <Uparams> args
%type <Uparams> params
%type <Uparam>  param

%type <Udecls>  decls
%type <Udecl>   decl

%type <Ustmts>  stmtlist
%type <Ustmt>   stmt

%type <Uexprs>  exprlist
%type <Uexpr>   expr

%type <BinOp>   binop
%type <Uconst>  const

%start programme

%%

/**********************************************************************/

programme
    : programme function
        { ins_func($2, $1); }
    | function
        { ins_func($1, NULL); }
    ;

function
    : FUNCTION ID args RETURNS TYPE BEGIN decls stmtlist END
        { $$ = make_func($2, $3, $5, $7, $8); }
    ;

args
    : LPAREN RPAREN
        { $$ = NULL; }
    | LPAREN params RPAREN
        { $$ = $2 }
    ;

params
    : param 
        { $$ = ins_param($1, NULL); }
    | params COMMA param
        { $$ = ins_param($3, $1); }
    ;

param 
    : ID COLON TYPE
        { $$ = make_param($1, $3); }
    ;   

decls
    : decls decl
        { $$ = ins_decl($2, $1); }
    |   { $$ = NULL; }
    ;

decl
    : DECLARE ID TYPE SEMICOLON
        { $$ = make_decl($2, $3, NULL); }
    | DECLARE ID TYPE INITIALIZE TO const
        { $$ = make_decl($2, $3, $6); }
    ;

stmtlist
    : stmtlist stmt
        { $$ = ins_stmt($2, $1); }
    | stmt
        { $$ = ins_stmt($1, NULL); }
    ;

stmt
    : ID ASSIGN expr SEMICOLON
        { $$ = make_assign($1, $3); }
    | READ ID SEMICOLON
        { $$ = make_read($2); }
    | WRITE expr SEMICOLON
        { $$ = make_write($2); }
    | IF expr THEN stmtlist ENDIF
        { $$ = make_if($2, $4); }
    | IF expr THEN stmtlist ELSE stmtlist ENDIF
        { $$ = make_else($2, $4, $6); }
    | WHILE expr DO stmtlist ENDWHILE
        { $$ = make_while($2, $4); }
    | RETURN expr SEMICOLON
        { $$ = make_return($2); }
    ;

exprlist
    : exprlist COMMA expr
        { $$ = ins_expr($3, $1); }
    | expr
        { $$ = ins_expr($1, NULL); }
    ;

expr
    : ID
        { $$ = make_ident($1); }
    | const
        { $$ = make_const($1); }
    | LPAREN expr RPAREN
        { $$ = $2 }
    | expr binop expr
        { $$ = make_binop($2, pli12yylinenum, $1, $3); }
    | SUB expr %prec UMINUS
        { $$ = make_unop(UNOP_UMINUS, pli12yylinenum, $2); }
    | NOT expr
        { $$ = make_unop(UNOP_NOT, pli12yylinenum, $2); }
    | ID LPAREN exprlist RPAREN
        { $$ = make_func_call($1, $3); }
    ;

binop
    : OR    { $$ = BINOP_OR }
    | AND   { $$ = BINOP_AND }
    | EQ    { $$ = BINOP_EQ }
    | NE    { $$ = BINOP_NE }
    | LT    { $$ = BINOP_LT }
    | GT    { $$ = BINOP_GT }
    | GE    { $$ = BINOP_GE }
    | ADD   { $$ = BINOP_ADD }
    | SUB   { $$ = BINOP_SUB }
    | MUL   { $$ = BINOP_MUL }
    | DIV   { $$ = BINOP_DIV }
    ;

const 
    : INT_CONST     { $$ = make_int($1); }
    | REAL_CONST    { $$ = make_real($1); }
    | BOOL_CONST    { $$ = make_bool($1); }
    | STRING_CONST  { $$ = make_str($1); }
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
