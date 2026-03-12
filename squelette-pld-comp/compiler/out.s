.globl main
 main: 
    pushq %rbp
    movq %rsp, %rbp
    subq $20, %rsp
bb0:
    movl $5, -8(%rbp)
    movl $3, -12(%rbp)
    movl -8(%rbp), %eax
    addl -12(%rbp), %eax
    movl %eax, -16(%rbp)
    movl $6, -20(%rbp)
    movl $4, -24(%rbp)
    movl -20(%rbp), %eax
    imull -24(%rbp), %eax
    movl %eax, -28(%rbp)
    movl -16(%rbp), %eax
    subl -28(%rbp), %eax
    movl %eax, -32(%rbp)
    movl -32(%rbp), %eax
    movl %eax, -36(%rbp)
    movl $5, -40(%rbp)
    movl $6, -44(%rbp)
    movl -40(%rbp), %eax
    imull -44(%rbp), %eax
    movl %eax, -48(%rbp)
    movl $47, -52(%rbp)
    movl -48(%rbp), %eax
    addl -52(%rbp), %eax
    movl %eax, -56(%rbp)
    movl -36(%rbp), %eax
    imull -36(%rbp), %eax
    movl %eax, -60(%rbp)
    movl -56(%rbp), %eax
    subl -60(%rbp), %eax
    movl %eax, -64(%rbp)
    movl $2, -68(%rbp)
    movl $6, -72(%rbp)
    movl -68(%rbp), %eax
    imull -72(%rbp), %eax
    movl %eax, -76(%rbp)
    movl -64(%rbp), %eax
    addl -76(%rbp), %eax
    movl %eax, -80(%rbp)
    movl -80(%rbp), %eax
    movl %eax, -84(%rbp)
    movl -84(%rbp), %eax
    subl -36(%rbp), %eax
    movl %eax, -88(%rbp)
    movl -88(%rbp), %eax
    movq %rbp, %rsp
    popq %rbp
    ret
.section .note.GNU-stack,"",@progbits
