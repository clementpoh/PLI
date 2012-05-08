/*
** vim: ts=4 sw=4 expandtab
*/
/*
** This module contains the code generator for PLI12 programs.
*/

#include    "ast.h"
#include    "t12.h"
#include    "codegen.h"

make_call(char *func);
make_label(char *str);
Code translate_func(Func f);

Code
translate_prog(Funcs prog)
{
    Instr call = { .op = OP_CALL, .string_const = "main" };
    Instr halt = { .op = OP_HALT };

    Code code = instr_to_code(call);
    code = seq(translate_func(halt), code);
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

make_call(char *func) {
    Instr new = checked_malloc(sizeof(*new));

    new->opcode = OP_CALL;
    new->string_const = "main";

    return new;
}

make_label(char *str) {
    Instr new = checked_malloc(sizeof(*new));

    new->opcode = OP_LABEL;
    new->string_const = str;

    return new;
}
