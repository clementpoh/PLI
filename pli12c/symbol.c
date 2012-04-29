#include "symbol.h"

Funcs symbol = NULL;

make_builtin(const char *id, Params ps, Type t);

void	init_with_builtin_functions(void) { 

    symbol = ins_func(make_builtin("read_int", NULL, TYPE_INT), symbol);
    symbol = ins_func(make_builtin("read_real", NULL, TYPE_INT), symbol);
    symbol = ins_func(make_builtin("read_bool", NULL, TYPE_INT), symbol);
    symbol = ins_func(make_builtin("read_string", NULL, TYPE_INT), symbol);

}

make_builtin(const char *id, Params ps, Type t) {
    Func new = checked_malloc(sizeof(*new));

    new->id = id;
    new->args = ps;
    new->type = t;
    new->decls = NULL;
    new->stmts = NULL;
    new->sts = BUILTIN;

    return new;
}
