%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
extern int yylex();
extern char* yyval;
extern int line;
void yyerror(char* str);

%}


%token VAR 
%token BBEGIN EEND
%token SEMICOLON COMMA LPARENTHENSIS RPARENTHENSIS
%token WHILE DO
%token MINUS
%token PLUS MUL DIV LESS MORE EQUALS
%token ASSIGN
%token ID CONST


%start program

%right ASSIGN
%left PLUS MINUS
%left MUL DIV

%union {
    struct Ast *tree;
    char *str;
}

%type<tree> program main variables_declaration variables operators operator expression sub_expression assigment loop_operator compound_operator
%type<str> ID CONST
%%
program:
    variables_declaration main { $$ = add_node(Root, NULL, $1, $2); }
    ;

main:
    BBEGIN operators EEND { $$ = add_node(Main, NULL, $2, NULL); }
    ;

variables_declaration:
    VAR variables { $$ = add_node(Variables_Declaration, NULL, NULL, $2); }
    ;

variables:
    ID SEMICOLON{ $$ = add_variable($1, NULL); }
    | ID COMMA variables { $$ = add_variable($1, $3); }
    | ID SEMICOLON variables { $$ = add_variable($1, $3); }
    ;

operators:
    operator { $$ = $1; }
    | operator operators { $$ = add_node(Operators, NULL, $1, $2); }
    ;

operator:
    assigment { $$ = $1;}
    | loop_operator { $$ = $1;}
    | compound_operator { $$ = $1;}
    ;
compound_operator:
    BBEGIN operators EEND { $$ = add_node(Operators, NULL, $2, NULL); }
    ;
assigment:
    ID ASSIGN expression SEMICOLON { $$ = add_assignment($1, $3); }
    ;

expression:
    MINUS sub_expression { $$ = add_node(Expression, "MINUS", NULL, $2); }
    | sub_expression { $$ = $1; }
    ;

sub_expression:
    LPARENTHENSIS expression RPARENTHENSIS { $$ = $2; }
    | ID { $$ = get_variable($1); }
    | CONST { $$ = get_constant($1); }
    | sub_expression PLUS sub_expression { $$ = add_node(Expression, "+", $1, $3); }
    | sub_expression MINUS sub_expression { $$ = add_node(Expression, "-", $1, $3); }
    | sub_expression MUL sub_expression { $$ = add_node(Expression, "*", $1, $3); }
    | sub_expression DIV sub_expression { $$ = add_node(Expression, "/", $1, $3); }
    | sub_expression EQUALS sub_expression { $$ = add_node(Expression, "=", $1, $3); }
    | sub_expression MORE sub_expression { $$ = add_node(Expression, ">", $1, $3); }
    | sub_expression LESS sub_expression { $$ = add_node(Expression, "<", $1, $3); }
    ;


loop_operator:
    WHILE expression DO operator { $$ = add_node(Loop, NULL, $4, $2); }
    ;

%%

void yyerror(char* str) {
    fprintf(stderr, "error at %d line\n", line);
    exit(1);
}

int main(int argc, char **argv) {
    return yyparse();
}

