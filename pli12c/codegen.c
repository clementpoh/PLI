/*
** vim: ts=4 sw=4 expandtab
*/
/*
** This module contains the code generator for PLI12 programs.
*/

#include    "ast.h"
#include    "t12.h"
#include    "codegen.h"

Code
translate_prog(Funcs prog)
{
    while(prog) {
        translate_func(prog);
        prog = prog-f_rest;
    }

    return code;
}

/* This is for you to fill in */
