# ACompiler

A self-hosting C compiler written in C that compiles a subset of C sufficient to compile itself.

## Features

- **Lexical Analysis**: Tokenizes C source code
- **Syntax Analysis**: Builds Abstract Syntax Tree (AST)
- **Code Generation**: Generates x86-64 assembly
- **Self-Hosting Capable**: Subset sufficient for compiler implementation

## Supported C Subset

- Basic types: `int`, `char`, `void`, pointers
- Arithmetic: `+`, `-`, `*`, `/`, `%`
- Comparisons: `==`, `!=`, `<`, `<=`, `>`, `>=`
- Control flow: `if/else`, `while`, `for`, `return`
- Functions: definitions and calls
- Variables: local variables with declarations
- Pointers and arrays (basic support)
- String literals
- Comments: `//` and `/* */`

## Building

```bash
make
```

## Usage

Compile a C source file to assembly:

```bash
./acompiler source.c > output.s
```

Assemble and link with GCC:

```bash
gcc -static -o output output.s
```

Complete compilation pipeline:

```bash
./acompiler source.c > output.s && gcc -static -o output output.s && ./output
```

## Testing

Run the test suite:

```bash
make test
```

This will:
1. Compile test programs with ACompiler
2. Assemble and link with GCC
3. Compare output with direct GCC compilation
4. Report results

## Example

**hello.c**:
```c
int main() {
    int a;
    int b;
    a = 5;
    b = 3;
    return a + b;
}
```

**Compilation**:
```bash
./acompiler hello.c > hello.s
gcc -static -o hello hello.s
./hello
echo $?  # Outputs: 8
```

## Project Structure

```
ACompiler/
├── src/
│   ├── compiler.h     # Header file with data structures
│   ├── main.c         # Main entry point
│   ├── tokenize.c     # Lexer
│   ├── parse.c        # Parser
│   └── codegen.c      # Code generator
├── tests/
│   ├── test.sh        # Test harness
│   └── test*.c        # Test cases
├── docs/
│   └── TECHNICAL.md   # Technical documentation
├── Makefile           # Build system
└── README.md          # This file
```

## Documentation

See [docs/TECHNICAL.md](docs/TECHNICAL.md) for detailed technical documentation including:
- Architecture overview
- Grammar specification
- Code generation details
- Calling conventions
- Limitations and future work

## Requirements

- GCC (for assembling and linking)
- GNU Make
- Linux x86-64 system

## License

This project is provided as-is for educational purposes.

## Acknowledgments

This compiler is inspired by the chibicc compiler and various compiler design resources.