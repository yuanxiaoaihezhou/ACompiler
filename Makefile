# Makefile for ACompiler

CC = gcc
CFLAGS = -Wall -std=c11 -g
SRCS = src/main.c src/tokenize.c src/parse.c src/codegen.c
OBJS = $(SRCS:.c=.o)
TARGET = acompiler

.PHONY: all clean test

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c src/compiler.h
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(TARGET) $(OBJS) tests/*.s tests/*.out tests/*.gcc.out

test: $(TARGET)
	@echo "Running tests..."
	@bash tests/test.sh

.PHONY: help
help:
	@echo "ACompiler - A self-hosting C compiler"
	@echo ""
	@echo "Usage:"
	@echo "  make          Build the compiler"
	@echo "  make test     Run test suite"
	@echo "  make clean    Clean build artifacts"
	@echo "  make help     Show this help message"
