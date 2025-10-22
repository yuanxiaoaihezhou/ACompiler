#!/bin/bash

# Test script for ACompiler
# Compares output of our compiler + GCC assembler vs. GCC direct compilation

set -e

COMPILER=./acompiler
TESTDIR=tests
PASSED=0
FAILED=0

# Colors for output
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m' # No Color

echo "Running ACompiler test suite..."
echo "================================"

for testfile in $TESTDIR/test*.c; do
    testname=$(basename $testfile .c)
    echo -n "Testing $testname... "
    
    # Compile with our compiler
    $COMPILER $testfile > $TESTDIR/$testname.s 2>/dev/null || {
        echo -e "${RED}FAIL${NC} (compilation failed)"
        FAILED=$((FAILED + 1))
        continue
    }
    
    # Assemble and link with GCC
    gcc -static -o $TESTDIR/$testname.out $TESTDIR/$testname.s 2>/dev/null || {
        echo -e "${RED}FAIL${NC} (assembly failed)"
        FAILED=$((FAILED + 1))
        continue
    }
    
    # Compile directly with GCC
    gcc -static -o $TESTDIR/$testname.gcc.out $testfile 2>/dev/null || {
        echo -e "${RED}FAIL${NC} (GCC compilation failed)"
        FAILED=$((FAILED + 1))
        continue
    }
    
    # Run both versions and compare exit codes
    set +e
    $TESTDIR/$testname.out
    our_exit=$?
    
    $TESTDIR/$testname.gcc.out
    gcc_exit=$?
    set -e
    
    if [ $our_exit -eq $gcc_exit ]; then
        echo -e "${GREEN}PASS${NC} (exit code: $our_exit)"
        PASSED=$((PASSED + 1))
    else
        echo -e "${RED}FAIL${NC} (our exit: $our_exit, gcc exit: $gcc_exit)"
        FAILED=$((FAILED + 1))
    fi
done

echo "================================"
echo "Results: $PASSED passed, $FAILED failed"

if [ $FAILED -eq 0 ]; then
    echo -e "${GREEN}All tests passed!${NC}"
    exit 0
else
    echo -e "${RED}Some tests failed.${NC}"
    exit 1
fi
