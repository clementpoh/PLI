/* A Bison parser, made by GNU Bison 2.4.2.  */

/* Skeleton interface for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2006, 2009-2010 Free Software
   Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     FUNCTION = 258,
     LPAREN = 259,
     RPAREN = 260,
     COLON = 261,
     COMMA = 262,
     RETURNS = 263,
     BEGINS = 264,
     END = 265,
     DECLARE = 266,
     SEMICOLON = 267,
     INITIALIZE = 268,
     TO = 269,
     READ = 270,
     WRITE = 271,
     IF = 272,
     THEN = 273,
     ELSE = 274,
     ENDIF = 275,
     WHILE = 276,
     DO = 277,
     ENDWHILE = 278,
     RETURN = 279,
     NOT = 280,
     OR = 281,
     AND = 282,
     ASSIGN = 283,
     EQ = 284,
     NE = 285,
     LT = 286,
     LE = 287,
     GT = 288,
     GE = 289,
     ADD = 290,
     SUB = 291,
     MUL = 292,
     DIV = 293,
     INT_CONST = 294,
     REAL_CONST = 295,
     BOOL_CONST = 296,
     STRING_CONST = 297,
     ID = 298,
     TYPE = 299,
     GARBAGE = 300,
     UMINUS = 301
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 1685 of yacc.c  */
#line 21 "parser.y"

    char    *Ustr;
    int     Uint;
    bool    Ubool;
    float   Ureal;
    Type    Utype;
    Const   Uconst;
    BinOp   Ubinop;

    Funcs   Ufuncs;
    Func    Ufunc;

    Params  Uparams;
    Param   Uparam;

    Decls   Udecls;
    Decl    Udecl;

    Stmts   Ustmts;
    Stmt    Ustmt;

    Exprs   Uexprs;
    Expr    Uexpr;



/* Line 1685 of yacc.c  */
#line 124 "parser.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE pli12yylval;


