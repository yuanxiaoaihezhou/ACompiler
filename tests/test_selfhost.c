// Simplified self-hosting test
// This tests if the compiler can handle code similar to itself

// Token structure (simplified)
int tokenize(int input) {
    int token;
    token = input;
    while (token > 0) {
        token = token - 1;
    }
    return token;
}

// Parser (simplified)
int parse(int tokens) {
    int i;
    int result;
    result = 0;
    for (i = 0; i < tokens; i = i + 1) {
        result = result + i;
    }
    return result;
}

// Code generator (simplified)
int codegen(int ast) {
    if (ast > 10)
        return ast * 2;
    else
        return ast + 5;
}

// Main compiler pipeline
int main() {
    int input;
    int tokens;
    int ast;
    int code;
    
    input = 15;
    
    tokens = tokenize(input);
    ast = parse(10);
    code = codegen(ast);
    
    return code;
}
