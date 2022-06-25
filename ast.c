#include "ast.h"

#define OFF 5

static void add(char *name);

static char *get_node_type(Ast *node);

static void asm_print(Ast *root);

static void asm_operators_print(Ast *node);

static void asm_assign_print(Ast *node);

static void asm_expression_print(Ast *node, char *name);

static void asm_loop_print(Ast *node);

static char* print_tree(Ast *node, int offset, char *buf);

static char *tmp_next();

static char *label_next();

static char **variable_list;
static uint64_t counter = 0;
static size_t buf_len = 16;
Ast *add_node(Node_Type type, char *value, Ast *left, Ast *right) {
  Ast *node = malloc(sizeof(Ast));
  node->type = type;
  if (value == NULL) {
    node->value = NULL;
  } else {
    node->value = strdup(value);
  }
  node->left = left;
  node->right = right;

  if (type == Root) {
    char *buf = malloc(sizeof(char *) * 16);
    asm_print(node);
    print_tree(node, 0, buf);
  }
  return node;
}

char *tmp_next() {
  static int tmp_count = 0;
  char *name = malloc(sizeof(char) * 16);
  snprintf(name, 16, "tmp_%d", tmp_count);
  tmp_count += 1;
  return name;
}

char *label_next() {
  static int label_count = 0;
  char *l = malloc(sizeof(char) * 16);
  snprintf(l, 16, "label_%d", label_count);
  label_count += 1;
  return l;
}
Ast *add_variable(char *value, Ast *variables) {
  Ast *node = malloc(sizeof(Ast));
  node->type = Variables;
  if (value == NULL) {
    node->value = NULL;
  } else {
    node->value = strdup(value);
  }
  node->right = variables;

  add(node->value);

  return node;
}

Ast *add_assignment(char *name, Ast *expression) {
  Ast *node = malloc(sizeof(Ast));
  node->type = Operator;
  node->value = "=";
  node->left = get_variable(name);
  node->right = expression;
  return node;
}

Ast *add_condition(Node_Type type, Ast *condition, Ast *condition_block,
                   Ast *else_block) {
  Ast *node = malloc(sizeof(Ast));
  node->type = type;
  node->value = NULL;
  node->left = condition;

  if (else_block) {
    node->right =
        add_node(Condition_Expression, NULL, condition_block, else_block);
  } else {
    node->right = add_node(Condition_Expression, NULL, condition_block, NULL);
  }
  return node;
}

static void add(char *name) {
  static uint64_t current_size = 0;
  if (current_size == 0) {
    variable_list = (malloc(sizeof(char *) * 16));
    current_size += 16;
  }

  if (current_size == counter - 1) {
    variable_list =
        (realloc(variable_list, sizeof(uint64_t) * (current_size + 16)));
    current_size += 16;
  }

  variable_list[counter] = name;
  counter++;
}

Ast *get_variable(char *name) {
  for (uint64_t i = 0; i < counter; i++) {
    if (strcmp(variable_list[i], name) == 0) {
      Ast *node = malloc(sizeof(Ast));
      node->type = Identifier;
      node->value = name;
      node->left = NULL;
      node->right = NULL;
      return node;
    }
  }
  fprintf(stderr, "%s wasn't declared!\n", name);
  exit(EXIT_FAILURE);
}

Ast *get_constant(char *value) {
  Ast *node = malloc(sizeof(Ast));
  node->type = Identifier;
  node->value = value;
  node->left = NULL;
  node->right = NULL;
  return node;
}

char* print_tree(Ast *node, int offset, char *buf) {
  if (node == NULL) {
    return buf;
  }

  offset += 4;
  buf = print_tree(node->right, offset, buf);
  if(buf_len - strlen(buf) < offset * 4 + 100){
    buf_len = buf_len + offset * 4 + 100;
    buf = realloc(buf, buf_len);
  }
  char* local_buf = calloc(offset * 4 + 100, 1);
  for (int i = 4; i < offset; i++) {
    strcat(local_buf," ");
  }

  strcat(buf, local_buf);
  char *node_type = get_node_type(node);
  sprintf(local_buf,"[%s]", node_type);
  strcat(buf, local_buf);
  if (node->value) {
    sprintf(local_buf,"(%s)\n", node->value);
  } else {
    sprintf(local_buf,"()\n");
  }
  strcat(buf, local_buf);
  buf = print_tree(node->left, offset, buf);
  
  if(node->type == Root){
    char** strings = NULL;
    size_t strings_size = 0;
    char *new_buf = malloc(buf_len);
    char *next = strtok(buf, "\n");
    while (next != NULL){
      strings_size = strings_size + 1;
      strings = realloc(strings, strings_size* sizeof(char*));
      strings[strings_size - 1] = next;
      next = strtok (NULL,"\n");
    }
    for(int i = strings_size - 1; i >= 0; i--){
      printf("%s\n",strings[i]);
    }
  }
  return buf;
}

char *get_node_type(Ast *node) {
  if (node == NULL) {
    return NULL;
  }

  switch (node->type) {
  case Root:
    return "Root";

  case Variables_Declaration:
    return "Variables_Declaration";

  case Variables:
    return "Variables";

  case Identifier:
    return "Identifier";

  case Main:
    return "Main";

  case Expression:
    return "Expression";

  case Operator:
    return "Operator";

  case Operators:
    return "Operators";

  case Const:
    return "Const";

  case Condition_Expression:
    return "Condition_Expression";

  case Loop:
    return "Loop";

  default:
    return NULL;
  }
}
void asm_operators_print(Ast *node) {
  if (!node) {
    return;
  }

  switch (node->type) {
  case Operators:
    asm_operators_print(node->left);
    asm_operators_print(node->right);
    break;

  case Operator:
    asm_assign_print(node);
    break;

  case Loop:
    asm_loop_print(node);
    break;
  default:
    printf("error\n");
    break;
  }
}

void asm_print(Ast *root) {
  if (!root) {
    return;
  }

  if (!root->right) {
    printf("Empty\n");
    return;
  }
  printf("BEGIN\n");
  asm_operators_print(root->right->left);
  printf("END\n\n");
}

static void asm_loop_print(Ast *node) {
  if (!node) {
    return;
  }

  Ast *operator = node->left;
  Ast *expression = node->right;

  char *l = label_next();
  char *tmp_name = tmp_next();
  printf("%s:\n", l);
  asm_operators_print(operator);
  asm_expression_print(expression, tmp_name);
  printf("cmp_true %s %s\n", tmp_name, l);
}

void asm_expression_print(Ast *node, char *name) {
  if (!node) {
    return;
  }

  Ast *left = node->left;
  Ast *right = node->right;

  char *tmp_name = NULL;

  if (left) {
    switch (left->type) {
    case Expression:
      tmp_name = tmp_next();
      asm_expression_print(left, tmp_name);
      break;

    case Const:
    case Identifier:
      tmp_name = left->value;
      break;

    default:
      printf("EXPRESSION ERROR!\n");
      break;
    }
  }
  char *tmp_name_2 = NULL;

  if (right) {
    switch (right->type) {
    case Expression:
      tmp_name_2 = tmp_next();
      asm_expression_print(right, tmp_name_2);
      break;

    case Const:
    case Identifier:
      tmp_name_2 = right->value;
      break;

    default:
      printf("EXPRESSION ERROR!\n");
      tmp_name_2 = "EXPRESSION ERROR!";
      break;
    }
  }

  if (strcmp(node->value, "-") == 0 || strcmp(node->value, "+") == 0 ||
      strcmp(node->value, "*") == 0 || strcmp(node->value, "/") == 0 ||
      strcmp(node->value, ">") == 0 || strcmp(node->value, "<") == 0 ||
      strcmp(node->value, "=") == 0) {
    printf("%s = %s %s %s\n", name, tmp_name, node->value, tmp_name_2);
  } else if (strcmp(node->value, "MINUS") == 0 ) {
    printf("%s = %s %s\n", name, node->value, tmp_name_2);
  } else {
    printf(node->value);
    printf("EXPRESSION ERROR!\n");
  }
}

void asm_assign_print(Ast *node) {
  if (!node) {
    return;
  }

  Ast *variable = node->left;
  Ast *expression = node->right;

  char *tmp_name;

  switch (expression->type) {
  case Const:
  case Identifier:
    printf("%s %s %s\n", variable->value, node->value, expression->value);
    break;

  case Expression:
    tmp_name = tmp_next();
    asm_expression_print(expression, tmp_name);
    printf("%s %s %s\n", variable->value, node->value, tmp_name);
    break;

  default:
    printf("ASSIGN ERROR\n");
    break;
  }
}


