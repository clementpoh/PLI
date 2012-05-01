#include "pli12c.h"
#include "symbol.h"

Syms s_table = NULL;

static Types arg_types(Params ps);
static Sym make_defined(Func f);
static Sym make_builtin(const char *id, Types args, Type t);
static Syms ins_sym(Sym  s, Syms ss);

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

    s_table = make_defined(f);

}

static Sym make_defined(Func f) {
    Sym new = checked_malloc(sizeof(*new));

    new->id = checked_strdup(f->id);
    new->args = arg_types(f);
    new->ret = f->type;
    new->sts = USER_DEFINED;

    return new;
}

static Types arg_types(Params ps) {
    return (!ps) ? ins_type() : NULL;

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
