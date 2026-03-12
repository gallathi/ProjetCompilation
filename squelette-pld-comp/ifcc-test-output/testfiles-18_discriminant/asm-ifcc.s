.globl main
 main: 
    pushq %rbp
    movq %rsp, %rbp
    subq $28, %rsp
bb0:
    movl $2, -12(%rbp)
    movl -12(%rbp), %eax
    movl %eax, -16(%rbp)
    movl $3, -20(%rbp)
    movl -20(%rbp), %eax
    movl %eax, -24(%rbp)
    movl $5, -28(%rbp)
    movl -28(%rbp), %eax
    movl %eax, -32(%rbp)
    movl -24(%rbp), %eax
    imull -24(%rbp), %eax
    movl %eax, -36(%rbp)
    movl $4, -40(%rbp)
    movl -40(%rbp), %eax
    imull -16(%rbp), %eax
    movl %eax, -44(%rbp)
    movl -44(%rbp), %eax
    imull -32(%rbp), %eax
    movl %eax, -48(%rbp)
    movl -36(%rbp), %eax
    subl -48(%rbp), %eax
    movl %eax, -52(%rbp)
    movl -52(%rbp), %eax
    movq %rbp, %rsp
    popq %rbp
    ret
.section .note.GNU-stack,"",@progbits
