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
    vala DWORD 0h
    valb SWORD 0h
    valc BYTE 0h
    format db "%d %d %d", 0ah, 0

.code
main PROC
    MOV eax, SIZEOF vala
    MOV ebx, SIZEOF valb
    MOV ecx, SIZEOF valc

    INVOKE crt_printf, ADDR format, eax, ebx, ecx
    RET
main ENDP
END main
