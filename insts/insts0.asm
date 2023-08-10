sub sp, sp, #8
str r0, [sp, #4]
mov r0, #2
str r0, [sp]
mov r0, #8
str r0, [sp]
ldr r0, [sp, #4]
mul r1, r0, r0
mov r0, r1
hlt
