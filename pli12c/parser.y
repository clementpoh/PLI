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

/* This is for you to fill in */

void pli12yyerror(const char *s)
{
	char		buf[80];

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
