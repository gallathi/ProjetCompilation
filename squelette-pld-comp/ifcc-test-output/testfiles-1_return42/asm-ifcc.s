.globl main
 main: 
    pushq %rbp
    movq %rsp, %rbp
    subq $4, %rsp
bb0:
    movl $42, -0(%rbp)
    movl -0(%rbp), %eax
    movq %rbp, %rsp
    popq %rbp
    ret
.section .note.GNU-stack,"",@progbits
