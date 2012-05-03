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

Funcs
analyze_prog(Funcs prog_funcs)
{
    Funcs funcs = prog_funcs;
    pop_sym_table(funcs);
    init_declarations(funcs);
    return funcs;
}

static void pop_sym_table(Funcs prog_funcs) {
    Funcs func = prog_funcs;
    while(func) {
        add_user_function(func->f_first);
        func = func->f_rest;
    }
}

static void init_declarations(Funcs fs) {
    while(fs) {
        fs->f_first = init_declaration(fs->f_first);
        fs = fs->f_rest;
    }
}

static Func init_declaration(Func f) {
    Decl    d;
    Decls   decls = f->decls;
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
