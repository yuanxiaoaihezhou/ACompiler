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
  sub rax, 8
  push rax
  mov rax, 5
  pop rdi
  mov [rdi], rax
  mov rax, rbp
  sub rax, 16
  push rax
  mov rax, 3
  pop rdi
  mov [rdi], rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  pop rdi
  add rax, rdi
  jmp .L.return
.L.return:
  mov rsp, rbp
  pop rbp
  ret
