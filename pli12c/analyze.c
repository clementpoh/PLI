/*
** vim: ts=4 sw=4 expandtab
*/
/*
** This module contains the semantic analyzer for PLI12 programs.
*/

#include    "ast.h"
#include    "analyze.h"
#include    "symbol.h"

static void pop_sym_table(Funcs prog_funcs);

Funcs
analyze_prog(Funcs prog_funcs)
{
    Funcs funcs = prog_funcs;
    pop_sym_table(funcs);
    return funcs;
}

static void pop_sym_table(Funcs prog_funcs) {
    Funcs func = prog_funcs;
    while(func) {
        add_user_function(func->f_first);

        func = func->f_rest;
    }
}
