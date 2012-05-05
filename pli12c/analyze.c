/*
** vim: ts=4 sw=4 expandtab foldmethod=indent
*/
/*
** This module contains the semantic analyzer for PLI12 programs.
*/

#include    <stdio.h>

#include    "pli12c.h"
#include    "ast.h"
#include    "analyze.h"
#include    "symbol.h"

static void verify_types(Funcs fs);

static void verify_statements(char *id, Stmts ss);
static void verify_statement(char *id, Stmt s);

static Type verify_expression(char *id, Expr e);

static Type verify_unop(char *id, Expr e);
static Type verify_binop(char *id, Expr e);
static Type verify_call(char *id, Expr call);

static bool is_type(int line, Type exp, Type got);

static void pop_sym_table(Funcs prog_funcs);

static void init_declarations(Funcs fs);
static Func init_declaration(Func f);


Funcs
analyze_prog(Funcs prog_funcs)
{
    pop_sym_table(prog_funcs);
    init_declarations(prog_funcs);
    verify_types(prog_funcs);
    return prog_funcs;
}

/* Must be run after declarations have been converted to assignment
 * statements */
static void verify_types(Funcs fs) {
    Stmts ss;
    char *id;

    while(fs) {
        id = fs->f_first->id;
        ss = fs->f_first->stmts;
        verify_statements(id, ss);
        fs = fs->f_rest;
    }
}

static void verify_statements(char *id, Stmts ss) {
    while(ss) {
        verify_statement(id, ss->s_first);
        ss = ss->s_rest;
    }
}

static void verify_statement(char *id, Stmt s) {
    Type t;
    switch(s->t) {
        case STMT_ASSIGN:
            t = verify_expression(id, s->s.Uassign.expr);
            if (t == get_var_type(id, s->s.Uassign.id)) {
                record_error(s->lineno, "W00t they match!");
            }
            break;
        case STMT_READ:
            record_error(s->lineno, "Read");
            break;
        case STMT_WRITE:
            record_error(s->lineno, "Write");
            break;
        case STMT_IF:
            t = verify_expression(id, s->s.Uif.cond);
            is_type(s->lineno, TYPE_BOOL, t);
            verify_statements(id, s->s.Uif.then);
            break;
        case STMT_ELSE:
            t = verify_expression(id, s->s.Uelse.cond);
            is_type(s->lineno, TYPE_BOOL, t);
            verify_statements(id, s->s.Uelse.then);
            verify_statements(id, s->s.Uelse.other);
            break;
        case STMT_WHILE:
            t = verify_expression(id, s->s.Uwhile.cond);
            is_type(s->lineno, TYPE_BOOL, t);
            verify_statements(id, s->s.Uwhile.rep);
            break;
        case STMT_RETURN:
            t = verify_expression(id, s->s.Ureturn);
            is_type(s->lineno, get_func_type(id), t);
            break;
    }
}

static Type verify_expression(char *id, Expr e) {
    switch(e->t) {
        case EXPR_ID:
            e->r = get_var_type(id, e->e.Uid);
            break;
        case EXPR_CONST:
            e->r = e->e.Uconst->type;
            break;
        case EXPR_BINOP:
            e->r = verify_binop(id, e);
            break;
        case EXPR_UNOP:
            e->r = verify_unop(id, e);
            break;
        case EXPR_FUNC:
            e->r = verify_call(id, e);
            break;
    }

    return e->r;
}

static Type verify_binop(char *id, Expr e) {
    Type t1 = verify_expression(id, e->e.Ubinop.e1);
    Type t2 = verify_expression(id, e->e.Ubinop.e2);
    switch (e->e.Ubinop.op) {
        case BINOP_OR:
        case BINOP_AND:
            return (is_type(e->lineno, TYPE_BOOL, t1)
                && is_type(e->lineno, TYPE_BOOL, t2))
                ? TYPE_BOOL
                : TYPE_ERROR;
            break;
        case BINOP_EQ:
        case BINOP_NE:
        case BINOP_LT:
        case BINOP_LE:
        case BINOP_GT:
        case BINOP_GE:
        case BINOP_ADD:
        case BINOP_SUB:
        case BINOP_MUL:
        case BINOP_DIV:
            if (t1 == t2) {
                return t1;
            } else if (t1 == TYPE_INT && t2 == TYPE_REAL) {
                pli12yylinenum = e->lineno;
                e->e.Ubinop.e1 = make_unop(UNOP_INT_TO_REAL, e->e.Ubinop.e1);
                return TYPE_REAL;
            } else if (t2 == TYPE_INT && t1 == TYPE_REAL) {
                pli12yylinenum = e->lineno;
                e->e.Ubinop.e2 = make_unop(UNOP_INT_TO_REAL, e->e.Ubinop.e2);
                return TYPE_REAL;
            } else {
                return is_type(e->lineno, t1, t2);
            }
            break;
    }

}

static Type verify_unop(char *id, Expr e) {
    Type t = verify_expression(id, e->e.Uunop.e);
    switch (e->e.Uunop.op) {
        case UNOP_NOT:
            return is_type(e->lineno, TYPE_BOOL, t) ? TYPE_BOOL : TYPE_ERROR;
            break;
        case UNOP_UMINUS:
        case UNOP_INT_TO_REAL:
            return t;
    }
}

/* TODO: Change the error message */
static Type verify_call(char *id, Expr call) {
    Fsym f = lookup_function(call->e.Ucall.id);
    Exprs as = call->e.Ucall.args;
    Types ts = f->args;
    Type t;

    if (f) {
        while(as || ts) {
            if (as && ts) {
                t = verify_expression(id, as->e_first);
                is_type(call->lineno, ts->t_first, t);
            } else if (as) {
                record_error(call->lineno, "Too many args");
            } else {
                record_error(call->lineno, "Too few args");
            }

            ts = (ts) ? ts->t_rest : NULL;
            as = (as) ? as->e_rest : NULL;
        }
        return f->ret;
    } else {
        return TYPE_ERROR;
    }
}

static bool is_type(int line, Type exp, Type got) {
    if(exp != got) {
        record_error(line, "Expected %s got %s");
        return FALSE;
    }
    return TRUE;
}


static void pop_sym_table(Funcs fs) {
    while(fs) {
        add_user_function(fs->f_first);
        fs = fs->f_rest;
    }
}

static void init_declarations(Funcs fs) {
    while(fs) {
        fs->f_first = init_declaration(fs->f_first);
        fs = fs->f_rest;
    }
}

static Func init_declaration(Func f) {
    Decls   decls = f->decls;
    Decl    d;
    Stmt    s;
    while(decls) {
        d = decls->d_first;
        if(d->val) {
            pli12yylinenum = d->lineno;
            s = make_assign(d->id,make_const(d->val));
            f->stmts = ins_stmt(s, f->stmts);
        }
        decls = decls->d_rest;
    }
    return f;
}
