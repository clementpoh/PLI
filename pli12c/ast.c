#include <stdlib.h>

#include "pli12c.h"
#include "ast.h"

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
    new->val.Uint = s;
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

    new->d_first = p;
    new->d_rest = ps;
    return new;
}

Stmts   ins_stmt(Stmt s, Stms ss) {
    Stmts new = checked_malloc(sizeof(Stmts));

    new->s_first = p;
    new->s_rest = ps;
    return new;
}

Exprs   ins_expr(Expr e, Exprs es) {
    Exprs new = checked_malloc(sizeof(Exprs));

    new->e_first = p;
    new->e_rest = ps;
    return new;
}
