.globl main
 main: 
    pushq %rbp
    movq %rsp, %rbp
    subq $20, %rsp
bb0:
    movl $5, -8(%rbp)
    movl $2, -12(%rbp)
    movl -12(%rbp), %eax
    negl %eax
    movl %eax, -16(%rbp)
    movl $1, -20(%rbp)
    movl -20(%rbp), %eax
    negl %eax
    movl %eax, -24(%rbp)
    movl -16(%rbp), %eax
    addl -24(%rbp), %eax
    movl %eax, -28(%rbp)
    movl -8(%rbp), %eax
    subl -28(%rbp), %eax
    movl %eax, -32(%rbp)
    movl -32(%rbp), %eax
    movl %eax, -36(%rbp)
    movl $2, -40(%rbp)
    movl $1, -44(%rbp)
    movl -40(%rbp), %eax
    subl -44(%rbp), %eax
    movl %eax, -48(%rbp)
    movl -48(%rbp), %eax
    negl %eax
    movl %eax, -52(%rbp)
    movl $5, -56(%rbp)
    movl -52(%rbp), %eax
    subl -56(%rbp), %eax
    movl %eax, -60(%rbp)
    movl -60(%rbp), %eax
    movl %eax, -64(%rbp)
    movl -64(%rbp), %eax
    negl %eax
    movl %eax, -68(%rbp)
    movl -36(%rbp), %eax
    subl -68(%rbp), %eax
    movl %eax, -72(%rbp)
    movl -72(%rbp), %eax
    movq %rbp, %rsp
    popq %rbp
    ret
.section .note.GNU-stack,"",@progbits
