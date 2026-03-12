.globl main
 main: 
    pushq %rbp
    movq %rsp, %rbp
    subq $8, %rsp
bb0:
    movl $1, -4(%rbp)
    movl $3, -8(%rbp)
    movl -4(%rbp), %eax
    addl -8(%rbp), %eax
    movl %eax, -12(%rbp)
    movl -12(%rbp), %eax
    movl %eax, -16(%rbp)
    movl -16(%rbp), %eax
    movq %rbp, %rsp
    popq %rbp
    ret
.section .note.GNU-stack,"",@progbits
