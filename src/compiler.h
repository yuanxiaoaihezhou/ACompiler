#ifndef COMPILER_H
#define COMPILER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Token types
typedef enum {
    TK_NUM,      // Integer literal
    TK_IDENT,    // Identifier
    TK_RETURN,   // return keyword
    TK_IF,       // if keyword
    TK_ELSE,     // else keyword
    TK_WHILE,    // while keyword
    TK_FOR,      // for keyword
    TK_INT,      // int keyword
    TK_CHAR,     // char keyword
    TK_VOID,     // void keyword
    TK_SIZEOF,   // sizeof keyword
    TK_STRING,   // String literal
    TK_PLUS,     // +
    TK_MINUS,    // -
    TK_MUL,      // *
    TK_DIV,      // /
    TK_MOD,      // %
    TK_EQ,       // ==
    TK_NE,       // !=
    TK_LT,       // <
    TK_LE,       // <=
    TK_GT,       // >
    TK_GE,       // >=
    TK_ASSIGN,   // =
    TK_LPAREN,   // (
    TK_RPAREN,   // )
    TK_LBRACE,   // {
    TK_RBRACE,   // }
    TK_LBRACKET, // [
    TK_RBRACKET, // ]
    TK_SEMICOLON,// ;
    TK_COMMA,    // ,
    TK_AMPERSAND,// &
    TK_EOF,      // End of file
} TokenKind;

// Token structure
typedef struct Token {
    TokenKind kind;
    struct Token *next;
    int val;         // For TK_NUM
    char *str;       // Token string
    int len;         // Token length
} Token;

// AST node types
typedef enum {
    ND_ADD,       // +
    ND_SUB,       // -
    ND_MUL,       // *
    ND_DIV,       // /
    ND_MOD,       // %
    ND_EQ,        // ==
    ND_NE,        // !=
    ND_LT,        // <
    ND_LE,        // <=
    ND_ASSIGN,    // =
    ND_LVAR,      // Local variable
    ND_NUM,       // Integer
    ND_RETURN,    // return
    ND_IF,        // if
    ND_WHILE,     // while
    ND_FOR,       // for
    ND_BLOCK,     // { ... }
    ND_FUNCALL,   // Function call
    ND_ADDR,      // Unary &
    ND_DEREF,     // Unary *
    ND_SIZEOF,    // sizeof
    ND_STRING,    // String literal
} NodeKind;

// AST node structure
typedef struct Node {
    NodeKind kind;
    struct Node *lhs;   // Left-hand side
    struct Node *rhs;   // Right-hand side
    
    // For ND_IF, ND_WHILE, ND_FOR
    struct Node *cond;
    struct Node *then;
    struct Node *els;
    struct Node *init;
    struct Node *inc;
    
    // For ND_BLOCK
    struct Node **stmts;
    int num_stmts;
    
    // For ND_FUNCALL
    char *funcname;
    struct Node **args;
    int num_args;
    
    // For ND_NUM
    int val;
    
    // For ND_LVAR and ND_STRING
    int offset;      // Offset from RBP for local variables
    
    // For ND_STRING
    int str_label;   // Label number for string literal
    char *str_val;   // String value
    
    // Type information
    int type_size;   // Size of type (for pointers, arrays)
} Node;

// Local variable
typedef struct LVar {
    struct LVar *next;
    char *name;
    int len;
    int offset;
} LVar;

// Function
typedef struct Function {
    struct Function *next;
    char *name;
    Node **params;
    int num_params;
    Node **locals;
    int num_locals;
    Node **stmts;
    int num_stmts;
    int stack_size;
} Function;

// Global variables
extern char *user_input;
extern Token *token;
extern LVar *locals;
extern int label_count;
extern int str_count;

// Lexer functions
Token *tokenize(char *p);
int consume(TokenKind kind);
Token *consume_ident();
void expect(TokenKind kind);
int expect_number();
int at_eof();

// Parser functions
Function *program();
Function *function();
Node *stmt();
Node *expr();
Node *assign();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();
Node *new_node_addr(Node *node);
Node *new_node_deref(Node *node);

// Code generator functions
void codegen(Function *prog);
void gen(Node *node);
void gen_strings(Function *prog);
void gen_strings_node(Node *node);

// Utility functions
void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);

#endif
