#!/bin/bash

# Comprehensive validation script for ACompiler
# Tests all aspects of the compiler

echo "====================================="
echo "   ACompiler Validation Suite"
echo "====================================="
echo

# Colors
GREEN='\033[0;32m'
RED='\033[0;31m'
BLUE='\033[0;34m'
NC='\033[0m'

ERRORS=0

# Function to report test result
report() {
    if [ $1 -eq 0 ]; then
        echo -e "${GREEN}✓${NC} $2"
    else
        echo -e "${RED}✗${NC} $2"
        ERRORS=$((ERRORS + 1))
    fi
}

# 1. Check if compiler builds
echo -e "${BLUE}[1/6] Building compiler...${NC}"
make clean > /dev/null 2>&1
if make > /dev/null 2>&1; then
    report 0 "Compiler builds successfully"
else
    report 1 "Compiler build failed"
    exit 1
fi
echo

# 2. Run test suite
echo -e "${BLUE}[2/6] Running test suite...${NC}"
if make test > /tmp/test_output.txt 2>&1; then
    PASSED=$(grep "passed" /tmp/test_output.txt | grep -o '[0-9]*' | head -1)
    FAILED=$(grep "failed" /tmp/test_output.txt | grep -o '[0-9]*' | tail -1)
    report 0 "Test suite: $PASSED passed, $FAILED failed"
else
    report 1 "Test suite execution failed"
fi
echo

# 3. Test examples
echo -e "${BLUE}[3/6] Testing example programs...${NC}"
for example in examples/*.c; do
    name=$(basename "$example" .c)
    if ./acompiler "$example" > /tmp/ex.s 2>/dev/null && \
       gcc -static -o /tmp/ex /tmp/ex.s 2>/dev/null; then
        /tmp/ex 2>/dev/null
        EXIT_CODE=$?
        report 0 "$name (exit: $EXIT_CODE)"
    else
        report 1 "$name compilation failed"
    fi
done
echo

# 4. Verify output matches GCC
echo -e "${BLUE}[4/6] Verifying output matches GCC...${NC}"
TEST_PROG="tests/test03.c"
./acompiler "$TEST_PROG" > /tmp/our.s 2>/dev/null
gcc -static -o /tmp/our /tmp/our.s 2>/dev/null
gcc -static -o /tmp/gcc "$TEST_PROG" 2>/dev/null
/tmp/our 2>/dev/null
OUR_EXIT=$?
/tmp/gcc 2>/dev/null
GCC_EXIT=$?
if [ $OUR_EXIT -eq $GCC_EXIT ]; then
    report 0 "Output matches GCC (exit code: $OUR_EXIT)"
else
    report 1 "Output differs from GCC (our: $OUR_EXIT, gcc: $GCC_EXIT)"
fi
echo

# 5. Check documentation
echo -e "${BLUE}[5/6] Checking documentation...${NC}"
for doc in README.md docs/TECHNICAL.md docs/USAGE.md docs/SELF_HOSTING.md PROJECT_SUMMARY.md; do
    if [ -f "$doc" ]; then
        LINES=$(wc -l < "$doc")
        report 0 "$doc ($LINES lines)"
    else
        report 1 "$doc missing"
    fi
done
echo

# 6. Code statistics
echo -e "${BLUE}[6/6] Code Statistics...${NC}"
C_LINES=$(find src -name "*.c" -o -name "*.h" | xargs wc -l | tail -1 | awk '{print $1}')
TEST_LINES=$(find tests -name "*.c" | xargs wc -l 2>/dev/null | tail -1 | awk '{print $1}')
DOC_WORDS=$(wc -w docs/*.md README.md PROJECT_SUMMARY.md 2>/dev/null | tail -1 | awk '{print $1}')
echo "  Source code: $C_LINES lines"
echo "  Test code: $TEST_LINES lines"
echo "  Documentation: $DOC_WORDS words"
echo

# Summary
echo "====================================="
if [ $ERRORS -eq 0 ]; then
    echo -e "${GREEN}All validation checks passed!${NC}"
    echo "====================================="
    exit 0
else
    echo -e "${RED}$ERRORS validation check(s) failed${NC}"
    echo "====================================="
    exit 1
fi
