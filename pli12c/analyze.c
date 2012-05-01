/*
** vim: ts=4 sw=4 expandtab
*/
/*
** This module contains the semantic analyzer for PLI12 programs.
*/

#include    "ast.h"
#include    "analyze.h"

Funcs
analyze_prog(Funcs prog_funcs)
{
    Funcs func = prog_funcs;
    while(func) {
        // add_user_function(func);

        func = prog_funcs->f_rest;
    }
}
