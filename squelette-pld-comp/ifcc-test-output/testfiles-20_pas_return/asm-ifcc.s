.globl main
 main: 
    pushq %rbp
    movq %rsp, %rbp
    subq $8, %rsp
bb0:
    movl $12, -4(%rbp)
    movl -4(%rbp), %eax
    movl %eax, -8(%rbp)
    movq %rbp, %rsp
    popq %rbp
    ret
.section .note.GNU-stack,"",@progbits
