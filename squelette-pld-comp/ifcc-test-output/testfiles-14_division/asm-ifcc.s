.globl main
 main: 
    pushq %rbp
    movq %rsp, %rbp
    subq $4, %rsp
bb0:
    movl $6, -0(%rbp)
    movl $2, -4(%rbp)
    movl -8(%rbp), %eax
    movq %rbp, %rsp
    popq %rbp
    ret
.section .note.GNU-stack,"",@progbits
