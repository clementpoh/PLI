/*
** vim: ts=4 sw=4 expandtab foldmethod=indent
**
**
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

Code translate_expr(char *id, Expr e);
Code translate_binop(Expr e);

Code translate_prologue(char *str, Decls ds, int *size);
Code translate_epilogue(char *str, int size);

Instr make_arith(Opcode op, Type t);
Code make_comment(const char *str);

Code load_const(Const c, int r);
Code load_from(int slot, int r);
Code save_to(int slot, int r);
const char *suff(Type t);

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
            code = make_comment("assignment");
            // Expression
            var = lookup_variable(id, s->s.Uassign.id);
            code = seq(code, translate_expr(id, s->s.Uassign.expr));
            code = seq(code, save_to(var->pos, reg));
            break;
        case STMT_READ:
            code = make_comment("read");
            // Read
            instr = make_op(OP_CALL_BUILTIN);
            var = lookup_variable(id, s->s.Uread);
            switch (var->type) {
                case TYPE_INT: instr->string_const = "read_int"; break;
                case TYPE_REAL: instr->string_const = "read_real"; break;
                case TYPE_BOOL: instr->string_const = "read_bool"; break;
                case TYPE_STRING: instr->string_const = "read_string"; break;
                case TYPE_ERROR: instr->string_const = "read_error"; break;
            }
            code = seq(code, instr_to_code(instr));
            code = seq(code, save_to(var->pos, reg));
            break;
        case STMT_WRITE:
            code = make_comment("write");
            // Expression
            code = seq(code, translate_expr(id, s->s.Uwrite));
            // Call
            instr = make_op(OP_CALL_BUILTIN);
            switch (s->s.Uwrite->r) {
                case TYPE_INT: instr->string_const = "print_int"; break;
                case TYPE_REAL: instr->string_const = "print_real"; break;
                case TYPE_BOOL: instr->string_const = "print_bool"; break;
                case TYPE_STRING: instr->string_const = "print_string"; break;
                case TYPE_ERROR: instr->string_const = "print_error"; break;
            }
            code = seq(code, instr_to_code(instr));
            break;
        case STMT_IF:
            code = make_comment("if");
            break;
        case STMT_ELSE:
            code = make_comment("else");
            break;
        case STMT_WHILE:
            code = make_comment("while");
            break;
        case STMT_RETURN:
            code = make_comment("return");
            break;
    }

    return code;
}

Code translate_expr(char *id, Expr e) {
    Code    code;
    Param   var;
    switch(e->t) {
        case EXPR_ID:
            // Load id into reg.
            var = lookup_variable(id, e->e.Uid);
            code = load_from(var->pos, reg);
            break;
        case EXPR_CONST: 
            // Load appropriate const into reg.
            code = load_const(e->e.Uconst, reg);
            break;
        case EXPR_BINOP:
            code = translate_expr(id, e->e.Ubinop.e1);
            reg++;
            code = seq(code, translate_expr(id, e->e.Ubinop.e2));
            code = seq(code, translate_binop(e));
            break;
        case EXPR_UNOP:
            break;
        case EXPR_FUNC:
            break;
    }
    return code;
}

Code translate_binop(Expr e) {
    Instr   instr;
    switch (e->e.Ubinop.op) {
        case BINOP_OR:
            instr = make_arith(OP_OR, e->r);
            break;
        case BINOP_AND:
            instr = make_arith(OP_AND, e->r);
            break;
        case BINOP_EQ:
            instr = make_arith(OP_CMP_EQ, e->e.Ubinop.e1->t);
            break;
        case BINOP_NE:
            instr = make_arith(OP_CMP_NE, e->e.Ubinop.e1->t);
            break;
        case BINOP_LT:
            instr = make_arith(OP_CMP_LT, e->e.Ubinop.e1->t);
            break;
        case BINOP_LE:
            instr = make_arith(OP_CMP_LE, e->e.Ubinop.e1->t);
            break;
        case BINOP_GT:
            instr = make_arith(OP_CMP_GT, e->e.Ubinop.e1->t);
            break;
        case BINOP_GE:
            instr = make_arith(OP_CMP_GE, e->e.Ubinop.e1->t);
            break;
        case BINOP_ADD:
            instr = make_arith(OP_ADD, e->r);
            break;
        case BINOP_SUB:
            instr = make_arith(OP_SUB, e->r);
            break;
        case BINOP_MUL:
            instr = make_arith(OP_MUL, e->r);
            break;
        case BINOP_DIV:
            instr = make_arith(OP_DIV, e->r);
            break;
    }
    return instr_to_code(instr);
}

Code translate_binop(Expr e) {
    Instr   instr;
    switch (e->e.Uunop.op) {
        case UNOP_NOT:
        case UNOP_UMINUS:
        case UNOP_INT_TO_REAL:

    }
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

Code load_from(int slot, int r) {
    Instr instr = make_op(OP_LOAD);
    instr->rd = r;
    instr->int_const = slot;

    return instr_to_code(instr);
}

Code load_const(Const c, int r) {
    Instr instr;
    switch (c->type) {
        case TYPE_INT:
            instr = make_op(OP_INT_CONST);
            instr->rd = r;
            instr->int_const = c->val.Uint;
            break;
        case TYPE_REAL:
            instr = make_op(OP_REAL_CONST);
            instr->rd = r;
            instr->real_const = c->val.Ureal;
            break;
        case TYPE_BOOL:
            instr = make_op(OP_INT_CONST);
            instr->rd = r;
            instr->bool_const = c->val.Ubool;
            break;
        case TYPE_STRING:
            instr = make_op(OP_STRING_CONST);
            instr->rd = r;
            instr->string_const = checked_strdup(c->val.Ustr);
        default:
            break;
    }
    return instr_to_code(instr);
}

Code save_to(int slot, int r) {
    Instr instr = make_op(OP_STORE);
    instr->rs1 = r;
    instr->int_const = slot;
    return instr_to_code(instr);
}

Code make_comment(const char *str) {
    Instr instr = make_op(OP_COMMENT);
    instr->string_const = str;
    return instr_to_code(instr);
}

Instr make_arith(Opcode op, Type t) {
    Instr instr;

    instr = make_op(op);
    instr->suffix = suff(t);
    instr->rs2 = reg--;
    instr->rs1 = reg;
    instr->rd = reg;

    return instr;
}

const char *suff(Type t) {
    switch (t) {
        case TYPE_INT:
            return "int";
        case TYPE_REAL:
            return "real";
        case TYPE_BOOL:
            return "bool";
        case TYPE_STRING:
            return "string";
        case TYPE_ERROR:
            return "error";
    }
    return "error";
}
