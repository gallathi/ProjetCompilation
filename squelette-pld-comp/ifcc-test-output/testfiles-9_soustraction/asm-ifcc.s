.globl main
 main: 
    pushq %rbp
    movq %rsp, %rbp
    subq $4, %rsp
bb0:
    movl $1, -0(%rbp)
    movl $1, -4(%rbp)
    movl -0(%rbp), %eax
    subl -4(%rbp), %eax
    movl %eax, -8(%rbp)
    movl $1, -12(%rbp)
    movl -8(%rbp), %eax
    addl -12(%rbp), %eax
    movl %eax, -16(%rbp)
    movl $1, -20(%rbp)
    movl -16(%rbp), %eax
    subl -20(%rbp), %eax
    movl %eax, -24(%rbp)
    movl -24(%rbp), %eax
    movq %rbp, %rsp
    popq %rbp
    ret
.section .note.GNU-stack,"",@progbits
