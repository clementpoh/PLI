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
    code = NULL;
    while(prog) {
        code = seq(translate_func(prog->f_first), code);
        prog = prog-f_rest;
    }

    return code;
}

Code
translate_func(Func f) {
    Instr lbl = make_label(f->id);
    
    
    
    

}

make_label(char *str) {
    Instr new = checked_malloc(sizeof(*new));

    new->opcode = OP_LABEL;
    new->string_const = str;

    return new;
}
