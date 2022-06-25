#ifndef AST_UTIL_H
#define AST_UTIL_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
  Root,
  Variables_Declaration,
  Variables,
  Identifier,
  Main,
  Expression,
  Const,
  Operator,
  Operators,
  Condition_Expression,
  Loop,
  Undefined
} Node_Type;

typedef struct Ast {
  Node_Type type;
  char *value;
  struct Ast *left;
  struct Ast *right;

} Ast;

Ast *add_node(Node_Type type, char *value, Ast *left, Ast *right);

Ast *add_variable(char *value, Ast *variables);

Ast *add_assignment(char *name, Ast *expression);

Ast *add_condition(Node_Type type, Ast *condition, Ast *condition_block,
                   Ast *else_block);

Ast *get_variable(char *name);

Ast *get_constant(char *value);

#endif // AST_UTIL_H
