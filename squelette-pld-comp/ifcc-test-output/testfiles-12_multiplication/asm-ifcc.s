.globl main
 main: 
    pushq %rbp
    movq %rsp, %rbp
    subq $12, %rsp
bb0:
    movl $2, -4(%rbp)
    movl $3, -8(%rbp)
    movl -4(%rbp), %eax
    imull -8(%rbp), %eax
    movl %eax, -12(%rbp)
    movl -12(%rbp), %eax
    movl %eax, -16(%rbp)
    movl $2, -20(%rbp)
    movl -20(%rbp), %eax
    negl %eax
    movl %eax, -24(%rbp)
    movl -16(%rbp), %eax
    imull -24(%rbp), %eax
    movl %eax, -28(%rbp)
    movl -28(%rbp), %eax
    movq %rbp, %rsp
    popq %rbp
    ret
.section .note.GNU-stack,"",@progbits
