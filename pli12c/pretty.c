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
static void pretty_decl(Decl d);
static void pretty_stmt(Stmt s);
static void pretty_expr(Expr e);
static void pretty_type(Type t);

/* Lists */
static void pretty_funcs(Funcs fs);
static void pretty_args(Params ps);
static void pretty_params(Params ps);
static void pretty_decls(Decls ds);
static void pretty_stmts(Stmts ss);
static void pretty_exprs(Exprs es);

void
pretty_prog(FILE *fp, Funcs prog_funcs)
{
    pretty_funcs(prog_funcs);    
}

static void pretty_func(Func f) {
    printf("function %s", f->id);
    pretty_args(f->args);
    printf(" returns ");
    pretty_type(f->type);
    printf("\nbegin\n");
    pretty_decls(f->decls);
    printf("\n");
    pretty_stmts(f->stmts);
    printf("\nend\n");
}

static void pretty_param(Param p) {
    printf("%s: ", p->id);
    pretty_type(p->type);
}

static void pretty_decl(Decl d) {
    printf("declare %s ", d->id);
    pretty_type(d->type);
    printf(";\n");
}

static void pretty_stmt(Stmt s) {

}

static void pretty_expr(Expr e) {

}

static void pretty_type(Type t) {
    switch(t) {
        case TYPE_INT: printf("int"); break;
        case TYPE_REAL: printf("real"); break;
        case TYPE_BOOL: printf("bool"); break;
        case TYPE_STRING: printf("string"); break;
    }
}


/* Lists */
static void pretty_funcs(Funcs fs) {
    if(fs) {
        pretty_func(fs->f_first);
        pretty_funcs(fs->f_rest);
    }
}

static void pretty_args(Params ps) {
    printf("(");
    if(ps) {
        pretty_param(ps->p_first);
        pretty_params(ps);
    }
    printf(")");
}

static void pretty_params(Params ps) {
    while(ps) {
        printf(", ");
        pretty_param(ps->p_first);
        ps = ps->p_rest;
    }
}

static void pretty_decls(Decls ds) {
    while(ds) {
        pretty_decl(ds->d_first);
        ds = ds->d_rest;
    }
}

static void pretty_stmts(Stmts ss) {
}

static void pretty_exprs(Exprs es) {
}
