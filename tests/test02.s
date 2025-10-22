.intel_syntax noprefix
.data
.text
.globl main
main:
  push rbp
  mov rbp, rsp
  sub rsp, 0
  mov rax, 5
  push rax
  mov rax, 3
  push rax
  mov rax, 2
  pop rdi
  imul rax, rdi
  pop rdi
  add rax, rdi
  jmp .L.return
.L.return:
  mov rsp, rbp
  pop rbp
  ret
