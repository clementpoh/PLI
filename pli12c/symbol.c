/*
** vim: ts=4 sw=4 expandtab foldmethod=indent
*/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "pli12c.h"
#include "symbol.h"

#define BUFFER_LEN 1024

char err_buff[BUFFER_LEN];

static Fsyms s_table = NULL;

static Param    lookup_var(char *id, Params vs);

static Fsym     make_defined(Func f);
static Types    arg_types(Params ps);
static Params   make_vars(Func f);

static Params   params_to_vars(Params ps, Params new);
static Param    clone_param(Param p);

static Params   decls_to_vars(Decls ds, Params vs);
static Param    decl_to_var(Decl d, int pos);

static Fsym     make_builtin(const char *id, Types args, Type t);
static Fsyms    ins_sym(Fsym s, Fsyms ss);


/* Returns NULL if id is not in the function table. */
Fsym    lookup_function(const char *id) {
    Fsyms curr = s_table;
    while(curr) {
        if (!strcmp(id, curr->first->id)) {
            return curr->first;
        } 
        curr = curr->rest;
    }

    return NULL;
}

Param   lookup_variable(char *func, char *var) {
    Fsym f = lookup_function(func);
    return (f) ? lookup_var(var, f->vars) : NULL;
}

Param   lookup_var(char *id, Params vs) {
    Params curr = vs;
    while(curr) {
        if (!strcmp(id, curr->p_first->id)) {
            return curr->p_first;
        } 
        curr = curr->p_rest;
    }

    return NULL;
}

Type    get_func_type(char *func) {
    Fsym f = lookup_function(func);

    return (f) ? f->ret : TYPE_ERROR;
}

Type    get_var_type(int lineno, char *func, char *var) {
    Fsym f = lookup_function(func);
    Param v;

    if(f) {
        v = lookup_var(var, f->vars);
        if(v) {
            return v->type;
        } else {
            sprintf(err_buff, "reference to undefined variable '%s'", var);
            record_error(lineno, err_buff);
        }
    }

    return TYPE_ERROR;
}

bool	add_user_function(Func f) {
    if(!lookup_function(f->id)) {
        s_table = ins_sym(make_defined(f), s_table);
        return TRUE;
    } else {
        sprintf(err_buff, "function '%s' redefined", f->id);
        record_error(f->lineno, err_buff);
        return FALSE;
    }
}

void	init_with_builtin_functions(void) { 
    Fsym sym;
    Types args;

    args = ins_type(TYPE_STRING, NULL);
    args = ins_type(TYPE_STRING, args);
    sym = make_builtin("string_concat", args, TYPE_STRING);
    s_table = ins_sym(sym, s_table);

    args = ins_type(TYPE_STRING, NULL);
    sym = make_builtin("string_length", args, TYPE_INT);
    s_table = ins_sym(sym, s_table);

    args = ins_type(TYPE_INT, NULL);
    args = ins_type(TYPE_INT, args);
    args = ins_type(TYPE_STRING, args);
    sym = make_builtin("substring", args, TYPE_STRING);
    s_table = ins_sym(sym, s_table);


    args = ins_type(TYPE_REAL, NULL);
    sym = make_builtin("sqrt", args, TYPE_REAL);
    s_table = ins_sym(sym, s_table);

    args = ins_type(TYPE_REAL, NULL);
    sym = make_builtin("trunc", args, TYPE_INT);
    s_table = ins_sym(sym, s_table);

    args = ins_type(TYPE_REAL, NULL);
    sym = make_builtin("round", args, TYPE_INT);
    s_table = ins_sym(sym, s_table);
}

static Fsym      make_defined(Func f) {
    Fsym new = checked_malloc(sizeof(*new));

    new->id = checked_strdup(f->id);
    new->args = arg_types(f->args);
    new->ret = f->type;
    new->sts = USER_DEFINED;

    new->lineno = f->lineno;

    new->vars = make_vars(f);

    return new;
}

static Types arg_types(Params ps) {
    return (ps) ? ins_type(ps->p_first->type, arg_types(ps->p_rest)) : NULL;
}

static Params make_vars(Func f) {
    Params  vs = NULL;
    
    vs = params_to_vars(f->args, vs);
    vs = decls_to_vars(f->decls, vs);

    return vs;
}

static Params params_to_vars(Params ps, Params vs) {
    Param v, p;

    while(ps) {
        p = ps->p_first;
        if(!lookup_var(p->id, vs)) {
            v = clone_param(p);
            vs = ins_param(v, vs);
        } else {
            sprintf(err_buff, "variable '%s' redefined", p->id);
            record_error(p->lineno, err_buff);
        }
        ps = ps->p_rest;
    }

    return vs;
}

static Param clone_param(Param p) {
    Param new = checked_malloc(sizeof(*new));

    new->id = checked_strdup(p->id);
    new->type = p->type;
    new->lineno = p->lineno;
    
    return new;
}

static Params   decls_to_vars(Decls ds, Params vs) {
    int pos = 0;
    Param v;
    Decl d;

    while(ds) {
        d = ds->d_first;
        if(!lookup_var(d->id, vs)) {
            v = decl_to_var(d, pos++);
            vs = ins_param(v, vs);
            if(d->val && d->val->type != d->type) {
                sprintf(err_buff,
                        "type mismatch in initialization of '%s': "
                        "assigning %s to %s" 
                        , d->id
                        , type_to_str(d->val->type)
                        , type_to_str(d->type)
                        );
                record_error(d->lineno, err_buff);

                free(d->val);
                d->val = NULL;
            }
        } else {
            sprintf(err_buff, "variable '%s' redefined", d->id);
            record_error(d->lineno, err_buff);
            /* Remove any would be initializations. */
            free(d->val);
            d->val = NULL;
        }
        ds = ds->d_rest;
    }

    return vs;
}

static Param decl_to_var(Decl d, int pos) {
    Param new = checked_malloc(sizeof(*new));

    new->id = checked_strdup(d->id);
    new->type = d->type;
    new->pos = pos;

    new->lineno = d->lineno;
    
    return new;
}

static Fsym make_builtin(const char *id, Types args, Type t) {
    Fsym new = checked_malloc(sizeof(*new));

    new->id = checked_strdup(id);
    new->args = args;
    new->ret = t;
    new->sts = BUILTIN;

    new->lineno = 0;

    return new;
}

static Fsyms ins_sym(Fsym  s, Fsyms ss) {
    Fsyms new = checked_malloc(sizeof(*new));

    new->first = s;
    new->rest = ss;

    return new;
}
