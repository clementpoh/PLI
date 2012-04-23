/*
** vim: ts=4 sw=4 expandtab
*/
/*
** This module should define an abstract syntax tree for PLI12 programs.
** I want Haskell's discriminating unionised types. C sucks.
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

typedef struct s_assign *Assign;
typedef struct s_if     *If;
typedef struct s_else   *Else;
typedef struct s_while  *While;

typedef struct s_types  *Types;
typedef struct s_exprs  *Exprs;
typedef struct s_decls  *Decls;
typedef struct s_stmts  *Stmts;
typedef struct s_params *Params;
typedef struct s_funcs  *Funcs;

typedef union {
    char    *Ustr;
    int     Uint;
    bool    Ubool;
    float   Ureal;
} Val;

typedef enum {
    TYPE_INT,
    TYPE_REAL,
    TYPE_BOOL,
    TYPE_STRING
} Type;

typedef enum {
    TYPE_ASSIGN,
    TYPE_READ,
    TYPE_WRITE,
    TYPE_IF,
    TYPE_ELSE,
    TYPE_WHILE,
    TYPE_RETURN
} StateType;

typedef union {
    Assign  Uassign;
    char    *Uread;
    Expr    Uwrite;
    If      Uif;
    Else    Uelse;
    While   Uwhile;
    Expr    Ureturn;
} Statement;

struct s_const {
    Type type;
    Val val;
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
    Type type;
    expr_t ex;
};

struct s_decl {
    char    *name;
    Type    type;
    Val     val;   
};

struct s_stmt {
    StateType t;
    Statement s;
};

struct s_param {
    char    *name;
    Type    type;
};

struct s_func {
    char    *name;
    Params  args;
    Type    type;
    Decls   decls;
    Stmts   stms;
};

struct s_assign {
    char *ident;
    Expr expr;
}

struct s_if {
    Expr    cond;
    Stmts   then;
}

struct s_else {
    Expr    cond;
    Stmts   then;
    Stmts   other;
}

struct s_while {
    Expr    cond;
    Stmts   rep;
}

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

extern  Func    make_func(char*, Params, Type, Decls, Stmts); 

extern  Const   make_int(int);
extern  Const   make_real(float);
extern  Const   make_bool(bool);
extern  Const   make_str(char*);

extern  Params  ins_param(Param p, Params ps);
extern  Decls   ins_decl(Decl d, Decls ds);
extern  Stmts   ins_stmt(Stmt s, Stms ss);
extern  Exprs   ins_expr(Expr e, Exprs es);

extern  Expr    make_binop(BinOp binop, int lineno, Expr e1, Expr e2);
extern  Expr    make_unop(UnOp unop, int lineno, Expr e1);

extern  Expr    convert_int_to_real(Expr expr);


#endif  /* AST_H */
