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
    targetStr   BYTE "ABCDE", 10 DUP(0)
    sourceStr   BYTE "FGH", 0
    strFormat   db "%s", 0ah, 0

.code

; Str_concat 过程：将源字符串连接到目标字符串后面
; 参数：pTarget - 目标字符串指针，pSource - 源字符串指针
Str_concat PROC pTarget:DWORD, pSource:DWORD
    push esi
    push edi
    
    ; 首先找到目标字符串的结尾（找到 0 终止符）
    mov edi, pTarget
FindEnd:
    mov al, [edi]
    cmp al, 0
    je EndFound
    inc edi
    jmp FindEnd
    
EndFound:
    ; edi 现在指向目标字符串的结尾（0 终止符位置）
    ; 开始复制源字符串
    mov esi, pSource
    
CopyLoop:
    mov al, [esi]       ; 读取源字符串的一个字符
    mov [edi], al       ; 写入目标字符串
    cmp al, 0           ; 检查是否是结束符
    je CopyDone
    inc esi             ; 移动源指针
    inc edi             ; 移动目标指针
    jmp CopyLoop
    
CopyDone:
    pop edi
    pop esi
    ret
Str_concat ENDP

main PROC
    ; 输出源字符串
    INVOKE crt_printf, ADDR strFormat, ADDR sourceStr
    
    ; 输出连接前的目标字符串
    INVOKE crt_printf, ADDR strFormat, ADDR targetStr
    
    ; 调用 Str_concat 进行字符串连接
    INVOKE Str_concat, ADDR targetStr, ADDR sourceStr
    
    ; 输出连接后的目标字符串
    INVOKE crt_printf, ADDR strFormat, ADDR targetStr
    
    ; 退出程序
    INVOKE ExitProcess, 0
main ENDP

END main
