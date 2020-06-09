[BITS 64]

global unpack:function
global g_unpack_len:data
global g_unpack_offset:data

g_unpack_len dd end - unpack
g_unpack_offset dd start - unpack

unpack:
  woody db "....WOODY.....", 10
  len equ $ - woody
start:
  push rbp
  mov rbp, rsp
  sub rsp, 16
  mov rdi, 1
  lea rsi, [rel woody]
  mov rdx, len
  mov rax, 0x01
  syscall
  add rsp, 16
  pop rbp
  jmp 0xffffffff
  ret
end: