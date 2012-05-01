/* ast.c
 * Clement Poh (cjpoh) 269508
 *
 * Repository Location:
 * /home/stude1/c/cjpoh/90045/
 * Git repository.
 *
 * vim: ts=4 sw=4 expandtab foldmethod=indent
*/

#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "pli12c.h"
#include "ast.h"

Func    make_func(char *id, Params ps, Type t, Decls ds, Stmts ss) {
    Func new = checked_malloc(sizeof(*new));

    new->id = id;
    new->args = ps;
    new->type = t;
    new->decls = ds;
    new->stmts = ss;

    new->lineno = pli12yylinenum;

    return new;
}

Param   make_param(char *id, Type t) {
    Param new = checked_malloc(sizeof(*new));

    new->id = id;
    new->type = t;

    new->lineno = pli12yylinenum;

    return new;
}

Decl    make_decl(char *id, Type t, Const val) {
    Decl new = checked_malloc(sizeof(*new));

    new->id = id;
    new->type = t;
    new->val = val;

    new->lineno = pli12yylinenum;

    return new;
}

Stmt    make_assign(char *id, Expr e) {
    Stmt new = checked_malloc(sizeof(*new));

    new->t = STMT_ASSIGN;
    new->s.Uassign.id = id;
    new->s.Uassign.expr = e;

    new->lineno = pli12yylinenum;

    return new;
}

Stmt    make_read(char *id) {
    Stmt new = checked_malloc(sizeof(*new));

    new->t = STMT_READ;
    new->s.Uread = id;

    new->lineno = pli12yylinenum;

    return new;
}

Stmt    make_write(Expr e) {
    Stmt new = checked_malloc(sizeof(*new));

    new->t = STMT_WRITE;
    new->s.Uwrite = e;

    new->lineno = pli12yylinenum;

    return new;
}

Stmt    make_if(Expr cond, Stmts then) {
    Stmt new = checked_malloc(sizeof(*new));

    new->t = STMT_IF;
    new->s.Uif.cond = cond;
    new->s.Uif.then = then;

    new->lineno = pli12yylinenum;

    return new;
}

Stmt    make_else(Expr cond, Stmts then, Stmts other) {
    Stmt new = checked_malloc(sizeof(*new));

    new->t = STMT_ELSE;
    new->s.Uelse.cond = cond;
    new->s.Uelse.then = then;
    new->s.Uelse.other = other;

    new->lineno = pli12yylinenum;

    return new;
}

Stmt    make_while(Expr cond, Stmts rep) {
    Stmt new = checked_malloc(sizeof(*new));

    new->t = STMT_WHILE;
    new->s.Uwhile.cond = cond;
    new->s.Uwhile.rep = rep;

    new->lineno = pli12yylinenum;

    return new;
}

Stmt    make_return(Expr e) {
    Stmt new = checked_malloc(sizeof(*new));

    new->t = STMT_RETURN;
    new->s.Ureturn = e;

    new->lineno = pli12yylinenum;

    return new;
}

Expr    make_ident(char *id) {
    Expr new = checked_malloc(sizeof(*new));

    new->t = EXPR_ID;
    new->e.Uid = id;

    new->lineno = pli12yylinenum;

    return new;
}

Expr    make_const(Const c) {
    Expr new = checked_malloc(sizeof(*new));

    new->t = EXPR_CONST;
    new->e.Uconst = c;

    new->lineno = pli12yylinenum;

    return new;
}

Expr    make_binop(BinOp binop, Expr e1, Expr e2) {
    Expr new = checked_malloc(sizeof(*new));

    /* TODO: Work out the type of the returned expression. */
    new->t = EXPR_BINOP;
    new->e.Ubinop.op = binop;
    new->e.Ubinop.e1 = e1;
    new->e.Ubinop.e2 = e2;

    new->lineno = pli12yylinenum;

    return new;
}

Expr    make_unop(UnOp unop, Expr e) {
    Expr new = checked_malloc(sizeof(*new));

    /* TODO: Work out the type of the returned expression. */
    new->t = EXPR_UNOP;
    new->e.Uunop.op = unop;
    new->e.Uunop.e = e;

    new->lineno = pli12yylinenum;

    return new;
}

Expr    make_call(char *id, Exprs args) {
    Expr new = checked_malloc(sizeof(*new));

    /* TODO: Work out the type of the returned expression. */
    new->t = EXPR_FUNC;
    new->e.Ucall.id = id;
    new->e.Ucall.args = args;

    new->lineno = pli12yylinenum;

    return new;
}

Const   make_int(int i) {
    Const new = checked_malloc(sizeof(*new));

    new->type = TYPE_INT;
    new->val.Uint = i;
    return new;
}

Const   make_real(float r) {
    Const new = checked_malloc(sizeof(*new));

    new->type = TYPE_REAL;
    new->val.Ureal = r;
    return new;
}

Const   make_bool(bool b) {
    Const new = checked_malloc(sizeof(*new));

    new->type = TYPE_BOOL;
    new->val.Ubool = b;
    return new;
}

Const   make_str(char *s) {
    Const new = checked_malloc(sizeof(*new));

    new->type = TYPE_STRING;
    new->val.Ustr = s;
    return new;
}


Funcs  ins_func(Func f, Funcs fs) {
    Funcs new;

    if(!fs || strcmp(f->id, fs->f_first->id) < 0) {
        new = checked_malloc(sizeof(*new));
        new->f_first = f;
        new->f_rest = fs;
        return new;
    } else {
        fs->f_rest = ins_func(f, fs->f_rest);
        return fs;
    }
}

Params  ins_param(Param p, Params ps) {
    Params new = checked_malloc(sizeof(*new));

    new->p_first = p;
    new->p_rest = ps;
    return new;
}

Decls   ins_decl(Decl d, Decls ds) {
    Decls new = checked_malloc(sizeof(*new));

    new->d_first = d;
    new->d_rest = ds;
    return new;
}

Stmts   ins_stmt(Stmt s, Stmts ss) {
    Stmts new = checked_malloc(sizeof(*new));

    new->s_first = s;
    new->s_rest = ss;
    return new;
}

Exprs   ins_expr(Expr e, Exprs es) {
    Exprs new = checked_malloc(sizeof(*new));

    new->e_first = e;
    new->e_rest = es;
    return new;
}


Types   ins_type(Type t, Types ts) {
    Types new = checked_malloc(sizeof(*new));

    new->t_first = t;
    new->t_rest = ts;
    return new;
}
