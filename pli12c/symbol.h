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

typedef struct s_syms  *Fsyms;
typedef struct s_sym   *Fsym;

struct s_sym {
	char	*id;
	Type	ret;
	Types	args;
    Status  sts;

	int lineno;

	Params	vars;
};

struct s_syms {
	Fsym     first;
	Fsyms    rest;
};

/*
** These look after the list of known functions.
*/

extern  Fsyms   s_table;
extern  char    err_buff[];

extern	void	init_with_builtin_functions(void);
extern	bool	add_user_function(Func f);

extern	Fsym    lookup_function(char *id);
extern  Param   lookup_variable(char *func, char *var);

extern  Type    get_func_type(char *func);

/* Records an error if variable can not be found. */
extern  Type    get_var_type(int lineno, char *func, char *var);

/*
** These look after the list of known variables in a function,
** including parameters.
*/

extern	int	    num_variables(void);

#endif	/* SYMBOL_H */
