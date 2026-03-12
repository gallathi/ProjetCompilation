.globl main
 main: 
    pushq %rbp
    movq %rsp, %rbp
    subq $8, %rsp
bb0:
    movl $4285027, -4(%rbp)
    movl -4(%rbp), %eax
    movl %eax, -8(%rbp)
    movl -8(%rbp), %eax
    movq %rbp, %rsp
    popq %rbp
    ret
.section .note.GNU-stack,"",@progbits
