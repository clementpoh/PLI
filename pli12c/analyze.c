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
            break;
        case STMT_READ:
            break;
        case STMT_WRITE:
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
            e->r = lookup_variable(id, e->e.Uid);
            break;
        case EXPR_CONST:
            e->r = e->e.Uconst->type;
            break;
        case EXPR_BINOP:
            break;
        case EXPR_UNOP:
            break;
        case EXPR_FUNC:
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
            s = make_assign(d->id,make_const(d->val));
            f->stmts = ins_stmt(s, f->stmts);
        }
        decls = decls->d_rest;
    }
    return f;
}
