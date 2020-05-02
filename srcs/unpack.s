
section .data
  woody db "....WOODY.....", 10
  len   equ $ - woody

section .text

start:
  call _unpack
  ret

_unpack:
  mov rdi, 1
  lea rsi, [rel woody]
  mov rdx, len
  mov rax, 0x01
  syscall
  jmp
  ret

end:
  pop rbp
  ret
