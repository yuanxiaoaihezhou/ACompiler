.intel_syntax noprefix
.data
.text
.globl main
main:
  push rbp
  mov rbp, rsp
  sub rsp, 8
  mov rax, 0
  mov rax, rbp
  sub rax, 8
  push rax
  mov rax, 5
  pop rdi
  mov [rdi], rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, 5
  pop rdi
  cmp rdi, rax
  sete al
  movzb rax, al
  cmp rax, 0
  je .L.end.0
  mov rax, 10
  jmp .L.return
.L.end.0:
  mov rax, 0
  jmp .L.return
.L.return:
  mov rsp, rbp
  pop rbp
  ret
