#include "compiler.h"
#include <stdarg.h>

char *user_input;
Token *token;

// Error reporting
void error(char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

void error_at(char *loc, char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    
    int pos = loc - user_input;
    fprintf(stderr, "%s\n", user_input);
    fprintf(stderr, "%*s", pos, "");
    fprintf(stderr, "^ ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

// Create a new token
Token *new_token(TokenKind kind, Token *cur, char *str, int len) {
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    tok->len = len;
    cur->next = tok;
    return tok;
}

// Check if string starts with expected
int startswith(char *p, char *q) {
    return strncmp(p, q, strlen(q)) == 0;
}

// Check if character is valid for identifier
int is_alnum(char c) {
    return ('a' <= c && c <= 'z') ||
           ('A' <= c && c <= 'Z') ||
           ('0' <= c && c <= '9') ||
           (c == '_');
}

// Check if keyword
TokenKind check_keyword(char *p, int len) {
    if (len == 6 && strncmp(p, "return", 6) == 0)
        return TK_RETURN;
    if (len == 2 && strncmp(p, "if", 2) == 0)
        return TK_IF;
    if (len == 4 && strncmp(p, "else", 4) == 0)
        return TK_ELSE;
    if (len == 5 && strncmp(p, "while", 5) == 0)
        return TK_WHILE;
    if (len == 3 && strncmp(p, "for", 3) == 0)
        return TK_FOR;
    if (len == 3 && strncmp(p, "int", 3) == 0)
        return TK_INT;
    if (len == 4 && strncmp(p, "char", 4) == 0)
        return TK_CHAR;
    if (len == 4 && strncmp(p, "void", 4) == 0)
        return TK_VOID;
    if (len == 6 && strncmp(p, "sizeof", 6) == 0)
        return TK_SIZEOF;
    return TK_IDENT;
}

// Tokenize input string
Token *tokenize(char *p) {
    Token head;
    head.next = NULL;
    Token *cur = &head;
    
    while (*p) {
        // Skip whitespace
        if (isspace(*p)) {
            p++;
            continue;
        }
        
        // Skip line comments
        if (startswith(p, "//")) {
            p += 2;
            while (*p != '\n')
                p++;
            continue;
        }
        
        // Skip block comments
        if (startswith(p, "/*")) {
            char *q = strstr(p + 2, "*/");
            if (!q)
                error_at(p, "Unclosed block comment");
            p = q + 2;
            continue;
        }
        
        // String literal
        if (*p == '"') {
            char *start = p;
            p++;
            while (*p != '"') {
                if (*p == '\0')
                    error_at(start, "Unclosed string literal");
                if (*p == '\\')
                    p++;
                p++;
            }
            p++;
            cur = new_token(TK_STRING, cur, start, p - start);
            continue;
        }
        
        // Two-character operators
        if (startswith(p, "==")) {
            cur = new_token(TK_EQ, cur, p, 2);
            p += 2;
            continue;
        }
        if (startswith(p, "!=")) {
            cur = new_token(TK_NE, cur, p, 2);
            p += 2;
            continue;
        }
        if (startswith(p, "<=")) {
            cur = new_token(TK_LE, cur, p, 2);
            p += 2;
            continue;
        }
        if (startswith(p, ">=")) {
            cur = new_token(TK_GE, cur, p, 2);
            p += 2;
            continue;
        }
        
        // Single-character operators
        if (*p == '+') {
            cur = new_token(TK_PLUS, cur, p++, 1);
            continue;
        }
        if (*p == '-') {
            cur = new_token(TK_MINUS, cur, p++, 1);
            continue;
        }
        if (*p == '*') {
            cur = new_token(TK_MUL, cur, p++, 1);
            continue;
        }
        if (*p == '/') {
            cur = new_token(TK_DIV, cur, p++, 1);
            continue;
        }
        if (*p == '%') {
            cur = new_token(TK_MOD, cur, p++, 1);
            continue;
        }
        if (*p == '<') {
            cur = new_token(TK_LT, cur, p++, 1);
            continue;
        }
        if (*p == '>') {
            cur = new_token(TK_GT, cur, p++, 1);
            continue;
        }
        if (*p == '=') {
            cur = new_token(TK_ASSIGN, cur, p++, 1);
            continue;
        }
        if (*p == '(') {
            cur = new_token(TK_LPAREN, cur, p++, 1);
            continue;
        }
        if (*p == ')') {
            cur = new_token(TK_RPAREN, cur, p++, 1);
            continue;
        }
        if (*p == '{') {
            cur = new_token(TK_LBRACE, cur, p++, 1);
            continue;
        }
        if (*p == '}') {
            cur = new_token(TK_RBRACE, cur, p++, 1);
            continue;
        }
        if (*p == '[') {
            cur = new_token(TK_LBRACKET, cur, p++, 1);
            continue;
        }
        if (*p == ']') {
            cur = new_token(TK_RBRACKET, cur, p++, 1);
            continue;
        }
        if (*p == ';') {
            cur = new_token(TK_SEMICOLON, cur, p++, 1);
            continue;
        }
        if (*p == ',') {
            cur = new_token(TK_COMMA, cur, p++, 1);
            continue;
        }
        if (*p == '&') {
            cur = new_token(TK_AMPERSAND, cur, p++, 1);
            continue;
        }
        
        // Identifier or keyword
        if (isalpha(*p) || *p == '_') {
            char *start = p;
            while (is_alnum(*p))
                p++;
            int len = p - start;
            TokenKind kind = check_keyword(start, len);
            cur = new_token(kind, cur, start, len);
            continue;
        }
        
        // Number
        if (isdigit(*p)) {
            cur = new_token(TK_NUM, cur, p, 0);
            char *q = p;
            cur->val = strtol(p, &p, 10);
            cur->len = p - q;
            continue;
        }
        
        error_at(p, "Invalid token");
    }
    
    new_token(TK_EOF, cur, p, 0);
    return head.next;
}

// Consume a token of expected kind
int consume(TokenKind kind) {
    if (token->kind != kind)
        return 0;
    token = token->next;
    return 1;
}

// Consume identifier token
Token *consume_ident() {
    if (token->kind != TK_IDENT)
        return NULL;
    Token *tok = token;
    token = token->next;
    return tok;
}

// Expect a token of specific kind
void expect(TokenKind kind) {
    if (token->kind != kind)
        error_at(token->str, "Expected different token");
    token = token->next;
}

// Expect number token
int expect_number() {
    if (token->kind != TK_NUM)
        error_at(token->str, "Expected a number");
    int val = token->val;
    token = token->next;
    return val;
}

// Check if at end of file
int at_eof() {
    return token->kind == TK_EOF;
}
