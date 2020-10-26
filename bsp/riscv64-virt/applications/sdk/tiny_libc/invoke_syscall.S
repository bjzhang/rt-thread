#include <asm.h>

ENTRY(invoke_syscall)
  /* TODO */
  /* syscall have a different calling 
   * convention, adjust register content
   * to make syscall work
   */
  addi a7, a0, 0
  addi a0, a1, 0
  addi a1, a2, 0
  addi a2, a3, 0
  addi a3, a4, 0
  ecall
  jr ra
ENDPROC(invoke_syscall)
