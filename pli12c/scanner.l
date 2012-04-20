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
/* This is for you to fill in */

%}

alpha	[a-zA-Z_]
digit	[0-9]
alnum	[a-zA-Z_0-9]
sp	[ \t]
nl	[\n\f]
nonl	[^\n\f]

%%

{digit}+		{
				t12yylval.Uint = atoi(yytext);
				return NAT_CONST;
			}

{digit}+"."{digit}+	{
				t12yylval.Ureal = atof(yytext);
				return REAL_CONST;
			}

"true"			{
				t12yylval.Ubool = 1;
				return BOOL_CONST;
			}

"false"			{
				t12yylval.Ubool = 0;
				return BOOL_CONST;
			}


"r"{digit}+		{
				t12yylval.Uint = atoi(yytext+1);
				return TOKEN_REG;
			}

{alpha}{alnum}*		{
				t12yylval.Ustr = strdup(yytext);
				return TOKEN_ID;
			}


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
