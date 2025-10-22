#include "compiler.h"

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        return 1;
    }
    
    // Read input file
    FILE *fp = fopen(argv[1], "r");
    if (!fp) {
        perror(argv[1]);
        return 1;
    }
    
    // Read entire file
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    user_input = calloc(1, size + 1);
    fread(user_input, 1, size, fp);
    fclose(fp);
    
    // Tokenize
    token = tokenize(user_input);
    
    // Parse
    Function *prog = program();
    
    // Generate code
    codegen(prog);
    
    return 0;
}
