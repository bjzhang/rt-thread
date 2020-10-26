#include <asm.h>

ENTRY(_start)
    /* TODO: */
    /* Load the global pointer */
    .option push
    .option norelax
    la gp, __global_pointer$
    .option pop
    /* call main */
    call main
    /* call sys_exit*/
    call sys_exit

dead:
    j dead
END(_start)
