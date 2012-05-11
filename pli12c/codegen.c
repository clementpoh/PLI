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

static char buffer[128];
static int label = 0;

Instr make_op_call(const char *str);
Instr make_op(Opcode op);

Code translate_func(Func f);

Code translate_stmts(char *id, int r, Stmts ss);
Code translate_stmt(char *id, int r, Stmt s);

Code translate_expr(char *id, int r, Expr e);
Code translate_binop(int r, Expr e);
Code translate_unop(int r, char *id, Expr e);

Code translate_prologue(char *id, Params ps, Decls ds, int *size);
Code translate_epilogue(char *str, int size);

Instr make_arith(int r, Opcode op, Type t);

Code make_branch(Opcode op, const char *str, int r);
Code make_label(const char *str);
Code make_comment(const char *str);

Code load_const(int r, Const c);
Code load_from(int r, int slot);
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
    int     reg = 0;
    Stmts   ss = f->stmts;
    Code    code = translate_prologue(f->id, f->args, f->decls, &size);

    code = seq(code, translate_stmts(f->id, reg, ss));

    code = seq(code, translate_epilogue(f->id, size));

    return code;
}

Code translate_stmt(char *id, int r, Stmt s) {
    Code    code;
    Instr   instr;
    Param   var;
    int     orig = label;

    switch (s->t) {
        case STMT_ASSIGN:
            code = make_comment("assignment");
            // Expression
            var = lookup_variable(id, s->s.Uassign.id);
            code = seq(code, translate_expr(id, r, s->s.Uassign.expr));
            code = seq(code, save_to(var->pos, r));
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
            code = seq(code, save_to(var->pos, r));
            break;
        case STMT_WRITE:
            code = make_comment("write");
            // Expression
            code = seq(code, translate_expr(id, r, s->s.Uwrite));
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
            label++;
            code = make_comment("if");
            // condition
            code = seq(code, translate_expr(id, r, s->s.Uif.cond));
            // branch
            sprintf(buffer, "label%d", orig);
            code = seq(code, make_branch(OP_BRANCH_ON_FALSE, buffer, r));
            // body
            code = seq(code, translate_stmts(id, r, s->s.Uif.then));
            // label
            sprintf(buffer, "label%d", orig++);
            code = seq(code, make_label(buffer));
            break;
        case STMT_ELSE:
            label += 2;
            code = make_comment("else");
            // condition
            code = seq(code, translate_expr(id, r, s->s.Uelse.cond));
            // branch
            sprintf(buffer, "label%d", orig++);
            code = seq(code, make_branch(OP_BRANCH_ON_FALSE, buffer, r));
            // then
            code = seq(code, translate_stmts(id, r, s->s.Uelse.then));
            // jump to after the else
            sprintf(buffer, "label%d", orig--);
            code = seq(code, make_branch(OP_BRANCH_UNCOND, buffer, 0));
            // label
            sprintf(buffer, "label%d", orig++);
            code = seq(code, make_label(buffer));
            // else
            code = seq(code, translate_stmts(id, r, s->s.Uelse.other));
            // label after the else
            sprintf(buffer, "label%d", orig);
            code = seq(code, make_label(buffer));
            break;
        case STMT_WHILE:
            label += 2;
            code = make_comment("while");
            // label
            sprintf(buffer, "label%d", orig++);
            code = seq(code, make_label(buffer));
            // condition
            code = seq(code, translate_expr(id, r, s->s.Uwhile.cond));
            // branch
            sprintf(buffer, "label%d", orig--);
            code = seq(code, make_branch(OP_BRANCH_ON_FALSE, buffer, r));
            // loop body
            code = seq(code, translate_stmts(id, r, s->s.Uwhile.rep));
            // branch back
            sprintf(buffer, "label%d", orig++);
            code = seq(code, make_branch(OP_BRANCH_UNCOND, buffer, 0));
            // label
            sprintf(buffer, "label%d", orig);
            code = seq(code, make_label(buffer));
            break;
        case STMT_RETURN:
            code = make_comment("return");
            code = seq(code, translate_expr(id, 0, s->s.Ureturn));
            sprintf(buffer, "endfunc_%s", id);
            code = seq(code, make_branch(OP_BRANCH_UNCOND, buffer, 0));
            break;
    }

    return code;
}

Code translate_expr(char *id, int r, Expr e) {
    Code    code;
    Param   var;
    switch(e->t) {
        case EXPR_ID:
            // Load id into reg.
            var = lookup_variable(id, e->e.Uid);
            code = load_from(r, var->pos);
            break;
        case EXPR_CONST: 
            // Load appropriate const into reg.
            code = load_const(r, e->e.Uconst);
            break;
        case EXPR_BINOP:
            code = translate_expr(id, r, e->e.Ubinop.e1);
            code = seq(code, translate_expr(id, r + 1, e->e.Ubinop.e2));
            code = seq(code, translate_binop(r, e));
            break;
        case EXPR_UNOP:
            code = translate_unop(r, id, e);
            break;
        case EXPR_FUNC:
            break;
    }
    return code;
}

Code translate_binop(int r, Expr e) {
    Instr   instr;
    Type t = e->e.Ubinop.e1->r;
    switch (e->e.Ubinop.op) {
        case BINOP_OR: instr = make_arith(r, OP_OR, e->r); break;
        case BINOP_AND: instr = make_arith(r, OP_AND, e->r); break;
        case BINOP_EQ: instr = make_arith(r, OP_CMP_EQ, t); break;
        case BINOP_NE: instr = make_arith(r, OP_CMP_NE, t); break;
        case BINOP_LT: instr = make_arith(r, OP_CMP_LT, t); break;
        case BINOP_LE: instr = make_arith(r, OP_CMP_LE, t); break;
        case BINOP_GT: instr = make_arith(r, OP_CMP_GT, t); break;
        case BINOP_GE: instr = make_arith(r, OP_CMP_GE, t); break;
        case BINOP_ADD: instr = make_arith(r, OP_ADD, t); break;
        case BINOP_SUB: instr = make_arith(r, OP_SUB, t); break;
        case BINOP_MUL: instr = make_arith(r, OP_MUL, t); break;
        case BINOP_DIV: instr = make_arith(r, OP_DIV, t); break;
    }
    return instr_to_code(instr);
}

Code translate_unop(int r, char *id, Expr e) {
    Code    code;
    Instr   instr;
    switch (e->e.Uunop.op) {
        case UNOP_NOT:
            code = translate_expr(id, r, e->e.Uunop.e); 
            instr = make_arith(r, OP_NOT, e->r);
            code = seq(code, instr_to_code(instr));
            break;
        case UNOP_INT_TO_REAL:
            code = translate_expr(id, r, e->e.Uunop.e); 
            instr = make_arith(r, OP_INT_TO_REAL, e->r);
            code = seq(code, instr_to_code(instr));
            break;
        case UNOP_UMINUS:
            code = translate_expr(id, r + 1, e->e.Uunop.e); 
            switch (e->r) {
                case TYPE_INT:
                    instr = make_op(OP_INT_CONST);
                    instr->rd = r;
                    instr->int_const = 0;

                    code = seq(code, instr_to_code(instr));
                    instr = make_arith(r, OP_SUB, e->r);
                    code = seq(code, instr_to_code(instr));
                    break;
                case TYPE_REAL:
                    instr = make_op(OP_REAL_CONST);
                    instr->rd = r;
                    instr->real_const = 0;

                    code = seq(code, instr_to_code(instr));
                    instr = make_arith(r, OP_SUB, e->r);
                    code = seq(code, instr_to_code(instr));
                    break;
                default:
                    code = make_comment("internal error: uminus");
                    break;
            }
            break;
    }
    return code;
}

Code translate_stmts(char *id, int reg, Stmts ss) {
    Code code = NULL;
    while (ss) {
        code = seq(code, translate_stmt(id, reg, ss->s_first));
        ss = ss->s_rest;
    }

    return code;
}

Code translate_prologue(char *id, Params ps, Decls ds, int *size) {
    Code    c1 = NULL;
    Code    c2 = NULL;
    Decl    d;
    Param   p;
    Param   var;
    Instr   instr;

    // func label
    instr = make_op(OP_LABEL);
    sprintf(buffer, "func_%s", id);
    instr->string_const = checked_strdup(buffer);

    c1 = instr_to_code(instr);

    // Args
    while(ps) {
        p = ps->p_first;

        var = lookup_variable(id, p->id);
        var->pos = *size;
        
        sprintf(buffer, "argument %s is in stack slot %d", p->id, *size);
        c2 = seq(c2, make_comment(checked_strdup(buffer)));

        c2 = seq(c2, save_to(*size, *size));

        (*size)++;
        ps = ps->p_rest;
    }

    // Decs
    while(ds) {
        d = ds->d_first;

        var = lookup_variable(id, d->id);
        var->pos = *size;

        sprintf(buffer, "variable %s is in stack slot %d", d->id, (*size)++);
        c2 = seq(c2, make_comment(checked_strdup(buffer)));

        ds = ds->d_rest;
    }

    // push
    instr = make_op(OP_PUSH_STACK_FRAME);
    instr->int_const = *size;
    c1 = seq(c1, instr_to_code(instr));

    return seq(c1, c2);;
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

Code load_from(int r, int slot) {
    Instr instr = make_op(OP_LOAD);
    instr->rd = r;
    instr->int_const = slot;

    return instr_to_code(instr);
}

Code load_const(int r, Const c) {
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
            instr = make_op(OP_BOOL_CONST);
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

Code make_branch(Opcode op, const char *str, int r) {
    Instr instr = make_op(op);
    instr->string_const = checked_strdup(str);
    instr->rs1 = r;
    return instr_to_code(instr);
}

Code make_comment(const char *str) {
    Instr instr = make_op(OP_COMMENT);
    instr->string_const = str;
    return instr_to_code(instr);
}

Code make_label(const char *str) {
    Instr instr = make_op(OP_LABEL);
    instr->string_const = checked_strdup(str);
    return instr_to_code(instr);
}

Instr make_arith(int r, Opcode op, Type t) {
    Instr instr;

    instr = make_op(op);
    instr->suffix = suff(t);
    instr->rs2 = r + 1;
    instr->rs1 = r;
    instr->rd = r;

    return instr;
}

const char *suff(Type t) {
    switch (t) {
        case TYPE_INT: return "int";
        case TYPE_REAL: return "real";
        case TYPE_BOOL: return "bool";
        case TYPE_STRING: return "string";
        case TYPE_ERROR: return "error";
    }
    return "error";
}
