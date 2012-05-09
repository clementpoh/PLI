/*
** vim: ts=4 sw=4 expandtab foldmethod=indent
*/
/*
** This module contains the code generator for PLI12 programs.
*/

#include    "ast.h"
#include    "t12.h"
#include    "codegen.h"
#include    "pli12c.h"
#include    "symbol.h"

static int  reg = 0;
static char buffer[128];

Instr make_op_call(const char *str);
Instr make_op(Opcode op);

Code translate_func(Func f);

Code translate_stmts(char *id, Stmts ss);
Code translate_stmt(char *id, Stmt s);

Code translate_expr(Expr e);

Code translate_prologue(char *str, Decls ds, int *size);
Code translate_epilogue(char *str, int size);

Code
translate_prog(Funcs prog)
{
    Instr halt = make_op(OP_HALT);
    Instr call = make_op_call("main");

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
    int     size = 0;
    Stmts   ss = f->stmts;
    Code    code = translate_prologue(f->id, f->decls, &size);

    code = seq(code, translate_stmts(f->id, ss));

    code = seq(code, translate_epilogue(f->id, size));

    return code;
}


Code translate_stmt(char *id, Stmt s) {
    Code    code;
    Instr   instr;
    Param   var;

    switch (s->t) {
        case STMT_ASSIGN:
            break;
        case STMT_READ:
            // Comment
            instr = make_op(OP_COMMENT);
            instr->string_const = "read";
            code = instr_to_code(instr);

            // Read
            instr = make_op(OP_CALL_BUILTIN);
            var = lookup_variable(id, s->s.Uread);
            switch (var->type) {
                case TYPE_INT:
                    instr->string_const = "read_int";
                    break;
                case TYPE_BOOL:
                    instr->string_const = "read_bool";
                    break;
                case TYPE_STRING:
                    instr->string_const = "read_string";
                    break;
                case TYPE_REAL:
                    instr->string_const = "read_real";
                default:
                    break;
            }
            code = seq(code, instr_to_code(instr));

            // Store
            instr = make_op(OP_STORE);
            instr->rs1 = 0;
            instr->int_const = var->pos;
            code = seq(code, instr_to_code(instr));
            break;
        case STMT_WRITE:
            // Comment
            instr = make_op(OP_COMMENT);
            instr->string_const = "write";
            code = instr_to_code(instr);
            // Expression
            code = seq(code, translate_expr(s->s.Uwrite));
            // Call
            instr = make_op(OP_CALL_BUILTIN);
            switch (s->s.Uwrite->r) {
                case TYPE_INT:
                    instr->string_const = "print_int";
                    break;
                case TYPE_REAL:
                    instr->string_const = "print_real";
                    break;
                case TYPE_BOOL:
                    instr->string_const = "print_bool";
                    break;
                case TYPE_STRING:
                    instr->string_const = "print_string";
                default:
                    break;
            }
            code = seq(code, instr_to_code(instr));
            break;
        case STMT_IF:
        case STMT_ELSE:
        case STMT_WHILE:
        case STMT_RETURN:
            break;
    }

    return code;
}

Code translate_expr(Expr e) {
    Code    code;
    Instr   instr;
    switch(e->t) {
        case EXPR_ID:
        case EXPR_CONST: 
            switch (e->r) {
                case TYPE_INT:
                    instr = make_op(OP_INT_CONST);
                    instr->rd = reg++;
                    instr->int_const = e->e.Uconst->val.Uint;
                case TYPE_REAL:
                    instr = make_op(OP_REAL_CONST);
                    instr->rd = reg++;
                    instr->real_const = e->e.Uconst->val.Ureal;
                case TYPE_BOOL:
                    instr = make_op(OP_INT_CONST);
                    instr->rd = reg++;
                    instr->bool_const = e->e.Uconst->val.Ubool;
                case TYPE_STRING:
                    instr = make_op(OP_STRING_CONST);
                    instr->rd = reg++;
                    instr->string_const = checked_strdup(e->e.Uconst->val.Ustr);
                default:
                    break;
            }
            code = instr_to_code(instr);
        case EXPR_BINOP:
        case EXPR_UNOP:
        case EXPR_FUNC:
            break;
    }
    return code;
}

Code translate_stmts(char *id, Stmts ss) {
    Code code;
    while (ss) {
        code = seq(code, translate_stmt(id, ss->s_first));
        ss = ss->s_rest;
    }

    return code;
}

Code translate_prologue(char *str, Decls ds, int *size) {
    Code    code;
    Decl    d;
    Instr   instr;

    // func label
    instr = make_op(OP_LABEL);
    sprintf(buffer, "func_%s", str);
    instr->string_const = checked_strdup(buffer);

    code = instr_to_code(instr);

    while(ds) {
        d = ds->d_first;

        // stack size
        instr = make_op(OP_COMMENT);
        sprintf(buffer, "variable is %s in stack slot %d", d->id, (*size)++);
        instr->string_const = checked_strdup(buffer);
        code = seq(code, instr_to_code(instr));

        ds = ds->d_rest;
    }

    // push
    instr = make_op(OP_PUSH_STACK_FRAME);
    instr->int_const = *size;
    code = seq(code, instr_to_code(instr));

    return code;
}


Code translate_epilogue(char *str, int size) {
    Code    code;
    Instr   instr;

    // endfunc label
    instr = make_op(OP_LABEL);
    sprintf(buffer, "endfunc_%s", str);
    instr->string_const = checked_strdup(buffer);

    code = instr_to_code(instr);

    // pop
    instr = make_op(OP_POP_STACK_FRAME);
    instr->int_const = size;
    code = seq(code, instr_to_code(instr));

    // return
    instr = make_op(OP_RETURN);
    code = seq(code, instr_to_code(instr));

    return code;
}

Instr make_op_call(const char *str) {
    Instr new = make_op(OP_CALL);
    sprintf(buffer, "func_%s", str);
    
    new->string_const = checked_strdup(buffer);
    return new;

}

Instr make_op(Opcode op) {
    Instr new = checked_malloc(sizeof(*new));

    new->opcode = op;

    return new;
}
