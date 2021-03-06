%{
/*
** vim: ts=4 sw=4 expandtab
*/
/*
**  Scanner for PLI12.
*/

#define __USE_SVID
#define __USE_POSIX
#define __USE_XOPEN_EXTENDED
#define _GNU_SOURCE
#define __EXTENSIONS__

#include    <stdio.h>
#include    <string.h>
#include    "std.h"
#include    "pli12c.h"
#include    "parser.h"
#include    "missing.h"

/* Declarations flex should add but doesn't. */
extern  int     pli12yyget_lineno(void);
extern  FILE    *pli12yyget_in(void);
extern  FILE    *pli12yyget_out(void);
extern  int     pli12yyget_leng(void);
extern  char    *pli12yyget_text(void);
extern  void    pli12yyset_lineno(int);
extern  void    pli12yyset_in(FILE *);
extern  void    pli12yyset_out(FILE *);
extern  int     pli12yyget_debug(void);
extern  void    pli12yyset_debug(int);
extern  int     pli12yylex_destroy(void);

#undef  pli12yywrap
#define YY_NO_UNPUT

extern  void    pli12yyinit(void);
extern  int     pli12yywrap(void);

%}

alpha   [a-zA-Z_]
digit   [0-9]
alnum   [a-zA-Z_0-9]
sp  [ \t]
nl  [\n\f]
nonl    [^\n\f]

%%

"function"  { 
                pli12yylval.Uint = pli12yylinenum;
                return FUNCTION; 
            }

"("         { return LPAREN; }
")"         { return RPAREN; }

":"         { return COLON; }
","         { return COMMA; }
 
"returns"   { return RETURNS; }

"begin"     { return BEGINS; }
"end"       { return END; }

"declare"   { return DECLARE; }
";"         { return SEMICOLON; }

"initialize" { return INITIALIZE; }
"to"        { return TO; }

"read"      { return READ; }
"write"     { return WRITE; }

"if"        { return IF; }
"then"      { return THEN; }
"else"      { return ELSE; }
"endif"     { return ENDIF; }

"while"     { return WHILE; }
"do"        { return DO; }
"endwhile"  { return ENDWHILE; }

"return"    { return RETURN; }

"not"       { return NOT; }
"or"        { return OR; }
"and"       { return AND; }

":="        { return ASSIGN; }
"="         { return EQ; }
"!="        { return NE; }
"<"         { return LT; }
"<="        { return LE; }
">"         { return GT; }
">="        { return GE; }

"+"         { return ADD; }
"-"         { return SUB; }
"*"         { return MUL; }
"/"         { return DIV; }


"true"      {
                pli12yylval.Ubool = 1;
                return BOOL_CONST;
            }

"false"     {
                pli12yylval.Ubool = 0;
                return BOOL_CONST;
            }

"int"       {
                pli12yylval.Utype = TYPE_INT;
                return TYPE;
            }

"real"      {
                pli12yylval.Utype = TYPE_REAL;
                return TYPE;
            }

"bool"      {
                pli12yylval.Utype = TYPE_BOOL;
                return TYPE;
            }

"string"    {
                pli12yylval.Utype = TYPE_STRING;
                return TYPE;
            }

{digit}+    {
                pli12yylval.Uint = atoi(yytext);
                return INT_CONST;
            }

{digit}+"."{digit}+ {
                pli12yylval.Ureal = atof(yytext);
                return REAL_CONST;
            }

"\""([^\\\"]|\\.)*"\""	{
                pli12yylval.Ustr = checked_strdup(yytext);
                return STRING_CONST;
            }

{alpha}{alnum}*     {
                pli12yylval.Ustr = checked_strdup(yytext);
                return ID;
            }

"#"{nonl}*{nl}      { pli12yylinenum++; }

{nl}                { pli12yylinenum++; }

{sp}			{}
{nonl} 			{ return GARBAGE;	}

%%

void
pli12yyinit(void)
{
}

int
pli12yywrap(void)
{
    return 1;
}
