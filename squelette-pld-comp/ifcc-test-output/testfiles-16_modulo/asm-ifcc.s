.globl main
 main: 
    pushq %rbp
    movq %rsp, %rbp
    subq $12, %rsp
bb0:
    movl $16, -4(%rbp)
    movl -4(%rbp), %eax
    movl %eax, -8(%rbp)
    movl $2, -12(%rbp)
    movl -16(%rbp), %eax
    movq %rbp, %rsp
    popq %rbp
    ret
.section .note.GNU-stack,"",@progbits
