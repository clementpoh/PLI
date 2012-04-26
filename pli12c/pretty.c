/*
** vim: ts=4 sw=4 expandtab foldmethod=indent
*/
/*
** This module contains the prettyprinter for PLI12 programs.
*/

#include    <stdio.h>
#include    "ast.h"
#include    "pretty.h"

#define INDENT "    "

static void pretty_func(FILE *fp, Func f);
static void pretty_param(FILE *fp, Param p);
static void pretty_decl(FILE *fp, Decl d);
static void pretty_stmt(FILE *fp, Stmt s, int indent);
static void pretty_expr(FILE *fp, Expr e);
static void pretty_op_arg(FILE *fp, Expr e);
static void pretty_binop(FILE *fp, BinOp op);
static void pretty_unop(FILE *fp, UnOp op);
static void pretty_type(FILE *fp, Type t);
static void pretty_const(FILE *fp, Const c);
static void pretty_call(FILE *fp, Exprs es);


/* Lists */
static void pretty_funcs(FILE *fp, Funcs fs);
static void pretty_args(FILE *fp, Params ps);
static void pretty_params(FILE *fp, Params ps);
static void pretty_decls(FILE *fp, Decls ds);
static void pretty_stmts(FILE *fp, Stmts ss, int indent);
static void put_indent(FILE *fp, int indent);

void
pretty_prog(FILE *fp, Funcs prog_funcs)
{
    pretty_funcs(fp, prog_funcs);    
}

static void pretty_func(FILE *fp, Func f) {
    fprintf(fp, "function %s", f->id);
    pretty_args(fp, f->args);
    fprintf(fp, " returns ");
    pretty_type(fp, f->type);
    fprintf(fp, "\nbegin\n");
    pretty_decls(fp, f->decls);
    fprintf(fp, "\n");
    pretty_stmts(fp, f->stmts, 1);
    fprintf(fp, "end\n");
}

static void pretty_param(FILE *fp, Param p) {
    fprintf(fp, "%s: ", p->id);
    pretty_type(fp, p->type);
}

static void pretty_decl(FILE *fp, Decl d) {
    fprintf(fp, "declare %s ", d->id);
    pretty_type(fp, d->type);

    if(d->val) {
        fprintf(fp, " initialize to ");
        pretty_const(fp, d->val);
    }

    fprintf(fp, ";\n");
}

static void pretty_stmt(FILE *fp, Stmt s, int indent) {
    switch(s->t) {
        case STMT_ASSIGN:
            fprintf(fp, "%s := ", s->s.Uassign.id);
            pretty_expr(fp, s->s.Uassign.expr);
            fprintf(fp, ";\n");
            break;
        case STMT_READ:
            fprintf(fp, "read %s;\n", s->s.Uread);
            break;
        case STMT_WRITE:
            fprintf(fp, "write ");
            pretty_expr(fp, s->s.Uwrite);
            fprintf(fp, ";\n");
            break;
        case STMT_IF:
            fprintf(fp, "if ");
            pretty_expr(fp, s->s.Uif.cond);
            fprintf(fp, " then\n");
            pretty_stmts(fp, s->s.Uif.then, indent + 1);
            put_indent(fp, indent);
            fprintf(fp, "endif\n");
            break;
        case STMT_ELSE:
            fprintf(fp, "if ");
            pretty_expr(fp, s->s.Uelse.cond);
            fprintf(fp, " then\n");
            pretty_stmts(fp, s->s.Uelse.then, indent + 1);
            put_indent(fp, indent);
            fprintf(fp, "else\n");
            pretty_stmts(fp, s->s.Uelse.other, indent + 1);
            put_indent(fp, indent);
            fprintf(fp, "endif\n");
            break;
        case STMT_WHILE:
            fprintf(fp, "while ");
            pretty_expr(fp, s->s.Uwhile.cond);
            fprintf(fp, " do\n");
            pretty_stmts(fp, s->s.Uwhile.rep, indent + 1);
            put_indent(fp, indent);
            fprintf(fp, "endwhile\n");
            break;
        case STMT_RETURN:
            fprintf(fp, "return ");
            pretty_expr(fp, s->s.Ureturn);
            fprintf(fp, ";\n");
            break;
        default:
            fprintf(fp, "Error: %d\n", s->t);
    }
}

static void pretty_expr(FILE *fp, Expr e) {
    switch(e->t) {
        case EXPR_ID:
            fprintf(fp, "%s", e->e.Uid);
            break;
        case EXPR_CONST:
            pretty_const(fp, e->e.Uconst);
            break;
        case EXPR_BINOP:
            pretty_op_arg(fp, e->e.Ubinop.e1);
            pretty_binop(fp, e->e.Ubinop.op);
            pretty_op_arg(fp, e->e.Ubinop.e2);
            break;
        case EXPR_UNOP:
            pretty_unop(fp, e->e.Uunop.op);
            pretty_op_arg(fp, e->e.Uunop.e);
            break;
        case EXPR_FUNC:
            fprintf(fp, "%s", e->e.Ucall.id);
            pretty_call(fp, e->e.Ucall.args);
            break;
    }
}

static void pretty_op_arg(FILE *fp, Expr e) {
    switch(e->t) {
        case EXPR_BINOP:
        case EXPR_FUNC:
        case EXPR_UNOP:
            fprintf(fp, "(");
            pretty_expr(fp, e);
            fprintf(fp, ")");
            break;
        case EXPR_CONST:
        case EXPR_ID:
        default:
            pretty_expr(fp, e);
    }
}

static void pretty_type(FILE *fp, Type t) {
    switch(t) {
        case TYPE_INT:  fprintf(fp, "int");      break;
        case TYPE_REAL: fprintf(fp, "real");     break;
        case TYPE_BOOL: fprintf(fp, "bool");     break;
        case TYPE_STRING: fprintf(fp, "string"); break;
    }
}

static void pretty_unop(FILE *fp, UnOp op) {
    switch(op) {
        case UNOP_NOT: 
            fprintf(fp, "not ");
            break;
        case UNOP_UMINUS: 
            fprintf(fp, "- ");
            break;
        default:
            fprintf(fp, "ERROR: Operator not found");
    }
}

static void pretty_binop(FILE *fp, BinOp op) {
    switch(op) {
        case BINOP_OR: 
            fprintf(fp, " or "); break;
        case BINOP_AND:
            fprintf(fp, " and "); break;
        case BINOP_EQ:
            fprintf(fp, " = "); break;
        case BINOP_NE:
            fprintf(fp, " != "); break;
        case BINOP_LT:
            fprintf(fp, " < "); break;
        case BINOP_LE:
            fprintf(fp, " <= "); break;
        case BINOP_GT:
            fprintf(fp, " > "); break;
        case BINOP_GE:
            fprintf(fp, " >= "); break;
        case BINOP_ADD:
            fprintf(fp, " + "); break;
        case BINOP_SUB:
            fprintf(fp, " - "); break;
        case BINOP_MUL:
            fprintf(fp, " * "); break;
        case BINOP_DIV:
            fprintf(fp, " / "); break;
        default:
            fprintf(fp, "ERROR: Operator not found");
    }
}

static void pretty_const(FILE *fp, Const c) {
    switch(c->type) {
        case TYPE_INT: 
            fprintf(fp, "%d", c->val.Uint);
            break;
        case TYPE_REAL: 
            fprintf(fp, "%f", c->val.Ureal);
            break;
        case TYPE_BOOL: 
            fprintf(fp, "%s", c->val.Ubool? "true" : "false");
            break;
        case TYPE_STRING: 
            fprintf(fp, "%s", c->val.Ustr);
            break;
    }
}

static void put_indent(FILE *fp, int indent) {
    int i;
    for(i = 0; i < indent; i++) {
        fprintf(fp, INDENT);
    }
}

/* Lists */
static void pretty_funcs(FILE *fp, Funcs fs) {
    if(fs) {
        pretty_func(fp, fs->f_first);
        if(fs->f_rest) {
            fprintf(fp, "\n");
            pretty_funcs(fp, fs->f_rest);
        }
    }
}

static void pretty_args(FILE *fp, Params ps) {
    fprintf(fp, "(");
    if(ps) {
        pretty_param(fp, ps->p_first);
        pretty_params(fp, ps->p_rest);
    }
    fprintf(fp, ")");
}

static void pretty_params(FILE *fp, Params ps) {
    while(ps) {
        fprintf(fp, ", ");
        pretty_param(fp, ps->p_first);
        ps = ps->p_rest;
    }
}

static void pretty_decls(FILE *fp, Decls ds) {
    while(ds) {
        put_indent(fp, 1);
        pretty_decl(fp, ds->d_first);
        ds = ds->d_rest;
    }
}

static void pretty_stmts(FILE *fp, Stmts ss, int indent) {
    while(ss) {
        put_indent(fp, indent);
        pretty_stmt(fp, ss->s_first, indent);
        ss = ss->s_rest;
    }
}

static void pretty_call(FILE *fp, Exprs es) {
    fprintf(fp, "(");
    while(es->e_rest) {
        pretty_expr(fp, es->e_first);
        fprintf(fp, ", ");
        es = es->e_rest;
    }
    pretty_expr(fp, es->e_first);
    fprintf(fp, ")");
}

