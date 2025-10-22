# ACompiler Technical Documentation

## Overview

ACompiler is a self-hosting C compiler written in C. It compiles a subset of the C language sufficient to compile itself, generating x86-64 assembly code that can be assembled and linked by GCC.

## Architecture

### Components

1. **Lexer (tokenize.c)**: Converts source code into tokens
2. **Parser (parse.c)**: Builds an Abstract Syntax Tree (AST) from tokens
3. **Code Generator (codegen.c)**: Generates x86-64 assembly from AST
4. **Main (main.c)**: Orchestrates the compilation pipeline

### Data Flow

```
Source Code → Lexer → Tokens → Parser → AST → Code Generator → Assembly
```

## Supported C Subset

### Data Types
- `int`: 8-byte integer
- `char`: 1-byte character
- `void`: No type (for functions)
- Pointers: `type *`

### Operators

**Arithmetic**:
- `+`, `-`, `*`, `/`, `%`

**Comparison**:
- `==`, `!=`, `<`, `<=`, `>`, `>=`

**Assignment**:
- `=`

**Unary**:
- `&` (address-of)
- `*` (dereference)

### Control Flow

**Conditional**:
```c
if (condition)
    statement
else
    statement
```

**Loops**:
```c
while (condition)
    statement

for (init; condition; increment)
    statement
```

**Return**:
```c
return expression;
```

### Functions

**Definition**:
```c
return_type function_name(param_type param_name, ...) {
    statements
}
```

**Call**:
```c
function_name(arg1, arg2, ...);
```

### Variables

**Declaration**:
```c
int variable_name;
char *pointer_name;
```

**Assignment**:
```c
variable_name = expression;
```

### Literals

- Integer: `123`, `0`, `-45`
- String: `"hello world"`

### Arrays and Pointers

**Array subscript**:
```c
array[index]  // Equivalent to *(array + index)
```

**Address and dereference**:
```c
&variable  // Address of variable
*pointer   // Dereference pointer
```

### Comments

- Line comments: `// comment`
- Block comments: `/* comment */`

## Implementation Details

### Lexer

The lexer (`tokenize.c`) performs:
- Whitespace skipping
- Comment removal
- Keyword recognition
- Operator tokenization
- Number parsing
- String literal parsing
- Identifier extraction

### Parser

The parser (`parse.c`) uses recursive descent parsing with the following grammar precedence (lowest to highest):

1. **program**: function*
2. **function**: type ident "(" params? ")" "{" stmt* "}"
3. **stmt**: Various statement types
4. **expr**: assign
5. **assign**: equality ("=" assign)?
6. **equality**: relational ("==" relational | "!=" relational)*
7. **relational**: add ("<" add | "<=" add | ">" add | ">=" add)*
8. **add**: mul ("+" mul | "-" mul)*
9. **mul**: unary ("*" unary | "/" unary | "%" unary)*
10. **unary**: ("+" | "-" | "*" | "&")? unary | primary
11. **primary**: num | ident | "(" expr ")" | funcall

### Code Generator

The code generator (`codegen.c`) produces x86-64 assembly following the System V AMD64 ABI:

**Register Usage**:
- `rax`: Return value, temporary
- `rdi`, `rsi`, `rdx`, `rcx`, `r8`, `r9`: First 6 function arguments
- `rbp`: Frame pointer
- `rsp`: Stack pointer

**Calling Convention**:
- Arguments passed in registers (up to 6)
- Return value in `rax`
- Stack aligned to 16 bytes
- Caller-saved registers preserved as needed

**Stack Layout**:
```
High address
+------------------+
| Previous frame   |
+------------------+
| Return address   |
+------------------+
| Saved RBP        | <- RBP
+------------------+
| Local var 1      | <- RBP - 8
+------------------+
| Local var 2      | <- RBP - 16
+------------------+
| ...              |
+------------------+ <- RSP
Low address
```

## Limitations

The current implementation has the following limitations:

1. **No type system**: Treats all integers as 8 bytes
2. **No struct/union**: Only basic types supported
3. **Limited array support**: No multi-dimensional arrays
4. **No preprocessor**: No #include, #define, etc.
5. **No global variables**: Only local variables in functions
6. **No function declarations**: Functions must be defined before use
7. **Limited error messages**: Basic error reporting

## Future Enhancements

To achieve full self-hosting, the following features would be needed:

1. **Preprocessor**: #include for header files
2. **Global variables**: For static data
3. **struct/union**: For complex data structures
4. **typedef**: For type aliases
5. **Better type system**: Proper type checking and conversions
6. **Function declarations**: Forward declarations
7. **Better error messages**: Line numbers, more context

## Assembly Output Format

The compiler generates AT&T syntax assembly that can be assembled by GAS (GNU Assembler):

```assembly
.intel_syntax noprefix
.data
.LC0:
  .string "Hello, World!"
.text
.globl main
main:
  push rbp
  mov rbp, rsp
  sub rsp, 16
  ; function body
  mov rsp, rbp
  pop rbp
  ret
```

## Testing

The test suite (`tests/test.sh`) validates the compiler by:

1. Compiling test programs with ACompiler to assembly
2. Assembling and linking with GCC
3. Compiling the same programs directly with GCC
4. Comparing exit codes of both executables

All tests must produce identical exit codes to pass.
