/*
** vim: ts=4 sw=4 expandtab foldmethod=indent
*/
/*
** This module contains the semantic analyzer for PLI12 programs.
*/

#include    <stdio.h>

#include    "pli12c.h"
#include    "ast.h"
#include    "analyze.h"
#include    "symbol.h"

static void verify_types(Funcs fs);

static void verify_statements(char *id, Stmts ss);
static void verify_statement(char *id, Stmt s);

static Type verify_expression(char *id, Expr e);

static Type verify_unop(char *id, Expr e);
static Type verify_binop(char *id, Expr e);
static Type verify_call(char *id, Expr call);

static bool is_type(int line, Type exp, Type got);
static void should_be_bool(StateType s, Type t, int line);

static void pop_sym_table(Funcs prog_funcs);

static void init_declarations(Funcs fs);
static Func init_declaration(Func f);


Funcs
analyze_prog(Funcs prog_funcs)
{
    Fsym f;
        
    pop_sym_table(prog_funcs);
    f = lookup_function("main");
    if (!f) {
        record_error(0, "there is no function named 'main'");
    } else {
        if (f->ret != TYPE_INT) {
            sprintf(err_buff, "return value of main has type %s; expected int"
                    , type_to_str(f->ret));
            record_error(f->lineno, err_buff);
        }
        if (f->args) {
            sprintf(err_buff, "main has arguments");
            record_error(f->lineno, err_buff);
        }

    }
    init_declarations(prog_funcs);
    verify_types(prog_funcs);
    
    return prog_funcs;
}

/* Must be run after declarations have been converted to assignment
 * statements */
static void verify_types(Funcs fs) {
    Stmts ss;
    char *id;

    while(fs) {
        id = fs->f_first->id;
        ss = fs->f_first->stmts;
        verify_statements(id, ss);
        fs = fs->f_rest;
    }
}

static void verify_statements(char *id, Stmts ss) {
    while(ss) {
        verify_statement(id, ss->s_first);
        ss = ss->s_rest;
    }
}

static void verify_statement(char *id, Stmt s) {
    int line = s->lineno;
    Param p;
    Type t1, t2;
    switch(s->t) {
        case STMT_READ:
            if(!lookup_variable(id, s->s.Uread)) {
                sprintf(err_buff,
                        "read into undefined variable '%s'"
                        , s->s.Uread);
                record_error(line, err_buff);
            }
            break;
        case STMT_WRITE:
            t1 = verify_expression(id, s->s.Uwrite);
            break;
        case STMT_ASSIGN:
            p = lookup_variable(id, s->s.Uassign.id);
            if (!p) {
                sprintf(err_buff,
                        "assignment to undefined variable '%s'"
                        , s->s.Uassign.id);
                record_error(line, err_buff);
            } 
            
            t2 = verify_expression(id, s->s.Uassign.expr);
            if (p && p->type == TYPE_REAL && t2 == TYPE_INT) {
                pli12yylinenum = s->lineno;
                s->s.Uassign.expr = make_unop(UNOP_INT_TO_REAL
                        , s->s.Uassign.expr);
            } else if (p && p->type && p->type != t2) {
                sprintf(err_buff,
                        "type mismatch in assignment to '%s': "
                        "assigning %s to %s" 
                        , s->s.Uassign.id
                        , type_to_str(t2)
                        , type_to_str(p->type)
                        );
                record_error(line, err_buff);
            }
            break;
        case STMT_IF:
            t1 = verify_expression(id, s->s.Uif.cond);
            should_be_bool(STMT_IF, t1, line);
            verify_statements(id, s->s.Uif.then);
            break;
        case STMT_ELSE:
            t1 = verify_expression(id, s->s.Uelse.cond);
            should_be_bool(STMT_ELSE, t1, line);
            verify_statements(id, s->s.Uelse.then);
            verify_statements(id, s->s.Uelse.other);
            break;
        case STMT_WHILE:
            t1 = verify_expression(id, s->s.Uwhile.cond);
            should_be_bool(STMT_WHILE, t1, line);
            verify_statements(id, s->s.Uwhile.rep);
            break;
        case STMT_RETURN:
            t1 = verify_expression(id, s->s.Ureturn);
            t2 = get_func_type(id);
            if (t1 == TYPE_INT && t2 == TYPE_REAL) {
                pli12yylinenum = s->lineno;
                s->s.Ureturn = make_unop(UNOP_INT_TO_REAL, s->s.Ureturn);
            } else if (t1 && t1 != t2) {
                sprintf(err_buff,
                        "type mismatch in return statement; "
                        "actual %s, expected %s" 
                        , type_to_str(t1), type_to_str(t2));
                record_error(line, err_buff);
            }
            break;
    }
}

static Type verify_expression(char *id, Expr e) {
    switch(e->t) {
        case EXPR_ID:
            e->r = get_var_type(e->lineno, id, e->e.Uid);
            break;
        case EXPR_CONST:
            e->r = e->e.Uconst->type;
            break;
        case EXPR_BINOP:
            e->r = verify_binop(id, e);
            break;
        case EXPR_UNOP:
            e->r = verify_unop(id, e);
            break;
        case EXPR_FUNC:
            e->r = verify_call(id, e);
            break;
    }

    return e->r;
}

static Type verify_binop(char *id, Expr e) {
    Type t1 = verify_expression(id, e->e.Ubinop.e1);
    Type t2 = verify_expression(id, e->e.Ubinop.e2);

    switch (e->e.Ubinop.op) {
        case BINOP_OR:
        case BINOP_AND:
            if (t1 && t1 != TYPE_BOOL) {
                sprintf(err_buff, "left operand of '%s' has type %s: "
                        "expected bool"
                        , binop_to_str(e->e.Ubinop.op), type_to_str(t1));
                record_error(e->lineno, err_buff);
            }

            if (t2 && t2 != TYPE_BOOL) {
                sprintf(err_buff, "right operand of '%s' has type %s: "
                        "expected bool"
                        , binop_to_str(e->e.Ubinop.op), type_to_str(t2));
                record_error(e->lineno, err_buff);
            }
            return TYPE_BOOL;
        case BINOP_EQ:
        case BINOP_NE:
        case BINOP_LT:
        case BINOP_LE:
        case BINOP_GT:
        case BINOP_GE:
            if (t1 == t2) {
                return TYPE_BOOL;
            } else if (t1 == TYPE_INT && t2 == TYPE_REAL) {
                pli12yylinenum = e->lineno;
                e->e.Ubinop.e1 = make_unop(UNOP_INT_TO_REAL, e->e.Ubinop.e1);
                return TYPE_BOOL;
            } else if (t2 == TYPE_INT && t1 == TYPE_REAL) {
                pli12yylinenum = e->lineno;
                e->e.Ubinop.e2 = make_unop(UNOP_INT_TO_REAL, e->e.Ubinop.e2);
                return TYPE_BOOL;
            } else {
                is_type(e->lineno, t1, t2);
            }
            break;
        case BINOP_ADD:
        case BINOP_SUB:
        case BINOP_MUL:
        case BINOP_DIV:
            if (t1 == t2 && (t1 == TYPE_INT || t1 == TYPE_REAL)) {
                return t1;
            } else if (t1 == TYPE_INT && t2 == TYPE_REAL) {
                pli12yylinenum = e->lineno;
                e->e.Ubinop.e1 = make_unop(UNOP_INT_TO_REAL, e->e.Ubinop.e1);
                return TYPE_REAL;
            } else if (t2 == TYPE_INT && t1 == TYPE_REAL) {
                pli12yylinenum = e->lineno;
                e->e.Ubinop.e2 = make_unop(UNOP_INT_TO_REAL, e->e.Ubinop.e2);
                return TYPE_REAL;
            } 

            if (t2 == TYPE_BOOL || t2 == TYPE_STRING) {
                sprintf(err_buff, "right operand of '%s' has type %s: "
                        "expected int or real"
                        , binop_to_str(e->e.Ubinop.op), type_to_str(t2));
                record_error(e->lineno, err_buff);
            }
            if (t1 == TYPE_BOOL || t1 == TYPE_STRING) {
                sprintf(err_buff, "left operand of '%s' has type %s: "
                        "expected int or real"
                        , binop_to_str(e->e.Ubinop.op), type_to_str(t1));
                record_error(e->lineno, err_buff);
            }
            break;
    }

    return TYPE_ERROR;
}

static Type verify_unop(char *id, Expr e) {
    Type t = verify_expression(id, e->e.Uunop.e);
    switch (e->e.Uunop.op) {
        case UNOP_NOT:
            return is_type(e->lineno, TYPE_BOOL, t) ? TYPE_BOOL : TYPE_ERROR;
            break;
        case UNOP_INT_TO_REAL:
            return TYPE_REAL;
        case UNOP_UMINUS:
            if (t == TYPE_BOOL || t == TYPE_STRING) {
                sprintf(err_buff, "operand of '-' has type %s: "
                        "expected int or real"
                        , type_to_str(t));
                record_error(e->lineno, err_buff);
                return TYPE_ERROR;
            }
            return t;
    }

    return TYPE_ERROR;
}

static Type verify_call(char *id, Expr call) {
    int actual = 0, expected = 0;
    Fsym f = lookup_function(call->e.Ucall.id);
    Exprs as = call->e.Ucall.args;
    Types ts;
    Type t;

    if (f) {
        ts = f->args;
        while(as || ts) {
            if (as && ts) {
                t = verify_expression(id, as->e_first);
                if (ts->t_first == TYPE_REAL && t == TYPE_INT) {
                    pli12yylinenum = call->lineno;
                    as->e_first = make_unop(UNOP_INT_TO_REAL, as->e_first);
                } else if (t && t != ts->t_first) {
                    sprintf(err_buff,
                            "type mismatch in argument %d of call to '%s': " 
                            "actual %s, expected %s"
                            , actual + 1, f->id
                            , type_to_str(t), type_to_str(ts->t_first));
                    record_error(call->lineno, err_buff);
                }
            } else if (as) {
                verify_expression(id, as->e_first);
            }

            actual   += (as) ? 1 : 0;
            expected += (ts) ? 1 : 0;
            ts = (ts) ? ts->t_rest : NULL;
            as = (as) ? as->e_rest : NULL;
        }

        if (actual != expected) {
            sprintf(err_buff,
                    "wrong number of arguments in call to '%s': " 
                    "actual %d, expected %d"
                    , f->id, actual, expected);
            record_error(call->lineno, err_buff);
        }
        return f->ret;
    } else {
        sprintf(err_buff, "call to undefined function '%s'" , call->e.Ucall.id);
        record_error(call->lineno, err_buff);

        while(as) {
            verify_expression(id, as->e_first);
            as = as->e_rest;
        }

        return TYPE_ERROR;
    }
}

static bool is_type(int line, Type exp, Type got) {
    if(!exp || !got) {
        return FALSE;
    } else if(exp != got) {
        sprintf(err_buff, "type mismatch: expected '%s', actual '%s'"
                , type_to_str(exp), type_to_str(got));
        record_error(line, err_buff);
        return FALSE;
    }

    return TRUE;
}

static void should_be_bool(StateType s, Type t, int line) {
    if (t != TYPE_BOOL) {
        switch (s) {
            case STMT_WHILE:
                sprintf(err_buff,
                        "condition of while loop is %s; should be bool"
                        , type_to_str(t)
                        );
                record_error(line, err_buff);
                break;
            case STMT_IF:
            case STMT_ELSE:
                sprintf(err_buff,
                        "condition of if-then-else is %s; should be bool"
                        , type_to_str(t)
                        );
                record_error(line, err_buff);
                break;
            default:
                break;
        }
    }

}

static void pop_sym_table(Funcs fs) {
    while(fs) {
        add_user_function(fs->f_first);
        fs = fs->f_rest;
    }
}

static void init_declarations(Funcs fs) {
    while(fs) {
        fs->f_first = init_declaration(fs->f_first);
        fs = fs->f_rest;
    }
}

static Func init_declaration(Func f) {
    Decls   decls = f->decls;
    Decl    d;
    Stmt    s;
    while(decls) {
        d = decls->d_first;
        if(d->val) {
            pli12yylinenum = d->lineno;
            s = make_assign(d->id,make_const(d->val));
            f->stmts = ins_stmt(s, f->stmts);
        }
        decls = decls->d_rest;
    }
    return f;
}
