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
static void pretty_binop(BinOp op);
static void pretty_unop(UnOp op);
static void pretty_type(Type t);
static void pretty_const(Const c);
static void pretty_call(Exprs es);

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
    printf("end\n\n");
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
    switch(s->t) {
        case STMT_ASSIGN:
            printf("%s :=", s->s.Uassign.id);
            pretty_expr(s->s.Uassign.expr);
            printf(";\n");
            break;
        case STMT_READ:
            printf("read %s;\n", s->s.Uread);
            break;
        case STMT_WRITE:
            printf("write ");
            pretty_expr(s->s.Uwrite);
            printf(";\n");
            break;
        case STMT_IF:
            printf("if ");
            pretty_expr(s->s.Uif.cond);
            printf(" then\n     ");
            pretty_stmts(s->s.Uif.then);
            printf("    endif\n");
            break;
        case STMT_ELSE:
            printf("if");
            pretty_expr(s->s.Uelse.cond);
            printf(" then\n    ");
            pretty_stmts(s->s.Uelse.then);
            printf("    else\n    ");
            pretty_stmts(s->s.Uelse.other);
            printf("    endif\n");
            break;
        case STMT_WHILE:
            printf("while ");
            pretty_expr(s->s.Uwhile.cond);
            printf(" do\n    ");
            pretty_stmts(s->s.Uwhile.rep);
            printf("    endwhile\n");
            break;
        case STMT_RETURN:
            printf("return ");
            pretty_expr(s->s.Ureturn);
            printf(";\n");
            break;
        default:
            printf("Error: %d\n", s->t);
    }
}

static void pretty_expr(Expr e) {
    switch(e->t) {
        case EXPR_ID:
            printf("%s", e->e.Uid);
            break;
        case EXPR_CONST:
            pretty_const(e->e.Uconst);
            break;
        case EXPR_BINOP:
            pretty_expr(e->e.Ubinop.e1);
            pretty_binop(e->e.Ubinop.op);
            pretty_expr(e->e.Ubinop.e2);
            break;
        case EXPR_UNOP:
            pretty_unop(e->e.Uunop.op);
            pretty_expr(e->e.Uunop.e);
            break;
        case EXPR_FUNC:
            printf("%s", e->e.Ucall.id);
            pretty_call(e->e.Ucall.args);
            break;
    }
}

static void pretty_type(Type t) {
    switch(t) {
        case TYPE_INT:  printf("int");      break;
        case TYPE_REAL: printf("real");     break;
        case TYPE_BOOL: printf("bool");     break;
        case TYPE_STRING: printf("string"); break;
    }
}

static void pretty_unop(UnOp op) {
    switch(op) {
        case UNOP_NOT: 
            printf(" not ");
            break;
        case UNOP_UMINUS: 
            printf(" -");
            break;
        default:
            printf("ERROR: Operator not found");
    }
}

static void pretty_binop(BinOp op) {
    switch(op) {
        case BINOP_OR: 
            printf(" or "); break;
        case BINOP_AND:
            printf(" and "); break;
        case BINOP_EQ:
            printf(" = "); break;
        case BINOP_NE:
            printf(" != "); break;
        case BINOP_LT:
            printf(" < "); break;
        case BINOP_LE:
            printf(" <= "); break;
        case BINOP_GT:
            printf(" > "); break;
        case BINOP_GE:
            printf(" >= "); break;
        case BINOP_ADD:
            printf(" + "); break;
        case BINOP_SUB:
            printf(" - "); break;
        case BINOP_MUL:
            printf(" * "); break;
        case BINOP_DIV:
            printf(" / "); break;
        default:
            printf("ERROR: Operator not found");
    }
}

static void pretty_const(Const c) {
    switch(c->type) {
        case TYPE_INT: 
            printf("%d", c->val.Uint);
            break;
        case TYPE_REAL: 
            printf("%f", c->val.Ureal);
            break;
        case TYPE_BOOL: 
            printf("%d", c->val.Ubool);
            break;
        case TYPE_STRING: 
            printf("%s", c->val.Ustr);
            break;
    }
}

static void pretty_call(Exprs es) {
    printf("(");
    while(es->e_rest) {
        pretty_expr(es->e_first);
        printf(",");
        es = es->e_rest;
    }
    pretty_expr(es->e_first);
    printf(")");
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
        printf("    ");
        pretty_decl(ds->d_first);
        ds = ds->d_rest;
    }
}

static void pretty_stmts(Stmts ss) {
    while(ss) {
        printf("    ");
        pretty_stmt(ss->s_first);
        ss = ss->s_rest;
    }
}

static void pretty_exprs(Exprs es) {
    while(es) {
        pretty_expr(es->e_first);
        es = es->e_rest;
    }
}
