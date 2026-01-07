.386
.MODEL flat,stdcall
.STACK 4096
option casemap:none

include     windows.inc
include     kernel32.inc
includelib  kernel32.lib
include     msvcrt.inc
includelib  msvcrt.lib

.data
    varA     DWORD 5
    varB     DWORD 6
    sum      DWORD 0
    product  DWORD 0
    format   db "%d %d", 0ah, 0

.code
main PROC
    MOV eax, varA
    ADD eax, varB
    MOV sum, eax

    MOV eax, varA
    IMUL eax, varB
    MOV product, eax

    INVOKE crt_printf, ADDR format, sum, product
    RET
main ENDP
END main
