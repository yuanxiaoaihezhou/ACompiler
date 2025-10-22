.intel_syntax noprefix
.data
.text
.globl main
main:
  push rbp
  mov rbp, rsp
  sub rsp, 16
  mov rax, 0
  mov rax, 0
  mov rax, rbp
  sub rax, 16
  push rax
  mov rax, 0
  pop rdi
  mov [rdi], rax
  mov rax, rbp
  sub rax, 8
  push rax
  mov rax, 0
  pop rdi
  mov [rdi], rax
.L.begin.0:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, 10
  pop rdi
  cmp rdi, rax
  setl al
  movzb rax, al
  cmp rax, 0
  je .L.end.0
  mov rax, rbp
  sub rax, 16
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  pop rdi
  add rax, rdi
  pop rdi
  mov [rdi], rax
  mov rax, rbp
  sub rax, 8
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, 1
  pop rdi
  add rax, rdi
  pop rdi
  mov [rdi], rax
  jmp .L.begin.0
.L.end.0:
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  jmp .L.return
.L.return:
  mov rsp, rbp
  pop rbp
  ret
