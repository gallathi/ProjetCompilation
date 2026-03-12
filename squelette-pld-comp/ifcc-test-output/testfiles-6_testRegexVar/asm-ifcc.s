.globl main
 main: 
    pushq %rbp
    movq %rsp, %rbp
    subq $20, %rsp
bb0:
    movl $1, -12(%rbp)
    movl -12(%rbp), %eax
    negl %eax
    movl %eax, -16(%rbp)
    movl -16(%rbp), %eax
    movl %eax, -20(%rbp)
    movl $4, -24(%rbp)
    movl -24(%rbp), %eax
    movl %eax, -36(%rbp)
    movl -20(%rbp), %eax
    movl %eax, -32(%rbp)
    movl -32(%rbp), %eax
    movl %eax, -36(%rbp)
    movl -36(%rbp), %eax
    movq %rbp, %rsp
    popq %rbp
    ret
.section .note.GNU-stack,"",@progbits
