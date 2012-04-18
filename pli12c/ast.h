/*
** vim: ts=4 sw=4 expandtab
*/
/*
** This module should define an abstract syntax tree for PLI12 programs.
*/

#ifndef AST_H
#define AST_H

#include    "std.h"

typedef struct s_const  *Const;
typedef struct s_expr   *Expr;
typedef struct s_decl   *Decl;
typedef struct s_stmt   *Stmt;
typedef struct s_param  *Param;
typedef struct s_func   *Func;

typedef struct s_types  *Types;
typedef struct s_exprs  *Exprs;
typedef struct s_decls  *Decls;
typedef struct s_stmts  *Stmts;
typedef struct s_params *Params;
typedef struct s_funcs  *Funcs;

/* This is for you to fill in */

typedef enum {
/* This is for you to fill in */
} Type;

struct s_const {
/* This is for you to fill in */
};

typedef enum {
    BINOP_OR, BINOP_AND,
    BINOP_EQ, BINOP_NE, BINOP_LT, BINOP_LE, BINOP_GT, BINOP_GE,
    BINOP_ADD, BINOP_SUB, BINOP_MUL, BINOP_DIV
} BinOp;

typedef enum {
    UNOP_NOT, UNOP_UMINUS, UNOP_INT_TO_REAL
} UnOp;

typedef enum {
    BUILTIN, USER_DEFINED
} Status;

struct s_expr {
/* This is for you to fill in */
};

struct s_decl {
/* This is for you to fill in */
};

struct s_stmt {
/* This is for you to fill in */
};

struct s_param {
/* This is for you to fill in */
};

struct s_func {
/* This is for you to fill in */
};

struct s_types {
    Type    t_first;
    Types   t_rest;
};

struct s_exprs {
    Expr    e_first;
    Exprs   e_rest;
};

struct s_decls {
    Decl    d_first;
    Decls   d_rest;
};

struct s_stmts {
    Stmt    s_first;
    Stmts   s_rest;
};

struct s_params {
    Param   p_first;
    Params  p_rest;
};

struct s_funcs {
    Func    f_first;
    Funcs   f_rest;
};

extern  Expr    make_binop(BinOp binop, int lineno, Expr e1, Expr e2);
extern  Expr    make_unop(UnOp unop, int lineno, Expr e1);

extern  Expr    convert_int_to_real(Expr expr);

/* This is for you to fill in */

#endif  /* AST_H */
