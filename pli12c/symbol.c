#include "pli12c.h"
#include "symbol.h"

Syms s_table = NULL;

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
    sym = make_builtin("write_int", args, TYPE_INT);
    s_table = ins_sym(sym, s_table);

    args = ins_type(TYPE_REAL, NULL);
    sym = make_builtin("write_real", args, TYPE_INT);
    s_table = ins_sym(sym, s_table);

    args = ins_type(TYPE_BOOL, NULL);
    sym = make_builtin("write_bool", args, TYPE_INT);
    s_table = ins_sym(sym, s_table);

    args = ins_type(TYPE_STRING, NULL);
    sym = make_builtin("write_string", args, TYPE_INT);
    s_table = ins_sym(sym, s_table);
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
