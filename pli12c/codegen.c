/*
** vim: ts=4 sw=4 expandtab
*/
/*
** This module contains the code generator for PLI12 programs.
*/

#include    "ast.h"
#include    "t12.h"
#include    "codegen.h"
#include    "pli12c.h"

Instr make_instr(Opcode op);
Instr make_instr_str(Opcode op, char *str, const char *pre);

Code translate_func(Func f);

Code
translate_prog(Funcs prog)
{
    Instr halt = make_instr(OP_HALT);
    Instr call = make_instr_str(OP_CALL, "main", "func");

    Code code = instr_to_code(halt);
    code = seq(instr_to_code(call), code);
    
    while(prog) {
        code = seq(code, translate_func(prog->f_first));
        prog = prog->f_rest;
    }

    return code;
}

Code
translate_func(Func f) {
    Decls decls = f->decls;
    int locals = 0;

    Instr instr = make_instr_str(OP_LABEL, f->id, "func");
    Code code = instr_to_code(instr);

    while(decls) {
        locals++;
        instr = make_instr_str(OP_COMMENT, "variable is %s in stack slot %d");
        decls = decls->d_rest;
    }

    instr = make_instr(OP_PUSH_STACK_FRAME);
    instr->int_const = locals;
    code = seq(code, instr_to_code(instr));

    
    return code;
}

Instr make_instr(Opcode op) {
    Instr new = checked_malloc(sizeof(*new));

    new->opcode = op;

    return new;
}

Instr make_instr_str(Opcode op, char *str, const char *pre) {
    Instr new = make_instr(op);
    char buffer[128];

    if (pre) {
        sprintf(buffer, "%s_%s", pre, str);
    } else {
        sprintf(buffer, "%s", str);
    }
    
    new->string_const = checked_strdup(buffer);
    return new;
}
