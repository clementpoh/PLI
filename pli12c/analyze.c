/*
** vim: ts=4 sw=4 expandtab
*/
/*
** This module contains the semantic analyzer for PLI12 programs.
*/

#include    <stdio.h>

#include    "pli12c.h"
#include    "ast.h"
#include    "analyze.h"
#include    "symbol.h"

static void pop_sym_table(Funcs prog_funcs);

static void init_declarations(Funcs fs);
static Func init_declaration(Func f);

static void verify_types(Funcs fs);
static void verify_statement(char *id, Stmt s);
static void verify_expression(char *id, Expr e);

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
    Stmts stmts;
    char *id;

    while(fs) {
        id = fs->f_first->id;
        stmts = fs->f_first->stmts;
        while(stmts) {
            verify_statement(id, stmts->s_first);
            stmts = stmts->s_rest;
        }
        fs = fs->f_rest;
    }
}

static void verify_statement(char *id, Stmt s) {
    switch(s->t) {
        case STMT_ASSIGN:
            verify_expression(id, s->s.Uassign.expr);
            if (get_var_type(id, s->s.Uassign.id) == s->s.Uassign.expr->r) {
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
            break;
        case STMT_ELSE:
            break;
        case STMT_WHILE:
            break;
        case STMT_RETURN:
            break;
    }
}

static void verify_expression(char *id, Expr e) {
    switch(e->t) {
        case EXPR_ID:
            e->r = get_var_type(id, e->e.Uid);
            break;
        case EXPR_CONST:
            e->r = e->e.Uconst->type;
            break;
        case EXPR_BINOP:
            verify_expression(id, e->e.Ubinop.e1);
            verify_expression(id, e->e.Ubinop.e2);
            break;
        case EXPR_UNOP:
            verify_expression(id, e->e.Uunop.e);
            break;
        case EXPR_FUNC:
            e->r = get_func_type(e->e.Ucall.id);
            /* TODO: Handle the argument expressions.
             * Going to double recurse down the
             * expression list and the parameter list.
             */
            break;
    }
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
