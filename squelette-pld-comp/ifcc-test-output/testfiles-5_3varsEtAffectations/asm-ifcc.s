.globl main
 main: 
    pushq %rbp
    movq %rsp, %rbp
    subq $20, %rsp
bb0:
    movl $1, -12(%rbp)
    movl -12(%rbp), %eax
    movl %eax, -16(%rbp)
    movl $4, -20(%rbp)
    movl -20(%rbp), %eax
    movl %eax, -32(%rbp)
    movl -16(%rbp), %eax
    movl %eax, -28(%rbp)
    movl -28(%rbp), %eax
    movl %eax, -32(%rbp)
    movl -32(%rbp), %eax
    movq %rbp, %rsp
    popq %rbp
    ret
.section .note.GNU-stack,"",@progbits
