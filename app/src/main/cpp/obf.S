    .text
    .global raw_syscall
    .type raw_syscall,%function


raw_syscall:
    mov $0x1, %eax
    mov $0x2 , %ebx
    mov $0x3 ,%ecx
    mov $0x4 ,%edx
    cmp $0x1 ,%eax
    JNZ b1
    add $0x1 , %eax
    mov %eax , %ecx
    jmp END
b1:
    add %ebx , %eax
    mov %eax , %edx
    mov $0x2 , %eax
END:
    add %ebx , %ecx
    mov %ecx , %eax
    ret


