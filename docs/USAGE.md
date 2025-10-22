# ACompiler Usage Guide

## Quick Start

### 1. Build the Compiler

```bash
make
```

This creates the `acompiler` executable.

### 2. Compile a C Program

```bash
./acompiler input.c > output.s
```

This generates x86-64 assembly code in AT&T syntax.

### 3. Assemble and Link

```bash
gcc -static -o output output.s
```

This uses GCC to assemble and link the generated assembly.

### 4. Run the Program

```bash
./output
echo $?  # Print exit code
```

## Complete Example

Given this C program (`hello.c`):

```c
int add(int a, int b) {
    return a + b;
}

int main() {
    int x;
    int y;
    x = 10;
    y = 20;
    return add(x, y);
}
```

Compile and run it:

```bash
./acompiler hello.c > hello.s
gcc -static -o hello hello.s
./hello
echo $?  # Should print: 30
```

## Examples

The `examples/` directory contains sample programs:

### Fibonacci Numbers

```bash
./acompiler examples/fibonacci.c > fib.s
gcc -static -o fib fib.s
./fib
echo $?  # Prints the 9th Fibonacci number (34)
```

### Greatest Common Divisor

```bash
./acompiler examples/gcd.c > gcd.s
gcc -static -o gcd gcd.s
./gcd
echo $?  # Prints GCD of 48 and 18 (6)
```

### Pointer Operations

```bash
./acompiler examples/pointers.c > pointers.s
gcc -static -o pointers pointers.s
./pointers
echo $?  # Demonstrates pointer arithmetic
```

## Writing Compatible Code

### Supported Features

**Data Types**:
- `int` - 8-byte integer
- `char` - 1-byte character
- `void` - For function return types
- Pointers: `int *`, `char *`, etc.

**Operators**:
- Arithmetic: `+`, `-`, `*`, `/`, `%`
- Comparison: `==`, `!=`, `<`, `<=`, `>`, `>=`
- Assignment: `=`
- Address: `&variable`
- Dereference: `*pointer`

**Control Flow**:
```c
// If-else
if (condition)
    statement
else
    statement

// While loop
while (condition)
    statement

// For loop
for (init; condition; increment)
    statement

// Return
return expression;
```

**Functions**:
```c
int function_name(int param1, int param2) {
    // function body
    return result;
}
```

**Variables**:
```c
int variable;
char *pointer;
variable = 10;
pointer = &variable;
```

### Limitations

1. **No preprocessor**: Cannot use `#include`, `#define`, etc.
2. **No global variables**: Only local variables in functions
3. **No struct/union**: Only basic types
4. **No arrays**: Can use pointers instead
5. **Function order**: Functions must be defined before they are called
6. **No standard library**: Cannot use `printf`, `malloc`, etc.
7. **Exit codes only**: Programs communicate via exit codes

### Workarounds

**Instead of printf**:
- Use exit codes to return values
- Example: `return 42;` instead of `printf("%d\n", 42);`

**Instead of arrays**:
- Not directly supported in current version
- Future versions may add array support

**Instead of global variables**:
- Pass values as function parameters
- Return values from functions

## Testing Your Code

### Manual Testing

```bash
# Compile with our compiler
./acompiler myprogram.c > myprogram.s
gcc -static -o myprogram myprogram.s

# Compile with GCC for comparison
gcc -static -o myprogram.gcc myprogram.c

# Compare exit codes
./myprogram
echo "Our compiler: $?"
./myprogram.gcc
echo "GCC: $?"
```

### Automated Testing

Add your test to `tests/testXX.c` and run:

```bash
make test
```

The test harness will:
1. Compile your test with ACompiler
2. Assemble/link with GCC
3. Compile directly with GCC
4. Compare exit codes
5. Report results

## Debugging

### View Generated Assembly

```bash
./acompiler program.c
```

This prints assembly to stdout for inspection.

### Common Issues

**"Not an lvalue" error**:
- Trying to assign to a non-variable (e.g., `5 = x;`)
- Solution: Ensure left side of `=` is a variable or pointer dereference

**"Expected different token" error**:
- Syntax error in your C code
- Solution: Check syntax, especially semicolons and braces

**Assembly errors from GCC**:
- Bug in code generator
- Solution: Inspect generated assembly, report issue

### Tips

1. Start simple - test each feature individually
2. Compare generated assembly with GCC output
3. Use exit codes creatively to verify results
4. Write small, focused test cases

## Advanced Usage

### Inspecting AST (for developers)

Add debug prints to `parse.c` to see the Abstract Syntax Tree.

### Custom Code Generation

Modify `codegen.c` to:
- Support different architectures
- Optimize output
- Add new features

### Extending the Language

1. Add tokens in `compiler.h`
2. Update lexer in `tokenize.c`
3. Update parser in `parse.c`
4. Add code generation in `codegen.c`

## Getting Help

1. Read [TECHNICAL.md](TECHNICAL.md) for architecture details
2. Check existing tests in `tests/` for examples
3. Examine examples in `examples/`
4. Review generated assembly to understand behavior
