.intel_syntax noprefix
.data
.text
.globl main
main:
  push rbp
  mov rbp, rsp
  sub rsp, 0
  mov rax, 42
  jmp .L.return
.L.return:
  mov rsp, rbp
  pop rbp
  ret
