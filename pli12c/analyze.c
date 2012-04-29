/*
** vim: ts=4 sw=4 expandtab
*/
/*
** This module contains the semantic analyzer for PLI12 programs.
*/

#include    "ast.h"
#include    "analyze.h"

static void verify_types(Funcs prog_funcs);

Funcs
analyze_prog(Funcs prog_funcs)
{
    // verify_types(prog_funcs);
    return prog_funcs;
}

/* This is for you to fill in */
