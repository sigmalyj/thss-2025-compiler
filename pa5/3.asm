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
    myArray WORD 20 DUP(?)
    format db "%d", 0ah, 0

.code
main PROC
    MOV eax, SIZEOF myArray

    INVOKE crt_printf, ADDR format, eax
    RET
main ENDP
END main
