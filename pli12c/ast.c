/*
 * vim: ts=4 sw=4 expandtab foldmethod=indent
*/

#include <stdlib.h>
#include <assert.h>

#include "pli12c.h"
#include "ast.h"

Func    make_func(char *id, Params ps, Type t, Decls ds, Stmts ss) {
    Func new = checked_malloc(sizeof(Func));

    new->id = id;
    new->args = ps;
    new->type = t;
    new->decls = ds;
    new->stmts = ss;

    return new;
}

Param   make_param(char *id, Type t) {
    Param new = checked_malloc(sizeof(Param));

    new->id = id;
    new->type = t;

    return new;
}

Decl    make_decl(char *id, Type t, Const init) {
    Decl new = checked_malloc(sizeof(Decl));
    assert(t == init->type);

    new->id = id;
    new->type = t;
    new->val = init->val;

    return new;
}

Stmt    make_assign(char *id, Expr e) {
    Stmt new = checked_malloc(sizeof(Stmt));

    new->t = TYPE_ASSIGN;
    new->s.Uassign.id = id;
    new->s.Uassign.expr = e;

    return new;
}

Stmt    make_read(char *id) P
    Stmt new = checked_malloc(sizeof(Stmt));

    new->t = TYPE_READ;
    new->s.Uread = id;

    return new;

}

Stmt    make_write(Expr e) {
    Stmt new = checked_malloc(sizeof(Stmt));

    new->t = TYPE_WRITE;
    new->s.Uwrite = e;

    return new;
}

Stmt    make_if(Expr cond, Stmts then) {
    Stmt new = checked_malloc(sizeof(Stmt));

    new->t = TYPE_IF;
    new->s.Uif.cond = cond;
    new->s.Uif.then = then;

    return new;
}

Stmt    make_else(Expr cond, Stmts then, Stmts other) {
    Stmt new = checked_malloc(sizeof(Stmt));

    new->t = TYPE_ELSE;
    new->s.Uelse.cond = cond;
    new->s.Uelse.then = then;
    new->s.Uelse.other = other;

    return new;
}

Stmt    make_while(Expr cond, Stmts rep) {
    Stmt new = checked_malloc(sizeof(Stmt));

    new->t = TYPE_WHILE;
    new->s.Uwhile.cond = cond;
    new->s.Uwhile.rep = rep;

    return new;
}

Stmt    make_return(Expr e) {
    Stmt new = checked_malloc(sizeof(Stmt));

    new->t = TYPE_RETURN;
    new->s.Ureturn = e;

    return new;
}

Expr    make_ident(char *id);
Expr    make_const(Const c);
Expr    make_binop(BinOp binop, int lineno, Expr e1, Expr e2);
Expr    make_unop(UnOp unop, int lineno, Expr e1);
Expr    make_func_call(char *id, Exprs args);

Const   make_int(int i) {
    Const new = checked_malloc(sizeof(Const));

    new->type = TYPE_INT;
    new->val.Uint = i;
    return new;
}

Const   make_real(float r) {
    Const new = checked_malloc(sizeof(Const));

    new->type = TYPE_REAL;
    new->val.Ureal = r;
    return new;
}

Const   make_bool(bool b) {
    Const new = checked_malloc(sizeof(Const));

    new->type = TYPE_BOOL;
    new->val.Ubool = b;
    return new;
}

Const   make_str(char *s) {
    Const new = checked_malloc(sizeof(Const));

    new->type = TYPE_STRING;
    new->val.Ustr = s;
    return new;
}


Funcs  ins_func(Func f, Funcs fs) {
    Funcs new = checked_malloc(sizeof(Funcs));

    new->f_first = f;
    new->f_rest = fs;
    return new;
}

Params  ins_param(Param p, Params ps) {
    Params new = checked_malloc(sizeof(Params));

    new->p_first = p;
    new->p_rest = ps;
    return new;
}

Decls   ins_decl(Decl d, Decls ds) {
    Decls new = checked_malloc(sizeof(Decls));

    new->d_first = d;
    new->d_rest = ds;
    return new;
}

Stmts   ins_stmt(Stmt s, Stmts ss) {
    Stmts new = checked_malloc(sizeof(Stmts));

    new->s_first = s;
    new->s_rest = ss;
    return new;
}

Exprs   ins_expr(Expr e, Exprs es) {
    Exprs new = checked_malloc(sizeof(Exprs));

    new->e_first = e;
    new->e_rest = es;
    return new;
}
