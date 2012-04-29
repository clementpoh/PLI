#include "symbol.h"

Funcs symbol = NULL;

static Func make_builtin(const char *id, Params ps, Type t);
static Func make_write();

void	init_with_builtin_functions(void) { 

    symbol = ins_func(make_builtin("read_int", NULL, TYPE_INT), symbol);
    symbol = ins_func(make_builtin("read_real", NULL, TYPE_INT), symbol);
    symbol = ins_func(make_builtin("read_bool", NULL, TYPE_INT), symbol);
    symbol = ins_func(make_builtin("read_string", NULL, TYPE_INT), symbol);


}

static Func make_builtin(const char *id, Params ps, Type t) {
    return make_func(id, ps, t, NULL, NULL, BUILTIN);
}
