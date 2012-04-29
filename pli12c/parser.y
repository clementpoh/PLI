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
    Const   Uconst;
    BinOp   Ubinop;

    Funcs   Ufuncs;
    Func    Ufunc;

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

%token RETURNS

%token BEGINS
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

%token  GARBAGE

%nonassoc OR 
%nonassoc AND
%nonassoc NOT
%nonassoc EQ NE LT LE GT GE
%left ADD SUB
%left MUL DIV
%nonassoc UMINUS

%type <Ufuncs>  programme
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
%type <Uexpr>   binexpr

%type <Uconst>  constant

%start programme

%%

/**********************************************************************/

programme
    : programme function
        { parsed_prog = ins_func($2, parsed_prog); }
    | function
        { parsed_prog = ins_func($1, NULL); }
    ;

function
    : FUNCTION ID args RETURNS TYPE BEGINS decls stmtlist END
        { $$ = make_func($2, $3, $5, $7, $8, USER_DEFINED); }
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
    | param COMMA params
        { $$ = ins_param($1, $3); }
    ;

param 
    : ID COLON TYPE
        { $$ = make_param($1, $3); }
    ;   

decls
    : decl decls 
        { $$ = ins_decl($1, $2); }
    |   { $$ = NULL; }
    ;

decl
    : DECLARE ID TYPE SEMICOLON
        { $$ = make_decl($2, $3, NULL); }
    | DECLARE ID TYPE INITIALIZE TO constant SEMICOLON
        { $$ = make_decl($2, $3, $6); }
    ;

stmtlist
    : stmt stmtlist
        { $$ = ins_stmt($1, $2); }
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
    : expr COMMA exprlist
        { $$ = ins_expr($1, $3); }
    | expr
        { $$ = ins_expr($1, NULL); }
    ;

expr
    : ID
        { $$ = make_ident($1); }
    | constant
        { $$ = make_const($1); }
    | binexpr
        { $$ = $1; }
    | LPAREN expr RPAREN
        { $$ = $2; }
    | SUB expr %prec UMINUS
        { $$ = make_unop(UNOP_UMINUS, pli12yylinenum, $2); }
    | NOT expr
        { $$ = make_unop(UNOP_NOT, pli12yylinenum, $2); }
    | ID LPAREN exprlist RPAREN
        { $$ = make_call($1, $3); }
    ;

binexpr
    : expr OR expr  
        { $$ = make_binop(BINOP_OR, pli12yylinenum, $1, $3); }
    | expr AND expr
        { $$ = make_binop(BINOP_AND, pli12yylinenum, $1, $3); }
    | expr EQ expr  
        { $$ = make_binop(BINOP_EQ, pli12yylinenum, $1, $3); }
    | expr NE expr  
        { $$ = make_binop(BINOP_NE, pli12yylinenum, $1, $3); }
    | expr LT expr  
        { $$ = make_binop(BINOP_LT, pli12yylinenum, $1, $3); }
    | expr LE expr  
        { $$ = make_binop(BINOP_LE, pli12yylinenum, $1, $3); }
    | expr GT expr  
        { $$ = make_binop(BINOP_GT, pli12yylinenum, $1, $3); }
    | expr GE expr  
        { $$ = make_binop(BINOP_GE, pli12yylinenum, $1, $3); }
    | expr ADD expr 
        { $$ = make_binop(BINOP_ADD, pli12yylinenum, $1, $3); }
    | expr SUB expr 
        { $$ = make_binop(BINOP_SUB, pli12yylinenum, $1, $3); }
    | expr MUL expr 
        { $$ = make_binop(BINOP_MUL, pli12yylinenum, $1, $3); }
    | expr DIV expr 
        { $$ = make_binop(BINOP_DIV, pli12yylinenum, $1, $3); }
    ;

constant
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
