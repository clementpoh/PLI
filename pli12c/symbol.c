/*
** vim: ts=4 sw=4 expandtab foldmethod=indent
*/
#include <string.h>
#include <stdio.h>

#include "pli12c.h"
#include "symbol.h"

typedef struct s_syms  *Syms;
typedef struct s_sym   *Sym;

struct s_syms {
	Sym     first;
	Syms    rest;
};

struct s_sym {
	char	*id;
	Type	ret;
	Types	args;
    Status  sts;

	Params	vars;
};

static Syms s_table = NULL;

static Types arg_types(Params ps);
static Sym make_defined(Func f);
static Param dec_to_par(Decl d);
static Params make_vars(Func f);
static Sym make_builtin(const char *id, Types args, Type t);
static Syms ins_sym(Sym  s, Syms ss);
static bool lookup_var(char *id, Params vars);

void	init_with_builtin_functions(void) { 
    Sym sym;
    Types args;

    sym = make_builtin("read_int", NULL, TYPE_INT);
    s_table = ins_sym(sym, s_table);

    sym = make_builtin("read_real", NULL, TYPE_REAL);
    s_table = ins_sym(sym, s_table);

    sym = make_builtin("read_bool", NULL, TYPE_BOOL);
    s_table = ins_sym(sym, s_table);

    sym = make_builtin("read_string", NULL, TYPE_STRING);
    s_table = ins_sym(sym, s_table);

    args = ins_type(TYPE_INT, NULL);
    sym = make_builtin("write_int", args, TYPE_ERROR);
    s_table = ins_sym(sym, s_table);

    args = ins_type(TYPE_REAL, NULL);
    sym = make_builtin("write_real", args, TYPE_ERROR);
    s_table = ins_sym(sym, s_table);

    args = ins_type(TYPE_BOOL, NULL);
    sym = make_builtin("write_bool", args, TYPE_ERROR);
    s_table = ins_sym(sym, s_table);

    args = ins_type(TYPE_STRING, NULL);
    sym = make_builtin("write_string", args, TYPE_ERROR);
    s_table = ins_sym(sym, s_table);


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

bool	add_user_function(Func f) {
    if(!lookup_function(f->id)) {
        s_table = ins_sym(make_defined(f), s_table);
        return TRUE;
    } else {
        /* TODO: Error report for duplicate name. */
        record_error(f->lineno, "function '%s' redefined");
        return FALSE;
    }
}

static Sym make_defined(Func f) {
    Sym new = checked_malloc(sizeof(*new));

    new->id = checked_strdup(f->id);
    new->args = arg_types(f->args);
    new->ret = f->type;
    new->sts = USER_DEFINED;

    new->vars = make_vars(f);

    return new;
}

/* Returns true if id is already in the function table. */
bool    lookup_function(char *id) {
    Syms curr = s_table;
    while(curr) {
        if (!strcmp(id, curr->first->id)) {
            return TRUE;
        } else {
            curr = curr->rest;
        }
    }

    return FALSE;
}

static bool lookup_var(char *id, Params vars) {
    Params curr = vars;
    while(curr) {
        if (!strcmp(id, curr->p_first->id)) {
            return TRUE;
        } else {
            curr = curr->p_rest;
        }
    }

    return FALSE;
}

static Params make_vars(Func f) {
    Params vars;
    Params p = f->args; 
    Decls d = f->decls;

    while(p) {
        if(!lookup_var(p->p_first->id, vars)) {
            vars = ins_param(p->p_first, vars);
        } else {
            record_error(p->p_first->lineno, "variable '%s' redefined");
        }
        p = p->p_rest;
    }

    while(d) {
        if(!lookup_var(d->d_first->id, vars)) {
            vars = ins_param(dec_to_par(d->d_first), vars);
        } else {
            record_error(d->d_first->lineno, "variable '%s' redefined");
        }
         d = d->d_rest;
    }

    return vars;
}

static Param dec_to_par(Decl d) {
    Param new = checked_malloc(sizeof(*new));

    new->id = checked_strdup(d->id);
    new->type = d->type;
    
    return new;
}

static Types arg_types(Params ps) {
    return (ps) ? ins_type(ps->p_first->type, arg_types(ps->p_rest)) : NULL;
}

static Sym make_builtin(const char *id, Types args, Type t) {
    Sym new = checked_malloc(sizeof(*new));

    new->id = checked_strdup(id);
    new->args = args;
    new->ret = t;
    new->sts = BUILTIN;

    return new;
}

static Syms ins_sym(Sym  s, Syms ss) {
    Syms new = checked_malloc(sizeof(*new));

    new->first = s;
    new->rest = ss;

    return new;
}
