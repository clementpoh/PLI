/*
** vim: ts=4 sw=4 expandtab
*/
/*
** This module contains the prettyprinter for PLI12 programs.
*/

#include    <stdio.h>
#include    "ast.h"
#include    "pretty.h"

static void pretty_func(Func f);
static void pretty_param(Param p);
static void pretty_decls(Decl d);
static void pretty_stmt(Stmt s);
static void pretty_expr(Expr e);
const char *pretty_type(Type t);

/* Lists */
static void pretty_funcs(Funcs fs);
static void pretty_params(Params ps);
static void pretty_decls(Decls ds);
static void pretty_stmt(Stmts ss);
static void pretty_expr(Exprs es);

void
pretty_prog(FILE *fp, Funcs prog_funcs)
{
    pretty_funcs(prog_funcs);    
}

void pretty_func(Func f) {
    printf("function %s" f->id);
    pretty_params(f->args);
    pretty_type(f->type);
    pretty_decls(f->decls);
}

void pretty_param(Param p) {
    printf("%s: %s", p->id, pretty_type(p->type));
}

const char *pretty_type(Type t) {
    switch(t) {
        case TYPE_INT:      return "int";
        case TYPE_REAL:     return "real";
        case TYPE_BOOL:     return "bool";
        case TYPE_STRING:   return "string";
    }
}


/* Lists */
void pretty_funcs(Funcs fs) {
    if(fs) {
        pretty_func(fs->f_first);
        pretty_funcs(fs->f_rest);
    }
}

void pretty_args(Params ps) {
    printf("(");
    if(ps) { pretty_params(ps); }
    printf(")");
}

void pretty_params(Param ps) {


}

void pretty_decls(Decls ds) {
    if(ds) {
        pretty_func(ds->d_first);
        pretty_funcs(ds->d_rest);
    }
}

void pretty_stmt(Stmts ss) {
    if(ss) {
        pretty_func(ss->s_first);
        pretty_funcs(ss->s_rest);
    }
}
void pretty_expr(Exprs es) {
    if(es) {
        pretty_func(es->e_first);
        pretty_funcs(es->e_rest);
    }
}
