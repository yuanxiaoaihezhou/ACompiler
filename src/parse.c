#include "compiler.h"

LVar *locals;
int label_count = 0;
int str_count = 0;

// Create a new AST node
Node *new_node(NodeKind kind) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = kind;
    return node;
}

// Create a binary node
Node *new_binary(NodeKind kind, Node *lhs, Node *rhs) {
    Node *node = new_node(kind);
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

// Create a number node
Node *new_num(int val) {
    Node *node = new_node(ND_NUM);
    node->val = val;
    return node;
}

// Find local variable
LVar *find_lvar(Token *tok) {
    for (LVar *var = locals; var; var = var->next)
        if (var->len == tok->len && !memcmp(tok->str, var->name, var->len))
            return var;
    return NULL;
}

// Create new local variable
LVar *new_lvar(Token *tok) {
    LVar *var = calloc(1, sizeof(LVar));
    var->next = locals;
    var->name = tok->str;
    var->len = tok->len;
    
    if (locals)
        var->offset = locals->offset + 8;
    else
        var->offset = 8;
    
    locals = var;
    return var;
}

// Forward declarations
Node *expr();
Node *stmt();

// primary = num | ident ("(" (expr ("," expr)*)? ")")? | "(" expr ")" | 
//           "sizeof" "(" type ")" | string
Node *primary() {
    // String literal
    if (token->kind == TK_STRING) {
        Node *node = new_node(ND_STRING);
        node->str_val = calloc(1, token->len + 1);
        int j = 0;
        // Parse string, handling escape sequences
        for (int i = 1; i < token->len - 1; i++) {
            if (token->str[i] == '\\' && i + 1 < token->len - 1) {
                i++;
                if (token->str[i] == 'n') node->str_val[j++] = '\n';
                else if (token->str[i] == 't') node->str_val[j++] = '\t';
                else if (token->str[i] == '\\') node->str_val[j++] = '\\';
                else if (token->str[i] == '"') node->str_val[j++] = '"';
                else node->str_val[j++] = token->str[i];
            } else {
                node->str_val[j++] = token->str[i];
            }
        }
        node->str_val[j] = '\0';
        node->str_label = str_count++;
        token = token->next;
        return node;
    }
    
    // "(" expr ")"
    if (consume(TK_LPAREN)) {
        Node *node = expr();
        expect(TK_RPAREN);
        return node;
    }
    
    // sizeof "(" type ")"
    if (consume(TK_SIZEOF)) {
        expect(TK_LPAREN);
        // For simplicity, we'll just handle int and char
        int size = 0;
        if (consume(TK_INT)) {
            size = 8;
        } else if (consume(TK_CHAR)) {
            size = 1;
        } else {
            error_at(token->str, "Expected type name");
        }
        
        // Handle pointer type
        while (consume(TK_MUL)) {
            size = 8;
        }
        
        expect(TK_RPAREN);
        return new_num(size);
    }
    
    // Identifier (variable or function call)
    Token *tok = consume_ident();
    if (tok) {
        // Function call
        if (consume(TK_LPAREN)) {
            Node *node = new_node(ND_FUNCALL);
            node->funcname = calloc(1, tok->len + 1);
            memcpy(node->funcname, tok->str, tok->len);
            node->funcname[tok->len] = '\0';
            
            // Parse arguments
            Node **args = NULL;
            int num_args = 0;
            
            if (!consume(TK_RPAREN)) {
                args = calloc(16, sizeof(Node*));
                args[num_args++] = expr();
                
                while (consume(TK_COMMA)) {
                    args[num_args++] = expr();
                }
                
                expect(TK_RPAREN);
            }
            
            node->args = args;
            node->num_args = num_args;
            return node;
        }
        
        // Variable
        LVar *var = find_lvar(tok);
        if (!var)
            var = new_lvar(tok);
        
        Node *node = new_node(ND_LVAR);
        node->offset = var->offset;
        return node;
    }
    
    // Number
    return new_num(expect_number());
}

// unary = ("+" | "-" | "*" | "&")? unary | primary ("[" expr "]")*
Node *unary() {
    if (consume(TK_PLUS))
        return unary();
    if (consume(TK_MINUS))
        return new_binary(ND_SUB, new_num(0), unary());
    if (consume(TK_MUL))
        return new_node_deref(unary());
    if (consume(TK_AMPERSAND))
        return new_node_addr(unary());
    
    Node *node = primary();
    
    // Array subscript
    while (consume(TK_LBRACKET)) {
        Node *idx = expr();
        expect(TK_RBRACKET);
        // a[i] is equivalent to *(a + i)
        node = new_node_deref(new_binary(ND_ADD, node, idx));
    }
    
    return node;
}

Node *new_node_addr(Node *node) {
    Node *n = new_node(ND_ADDR);
    n->lhs = node;
    return n;
}

Node *new_node_deref(Node *node) {
    Node *n = new_node(ND_DEREF);
    n->lhs = node;
    return n;
}

// mul = unary ("*" unary | "/" unary | "%" unary)*
Node *mul() {
    Node *node = unary();
    
    for (;;) {
        if (consume(TK_MUL))
            node = new_binary(ND_MUL, node, unary());
        else if (consume(TK_DIV))
            node = new_binary(ND_DIV, node, unary());
        else if (consume(TK_MOD))
            node = new_binary(ND_MOD, node, unary());
        else
            return node;
    }
}

// add = mul ("+" mul | "-" mul)*
Node *add() {
    Node *node = mul();
    
    for (;;) {
        if (consume(TK_PLUS))
            node = new_binary(ND_ADD, node, mul());
        else if (consume(TK_MINUS))
            node = new_binary(ND_SUB, node, mul());
        else
            return node;
    }
}

// relational = add ("<" add | "<=" add | ">" add | ">=" add)*
Node *relational() {
    Node *node = add();
    
    for (;;) {
        if (consume(TK_LT))
            node = new_binary(ND_LT, node, add());
        else if (consume(TK_LE))
            node = new_binary(ND_LE, node, add());
        else if (consume(TK_GT))
            node = new_binary(ND_LT, add(), node);
        else if (consume(TK_GE))
            node = new_binary(ND_LE, add(), node);
        else
            return node;
    }
}

// equality = relational ("==" relational | "!=" relational)*
Node *equality() {
    Node *node = relational();
    
    for (;;) {
        if (consume(TK_EQ))
            node = new_binary(ND_EQ, node, relational());
        else if (consume(TK_NE))
            node = new_binary(ND_NE, node, relational());
        else
            return node;
    }
}

// assign = equality ("=" assign)?
Node *assign() {
    Node *node = equality();
    if (consume(TK_ASSIGN))
        node = new_binary(ND_ASSIGN, node, assign());
    return node;
}

// expr = assign
Node *expr() {
    return assign();
}

// stmt = "return" expr ";"
//      | "if" "(" expr ")" stmt ("else" stmt)?
//      | "while" "(" expr ")" stmt
//      | "for" "(" expr? ";" expr? ";" expr? ")" stmt
//      | "{" stmt* "}"
//      | type ident ";"
//      | expr ";"
Node *stmt() {
    // "return" expr ";"
    if (consume(TK_RETURN)) {
        Node *node = new_node(ND_RETURN);
        node->lhs = expr();
        expect(TK_SEMICOLON);
        return node;
    }
    
    // "if" "(" expr ")" stmt ("else" stmt)?
    if (consume(TK_IF)) {
        Node *node = new_node(ND_IF);
        expect(TK_LPAREN);
        node->cond = expr();
        expect(TK_RPAREN);
        node->then = stmt();
        if (consume(TK_ELSE))
            node->els = stmt();
        return node;
    }
    
    // "while" "(" expr ")" stmt
    if (consume(TK_WHILE)) {
        Node *node = new_node(ND_WHILE);
        expect(TK_LPAREN);
        node->cond = expr();
        expect(TK_RPAREN);
        node->then = stmt();
        return node;
    }
    
    // "for" "(" expr? ";" expr? ";" expr? ")" stmt
    if (consume(TK_FOR)) {
        Node *node = new_node(ND_FOR);
        expect(TK_LPAREN);
        
        if (!consume(TK_SEMICOLON)) {
            node->init = expr();
            expect(TK_SEMICOLON);
        }
        
        if (!consume(TK_SEMICOLON)) {
            node->cond = expr();
            expect(TK_SEMICOLON);
        }
        
        if (!consume(TK_RPAREN)) {
            node->inc = expr();
            expect(TK_RPAREN);
        }
        
        node->then = stmt();
        return node;
    }
    
    // "{" stmt* "}"
    if (consume(TK_LBRACE)) {
        Node *node = new_node(ND_BLOCK);
        Node **stmts = calloc(100, sizeof(Node*));
        int num_stmts = 0;
        
        while (!consume(TK_RBRACE)) {
            stmts[num_stmts++] = stmt();
        }
        
        node->stmts = stmts;
        node->num_stmts = num_stmts;
        return node;
    }
    
    // Variable declaration: type ident ";"
    if (token->kind == TK_INT || token->kind == TK_CHAR || token->kind == TK_VOID) {
        token = token->next;  // Skip type
        while (consume(TK_MUL)) {}  // Skip pointer stars
        
        Token *tok = consume_ident();
        if (tok) {
            // Create variable if it doesn't exist
            LVar *var = find_lvar(tok);
            if (!var)
                var = new_lvar(tok);
            
            expect(TK_SEMICOLON);
            // Return a dummy node (no code generated for declarations)
            return new_num(0);
        }
    }
    
    // expr ";"
    Node *node = expr();
    expect(TK_SEMICOLON);
    return node;
}

// Parse function parameters
void parse_params(Function *func) {
    func->params = NULL;
    func->num_params = 0;
    
    if (consume(TK_RPAREN))
        return;
    
    Node **params = calloc(16, sizeof(Node*));
    int num_params = 0;
    
    // For now, we'll just skip parameter types
    // and treat parameters as local variables
    do {
        if (consume(TK_INT) || consume(TK_CHAR) || consume(TK_VOID)) {
            while (consume(TK_MUL)) {}  // Skip pointer stars
        }
        
        Token *tok = consume_ident();
        if (tok) {
            LVar *var = new_lvar(tok);
            Node *node = new_node(ND_LVAR);
            node->offset = var->offset;
            params[num_params++] = node;
        }
    } while (consume(TK_COMMA));
    
    expect(TK_RPAREN);
    func->params = params;
    func->num_params = num_params;
}

// function = type ident "(" params? ")" "{" stmt* "}"
Function *function() {
    locals = NULL;
    
    // Parse return type
    if (consume(TK_INT) || consume(TK_CHAR) || consume(TK_VOID)) {
        while (consume(TK_MUL)) {}  // Skip pointer stars
    }
    
    // Parse function name
    Token *tok = consume_ident();
    if (!tok)
        error_at(token->str, "Expected function name");
    
    Function *func = calloc(1, sizeof(Function));
    func->name = calloc(1, tok->len + 1);
    memcpy(func->name, tok->str, tok->len);
    func->name[tok->len] = '\0';
    
    // Parse parameters
    expect(TK_LPAREN);
    parse_params(func);
    
    // Parse function body
    expect(TK_LBRACE);
    
    Node **stmts = calloc(100, sizeof(Node*));
    int num_stmts = 0;
    
    while (!consume(TK_RBRACE)) {
        stmts[num_stmts++] = stmt();
    }
    
    func->stmts = stmts;
    func->num_stmts = num_stmts;
    
    // Calculate stack size
    func->stack_size = locals ? locals->offset : 0;
    
    return func;
}

// program = function*
Function *program() {
    Function head;
    head.next = NULL;
    Function *cur = &head;
    
    while (!at_eof()) {
        Function *func = function();
        cur->next = func;
        cur = func;
    }
    
    return head.next;
}
