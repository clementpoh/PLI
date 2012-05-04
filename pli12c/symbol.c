/*
** vim: ts=4 sw=4 expandtab foldmethod=indent
*/
#include <string.h>
#include <stdio.h>

#include "pli12c.h"
#include "symbol.h"

typedef struct s_syms  *Fsyms;

struct s_syms {
	Fsym     first;
	Fsyms    rest;
};


static Fsyms s_table = NULL;

static Param    lookup_var(char *id, Params vs);

static Fsym     make_defined(Func f);
static Types    arg_types(Params ps);
static Params   make_vars(Func f);

static Params   clone_params(Params ps, Params new);
static Param    clone_param(Param p);

static Params   decls_to_vars(Decls ds, Params vs);
static Param    decl_to_var(Decl d);

static Fsym     make_builtin(const char *id, Types args, Type t);
static Fsyms    ins_sym(Fsym s, Fsyms ss);


/* Returns NULL if id is not in the function table. */
Fsym    lookup_function(char *id) {
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

Type    get_var_type(char *func, char *var) {
    Fsym f = lookup_function(func);
    Param v = lookup_var(var, f->vars);

    return (v) ? v->type : TYPE_ERROR;
}

/* TODO: Error report for duplicate name. */
bool	add_user_function(Func f) {
    if(!lookup_function(f->id)) {
        s_table = ins_sym(make_defined(f), s_table);
        return TRUE;
    } else {
        record_error(f->lineno, "function '%s' redefined");
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


    args = ins_type(TYPE_REAL, args);
    sym = make_builtin("sqrt", args, TYPE_REAL);
    s_table = ins_sym(sym, s_table);

    args = ins_type(TYPE_REAL, args);
    sym = make_builtin("trunc", args, TYPE_INT);
    s_table = ins_sym(sym, s_table);

    args = ins_type(TYPE_REAL, args);
    sym = make_builtin("round", args, TYPE_INT);
    s_table = ins_sym(sym, s_table);
}

static Fsym      make_defined(Func f) {
    Fsym new = checked_malloc(sizeof(*new));

    new->id = checked_strdup(f->id);
    new->args = arg_types(f->args);
    new->ret = f->type;
    new->sts = USER_DEFINED;

    new->vars = make_vars(f);

    return new;
}

static Types arg_types(Params ps) {
    return (ps) ? ins_type(ps->p_first->type, arg_types(ps->p_rest)) : NULL;
}

static Params make_vars(Func f) {
    Params  vs = NULL;
    
    vs = clone_params(f->args, vs);
    vs = decls_to_vars(f->decls, vs);

    return vs;
}

/* TODO: Fix the error messages. */
static Params clone_params(Params ps, Params vs) {
    Param v, p;

    while(ps) {
        p = ps->p_first;
        if(!lookup_var(p->id, vs)) {
            v = clone_param(p);
            vs = ins_param(v, vs);
        } else {
            record_error(p->lineno, "variable '%s' redefined");
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

/* TODO: Fix the error messages. */
static Params   decls_to_vars(Decls ds, Params vs) {
    Param v;
    Decl d;

    while(ds) {
        d = ds->d_first;
        if(!lookup_var(d->id, vs)) {
            v = decl_to_var(d);
            vs = ins_param(v, vs);
        } else {
            record_error(d->lineno, "variable '%s' redefined");
        }
        ds = ds->d_rest;
    }

    return vs;
}

static Param decl_to_var(Decl d) {
    Param new = checked_malloc(sizeof(*new));

    new->id = checked_strdup(d->id);
    new->type = d->type;
    new->lineno = d->lineno;
    
    return new;
}

static Fsym make_builtin(const char *id, Types args, Type t) {
    Fsym new = checked_malloc(sizeof(*new));

    new->id = checked_strdup(id);
    new->args = args;
    new->ret = t;
    new->sts = BUILTIN;

    return new;
}

static Fsyms ins_sym(Fsym  s, Fsyms ss) {
    Fsyms new = checked_malloc(sizeof(*new));

    new->first = s;
    new->rest = ss;

    return new;
}
