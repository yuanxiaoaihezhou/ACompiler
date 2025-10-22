# Self-Hosting Guide

## What is Self-Hosting?

A self-hosting compiler can compile its own source code. For ACompiler to be self-hosting, it must be able to compile `main.c`, `tokenize.c`, `parse.c`, and `codegen.c` to produce a working compiler executable.

## Current Status

ACompiler currently supports a subset of C that is **almost** sufficient for self-hosting. The main limitations are:

### Missing Features for Self-Hosting

1. **Preprocessor Directives**
   - `#include` - for header files
   - `#define` - for constants and macros
   - `#ifndef`, `#ifdef` - for header guards

2. **Global Variables**
   - Currently only supports local variables
   - Compiler uses globals like `user_input`, `token`, `locals`

3. **Multi-file Compilation**
   - Need to compile and link multiple `.c` files
   - Need to handle separate compilation units

4. **Standard Library Headers**
   - Need basic types from `<stdio.h>`, `<stdlib.h>`, `<string.h>`, etc.
   - Or need to declare these ourselves

## Roadmap to Self-Hosting

### Phase 1: Single-File Compilation (Current)

**Status**: ✅ Complete

The compiler can handle single-file programs with:
- Functions
- Local variables
- Control flow
- Pointers
- Recursion

### Phase 2: Global Variables

**Status**: ❌ Not implemented

**Required Changes**:

1. **Parser**: Add support for global variable declarations
2. **Code Generator**: 
   - Output `.data` or `.bss` section for globals
   - Reference globals by absolute address, not RBP offset
3. **Symbol Table**: Track both global and local variables separately

**Example**:
```c
int global_var;

int main() {
    global_var = 42;
    return global_var;
}
```

### Phase 3: Simple Preprocessor

**Status**: ❌ Not implemented

**Required Features**:

1. **#include**: Textual inclusion of header files
2. **#define**: Simple constant substitution
3. **Header Guards**: `#ifndef`/`#define`/`#endif`

**Implementation Approach**:
- Add a preprocessing pass before tokenization
- Simple text substitution for `#define`
- File reading for `#include`

### Phase 4: Multi-File Linking

**Status**: ❌ Not implemented

**Required Changes**:

1. **External Function Declarations**: `extern`
2. **Function Prototypes**: Forward declarations
3. **Linking**: Combine multiple object files

**Workaround**:
- Concatenate all `.c` files into one
- Remove duplicate includes/defines
- Compile as single unit

## Minimal Self-Hosting Example

### Option 1: Merge All Sources

Create `compiler_single.c`:

```c
// All type definitions and forward declarations
typedef enum { ... } TokenKind;
typedef struct Token { ... } Token;
// ... etc

// All functions from tokenize.c
Token *tokenize(char *p) { ... }
// ...

// All functions from parse.c
Function *program() { ... }
// ...

// All functions from codegen.c
void codegen(Function *prog) { ... }
// ...

// Main function
int main(int argc, char **argv) { ... }
```

Then:
```bash
./acompiler compiler_single.c > compiler.s
gcc -static -o acompiler_v2 compiler.s
./acompiler_v2 compiler_single.c > compiler2.s
diff compiler.s compiler2.s  # Should be identical!
```

### Option 2: Add Missing Features

Implement the features listed above to handle the existing multi-file structure.

## Testing Self-Hosting

### Stage 0: Bootstrap with GCC

```bash
gcc -o acompiler0 src/*.c
```

### Stage 1: Compile with Stage 0

```bash
./acompiler0 compiler_single.c > compiler1.s
gcc -static -o acompiler1 compiler1.s
```

### Stage 2: Compile with Stage 1

```bash
./acompiler1 compiler_single.c > compiler2.s
gcc -static -o acompiler2 compiler2.s
```

### Verification

```bash
# Stage 1 and Stage 2 should produce identical output
diff compiler1.s compiler2.s
```

If the diff shows no differences, the compiler is self-hosting!

## Limitations of Current Self-Hosting Approach

1. **Single File Only**: Must merge all sources
2. **No Standard Library**: Must avoid stdio, stdlib functions
3. **Manual Declarations**: Must declare all functions manually
4. **Limited Types**: Only basic integer types

## Future Enhancements

### Short Term (for basic self-hosting)

1. **Global Variables**: Required for compiler state
2. **Simple #include**: To include header files
3. **extern keyword**: For function declarations
4. **Static keyword**: For file-local functions

### Long Term (for full C support)

1. **struct/union**: For complex data structures
2. **typedef**: For type aliases  
3. **enum**: For better type safety
4. **Arrays**: Proper array support
5. **Float/Double**: Floating point types
6. **Preprocessor**: Full macro expansion
7. **Linker**: Handle object files

## Contributing to Self-Hosting

To help achieve self-hosting:

1. **Add Global Variables**:
   - Update parser to recognize global declarations
   - Modify codegen to emit `.data`/`.bss` sections
   - Test with simple global variable programs

2. **Add Preprocessor**:
   - Implement `#include` as file inclusion
   - Implement `#define` as text substitution
   - Handle conditional compilation

3. **Test Incrementally**:
   - Start with small self-compiling programs
   - Gradually add more features
   - Maintain compatibility with existing tests

## References

- [chibicc](https://github.com/rui314/chibicc) - A small C compiler
- [TinyCC](https://bellard.org/tcc/) - Tiny C Compiler
- [SubC](http://www.t3x.org/subc/) - A simple self-hosting C compiler

These compilers demonstrate various approaches to self-hosting and can provide inspiration for implementation.
