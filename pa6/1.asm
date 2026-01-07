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
    addrFormat  db "Address %08X = %08X", 0ah, 0

.code

; ShowParams 过程：显示调用者过程的参数地址和值
; 参数：paramCount - 调用者过程的参数个数
ShowParams PROC paramCount:DWORD
    push ebx
    push esi
    push edi
    
    ; 获取调用者的 EBP（ShowParams 的 EBP 指向的位置存储着调用者保存的 EBP）
    ; [ebp] = 调用者的 EBP (MySample 的 EBP)
    ; 调用者的第一个参数在 [调用者EBP + 8] 位置
    mov eax, [ebp]
    add eax, 8
    mov esi, eax            ; esi 指向调用者的第一个参数
    
    mov edi, paramCount     ; 使用 edi 作为循环计数器（edi 在 C 调用中保留）
    
PrintLoop:
    test edi, edi
    jz Done
    
    mov ebx, [esi]          ; 获取参数值
    
    ; 调用 printf 输出地址和值
    INVOKE crt_printf, ADDR addrFormat, esi, ebx
    
    add esi, 4              ; 移动到下一个参数
    dec edi
    jmp PrintLoop
    
Done:
    pop edi
    pop esi
    pop ebx
    ret
ShowParams ENDP

; MySample 过程：调用 ShowParams 的示例过程
MySample PROC first:DWORD, second:DWORD, third:DWORD
    paramCount = 3
    INVOKE ShowParams, paramCount
    ret
MySample ENDP

main PROC
    ; 调用 MySample，传递三个参数
    INVOKE MySample, 1234h, 5000h, 6543h
    
    ; 退出程序
    INVOKE ExitProcess, 0
main ENDP

END main
