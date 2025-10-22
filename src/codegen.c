#include "compiler.h"

static int label_seq = 0;
static char *current_func = NULL;

// Generate code to push to stack
void gen_push() {
    printf("  push rax\n");
}

// Generate code to pop from stack
void gen_pop(char *reg) {
    printf("  pop %s\n", reg);
}

// Generate address of a variable
void gen_lval(Node *node) {
    if (node->kind != ND_LVAR)
        error("Not an lvalue");
    
    printf("  mov rax, rbp\n");
    printf("  sub rax, %d\n", node->offset);
    gen_push();
}

// Generate code for an expression
void gen(Node *node) {
    switch (node->kind) {
    case ND_NUM:
        printf("  mov rax, %d\n", node->val);
        return;
    
    case ND_STRING:
        printf("  lea rax, [rip + .LC%d]\n", node->str_label);
        return;
    
    case ND_LVAR:
        gen_lval(node);
        gen_pop("rax");
        printf("  mov rax, [rax]\n");
        return;
    
    case ND_ASSIGN:
        gen_lval(node->lhs);
        gen(node->rhs);
        gen_pop("rdi");
        printf("  mov [rdi], rax\n");
        return;
    
    case ND_ADDR:
        gen_lval(node->lhs);
        gen_pop("rax");
        return;
    
    case ND_DEREF:
        gen(node->lhs);
        printf("  mov rax, [rax]\n");
        return;
    
    case ND_RETURN:
        gen(node->lhs);
        printf("  jmp .L.return.%s\n", current_func);
        return;
    
    case ND_IF: {
        int seq = label_seq++;
        if (node->els) {
            gen(node->cond);
            printf("  cmp rax, 0\n");
            printf("  je .L.else.%d\n", seq);
            gen(node->then);
            printf("  jmp .L.end.%d\n", seq);
            printf(".L.else.%d:\n", seq);
            gen(node->els);
            printf(".L.end.%d:\n", seq);
        } else {
            gen(node->cond);
            printf("  cmp rax, 0\n");
            printf("  je .L.end.%d\n", seq);
            gen(node->then);
            printf(".L.end.%d:\n", seq);
        }
        return;
    }
    
    case ND_WHILE: {
        int seq = label_seq++;
        printf(".L.begin.%d:\n", seq);
        gen(node->cond);
        printf("  cmp rax, 0\n");
        printf("  je .L.end.%d\n", seq);
        gen(node->then);
        printf("  jmp .L.begin.%d\n", seq);
        printf(".L.end.%d:\n", seq);
        return;
    }
    
    case ND_FOR: {
        int seq = label_seq++;
        if (node->init)
            gen(node->init);
        printf(".L.begin.%d:\n", seq);
        if (node->cond) {
            gen(node->cond);
            printf("  cmp rax, 0\n");
            printf("  je .L.end.%d\n", seq);
        }
        gen(node->then);
        if (node->inc)
            gen(node->inc);
        printf("  jmp .L.begin.%d\n", seq);
        printf(".L.end.%d:\n", seq);
        return;
    }
    
    case ND_BLOCK:
        for (int i = 0; i < node->num_stmts; i++)
            gen(node->stmts[i]);
        return;
    
    case ND_FUNCALL: {
        // Push arguments in reverse order (following x86-64 calling convention)
        for (int i = node->num_args - 1; i >= 0; i--) {
            gen(node->args[i]);
            gen_push();
        }
        
        // Pop arguments to registers (up to 6 arguments)
        char *regs[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
        for (int i = 0; i < node->num_args && i < 6; i++) {
            gen_pop(regs[i]);
        }
        
        // Call function
        // Align stack to 16 bytes
        printf("  mov rax, rsp\n");
        printf("  and rax, 15\n");
        printf("  jnz .L.call.%d\n", label_seq);
        printf("  mov rax, 0\n");
        printf("  call %s\n", node->funcname);
        printf("  jmp .L.end.%d\n", label_seq);
        printf(".L.call.%d:\n", label_seq);
        printf("  sub rsp, 8\n");
        printf("  mov rax, 0\n");
        printf("  call %s\n", node->funcname);
        printf("  add rsp, 8\n");
        printf(".L.end.%d:\n", label_seq);
        label_seq++;
        return;
    }
    
    case ND_SIZEOF:
        printf("  mov rax, %d\n", node->val);
        return;
    }
    
    // Binary operators
    gen(node->lhs);
    gen_push();
    gen(node->rhs);
    gen_pop("rdi");
    
    switch (node->kind) {
    case ND_ADD:
        printf("  add rax, rdi\n");
        return;
    case ND_SUB:
        printf("  sub rdi, rax\n");
        printf("  mov rax, rdi\n");
        return;
    case ND_MUL:
        printf("  imul rax, rdi\n");
        return;
    case ND_DIV:
        printf("  mov rcx, rax\n");
        printf("  mov rax, rdi\n");
        printf("  cqo\n");
        printf("  idiv rcx\n");
        return;
    case ND_MOD:
        printf("  mov rcx, rax\n");
        printf("  mov rax, rdi\n");
        printf("  cqo\n");
        printf("  idiv rcx\n");
        printf("  mov rax, rdx\n");
        return;
    case ND_EQ:
        printf("  cmp rdi, rax\n");
        printf("  sete al\n");
        printf("  movzb rax, al\n");
        return;
    case ND_NE:
        printf("  cmp rdi, rax\n");
        printf("  setne al\n");
        printf("  movzb rax, al\n");
        return;
    case ND_LT:
        printf("  cmp rdi, rax\n");
        printf("  setl al\n");
        printf("  movzb rax, al\n");
        return;
    case ND_LE:
        printf("  cmp rdi, rax\n");
        printf("  setle al\n");
        printf("  movzb rax, al\n");
        return;
    }
}

// Generate string literals
void gen_strings(Function *prog) {
    printf(".data\n");
    
    // Collect all string literals
    for (Function *fn = prog; fn; fn = fn->next) {
        for (int i = 0; i < fn->num_stmts; i++) {
            gen_strings_node(fn->stmts[i]);
        }
    }
}

void gen_strings_node(Node *node) {
    if (!node)
        return;
    
    if (node->kind == ND_STRING) {
        printf(".LC%d:\n", node->str_label);
        printf("  .string \"");
        for (char *p = node->str_val; *p; p++) {
            if (*p == '\n') printf("\\n");
            else if (*p == '\t') printf("\\t");
            else if (*p == '\\') printf("\\\\");
            else if (*p == '"') printf("\\\"");
            else printf("%c", *p);
        }
        printf("\"\n");
        return;
    }
    
    gen_strings_node(node->lhs);
    gen_strings_node(node->rhs);
    gen_strings_node(node->cond);
    gen_strings_node(node->then);
    gen_strings_node(node->els);
    gen_strings_node(node->init);
    gen_strings_node(node->inc);
    
    if (node->stmts) {
        for (int i = 0; i < node->num_stmts; i++)
            gen_strings_node(node->stmts[i]);
    }
    
    if (node->args) {
        for (int i = 0; i < node->num_args; i++)
            gen_strings_node(node->args[i]);
    }
}

// Generate code for entire program
void codegen(Function *prog) {
    // Output assembly header
    printf(".intel_syntax noprefix\n");
    
    // Generate string literals
    gen_strings(prog);
    
    // Generate code for each function
    printf(".text\n");
    for (Function *fn = prog; fn; fn = fn->next) {
        current_func = fn->name;
        printf(".globl %s\n", fn->name);
        printf("%s:\n", fn->name);
        
        // Prologue
        printf("  push rbp\n");
        printf("  mov rbp, rsp\n");
        printf("  sub rsp, %d\n", fn->stack_size);
        
        // Save arguments to local variables
        char *regs[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
        for (int i = 0; i < fn->num_params && i < 6; i++) {
            printf("  mov [rbp-%d], %s\n", fn->params[i]->offset, regs[i]);
        }
        
        // Generate code for statements
        for (int i = 0; i < fn->num_stmts; i++) {
            gen(fn->stmts[i]);
        }
        
        // Epilogue (with function-specific label)
        printf(".L.return.%s:\n", fn->name);
        printf("  mov rsp, rbp\n");
        printf("  pop rbp\n");
        printf("  ret\n");
    }
}
