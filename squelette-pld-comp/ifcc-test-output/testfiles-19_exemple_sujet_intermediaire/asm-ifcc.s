.globl main
 main: 
    pushq %rbp
    movq %rsp, %rbp
    subq $24, %rsp
bb0:
    movl $17, -12(%rbp)
    movl -12(%rbp), %eax
    movl %eax, -16(%rbp)
    movl $42, -20(%rbp)
    movl -20(%rbp), %eax
    movl %eax, -24(%rbp)
    movl -16(%rbp), %eax
    imull -16(%rbp), %eax
    movl %eax, -28(%rbp)
    movl -24(%rbp), %eax
    imull -24(%rbp), %eax
    movl %eax, -32(%rbp)
    movl -28(%rbp), %eax
    addl -32(%rbp), %eax
    movl %eax, -36(%rbp)
    movl $1, -40(%rbp)
    movl -36(%rbp), %eax
    addl -40(%rbp), %eax
    movl %eax, -44(%rbp)
    movl -44(%rbp), %eax
    movl %eax, -48(%rbp)
    movl -48(%rbp), %eax
    movq %rbp, %rsp
    popq %rbp
    ret
.section .note.GNU-stack,"",@progbits
