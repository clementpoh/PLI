/*
** This is the header file for the symbol table module of the PLI12 compiler.
**
** This is one possible set of functions for implementing the symbol table.
** You can use this set as it is, or change it as you like.
*/

#ifndef	SYMBOL_H
#define	SYMBOL_H

#include "std.h"
#include "ast.h"

/*
** These look after the list of known functions.
*/

extern	void	init_with_builtin_functions(void);
extern	bool	add_user_function(Func f);

/* Returns true if id is already in the function table. */
extern  bool    lookup_function(char *id);
/*
extern	bool	lookup_function(const char *name, Type *return_type_ptr,
                Types *arg_types_ptr, Status *status_ptr);
*/

/*
** These look after the list of known variables in a function,
** including parameters.
*/

extern	void	init_variables(void);
extern	int	add_variable(const char *name, Type type);
extern	int	lookup_variable(const char *name, Type *type_ptr);
extern	int	num_variables(void);

/*
 * Following are used internally.
*/
typedef Param	Var;
typedef Params 	Vars;

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

	Vars	vars;
};

#endif	/* SYMBOL_H */
