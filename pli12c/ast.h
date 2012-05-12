/*
** vim: ts=4 sw=4 expandtab foldmethod=indent
*/
/*
** This module should define an abstract syntax tree for PLI12 programs.
** I want Haskell's algebraic data types. C sucks.
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

/* Statements */
typedef struct s_assign Assign;
typedef struct s_if     If;
typedef struct s_else   Else;
typedef struct s_while  While;

/* Expressions */
typedef struct s_binop  Binop;
typedef struct s_unop   Unop;
typedef struct s_call   Call;

/* Lists */
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
    TYPE_ERROR = FALSE,
    TYPE_INT,
    TYPE_REAL,
    TYPE_BOOL,
    TYPE_STRING
} Type;

typedef enum {
    BINOP_OR, BINOP_AND,
    BINOP_EQ, BINOP_NE, BINOP_LT, BINOP_LE, BINOP_GT, BINOP_GE,
    BINOP_ADD, BINOP_SUB, BINOP_MUL, BINOP_DIV
} BinOp;

struct s_const {
    Type type;
    Val val;
};

typedef enum {
    UNOP_NOT, UNOP_UMINUS, UNOP_INT_TO_REAL
} UnOp;

typedef enum {
    BUILTIN, USER_DEFINED
} Status;

struct s_decl {
    char    *id;
    Type    type;
    Const   val;   

    int lineno;
};

struct s_param {
    char    *id;
    Type    type;
    int     pos;

    int lineno;
};

struct s_func {
    char    *id;
    Params  args;
    Type    type;
    Decls   decls;
    Stmts   stmts;

    int lineno;
};

/* Expressions */
typedef enum {
    EXPR_ID,
    EXPR_CONST,
    EXPR_BINOP,
    EXPR_UNOP,
    EXPR_FUNC
} ExprType;

struct s_binop {
    BinOp   op;
    Expr    e1;
    Expr    e2;
};

struct s_unop {
    UnOp    op;
    Expr    e;
};

struct s_call {
    char    *id;
    Exprs   args;
};

typedef union {
    char    *Uid;
    Const   Uconst;
    Binop   Ubinop;
    Unop    Uunop;
    Call    Ucall;
} Expression;

struct s_expr {
    Type r;
    ExprType t;
    Expression e;

    int lineno;
};


/* Statements */
typedef enum {
    STMT_ASSIGN,
    STMT_READ,
    STMT_WRITE,
    STMT_IF,
    STMT_ELSE,
    STMT_WHILE,
    STMT_RETURN
} StateType;

struct s_assign {
    char *id;
    Expr expr;
};

struct s_if {
    Expr    cond;
    Stmts   then;
};

struct s_else {
    Expr    cond;
    Stmts   then;
    Stmts   other;
};

struct s_while {
    Expr    cond;
    Stmts   rep;
};

typedef union {
    Assign  Uassign;
    char    *Uread;
    Expr    Uwrite;
    If      Uif;
    Else    Uelse;
    While   Uwhile;
    Expr    Ureturn;
} Statement;

struct s_stmt {
    StateType t;
    Statement s;

    int lineno;
};


/* Lists */
struct s_funcs {
    Func    f_first;
    Funcs   f_rest;
};

struct s_params {
    Param   p_first;
    Params  p_rest;
};

struct s_types {
    Type    t_first;
    Types   t_rest;
};

struct s_decls {
    Decl    d_first;
    Decls   d_rest;
};

struct s_exprs {
    Expr    e_first;
    Exprs   e_rest;
};

struct s_stmts {
    Stmt    s_first;
    Stmts   s_rest;
};

extern  Func    make_func
                    ( int lineno, char *id, Params ps
                    , Type t , Decls ds , Stmts stmts); 

extern  Param   make_param(char *id, Type t); 
extern  Decl    make_decl(char *id, Type t, Const val);

extern  Stmt    make_assign(char *id, Expr e);
extern  Stmt    make_read(char *id);
extern  Stmt    make_write(Expr e);
extern  Stmt    make_if(Expr cond, Stmts then);
extern  Stmt    make_else(Expr cond, Stmts then, Stmts other);
extern  Stmt    make_while(Expr cond, Stmts rep);
extern  Stmt    make_return(Expr e);

extern  Expr    make_ident(char *id);
extern  Expr    make_const(Const c);
extern  Expr    make_binop(BinOp binop, Expr e1, Expr e2);
extern  Expr    make_unop(UnOp unop, Expr e);
extern  Expr    make_call(char *id, Exprs args);

extern  Expr    convert_int_to_real(Expr expr);

extern  Const   make_int(int i);
extern  Const   make_real(float r);
extern  Const   make_bool(bool b);
extern  Const   make_str(char *s);

extern  Funcs   ins_func(Func f, Funcs fs);
extern  Params  ins_param(Param p, Params ps);
extern  Decls   ins_decl(Decl d, Decls ds);
extern  Stmts   ins_stmt(Stmt s, Stmts ss);
extern  Exprs   ins_expr(Expr e, Exprs es);
extern  Types   ins_type(Type t, Types ts);

const char *type_to_str(Type t);
const char *binop_to_str(BinOp op);

#endif  /* AST_H */
