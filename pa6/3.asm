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
    array       WORD 1000 DUP(0)    ; 数组最大支持1000个元素
    arrayLen    DWORD 0
    inputFormat db "%hd", 0          ; 读取 WORD 格式
    countFormat db "%d", 0           ; 读取数组长度
    outFormat   db "%d ", 0          ; 输出格式
    newLine     db 0ah, 0

.code

; BubbleSort 过程：对 WORD 数组按低8位大小进行冒泡排序
; 参数：pArray - 数组地址，count - 数组元素个数
BubbleSort PROC pArray:DWORD, count:DWORD
    LOCAL swapped:DWORD
    
    push ebx
    push ecx
    push edx
    push esi
    push edi
    
    mov ecx, count
    dec ecx                     ; 外层循环次数 = count - 1
    cmp ecx, 0
    jle SortDone
    
OuterLoop:
    mov swapped, 0              ; 初始化交换标志为0
    mov esi, pArray             ; esi 指向数组起始
    mov edx, ecx                ; edx = 内层循环次数
    
InnerLoop:
    cmp edx, 0
    jle CheckSwapped
    
    ; 获取当前元素和下一个元素
    movzx eax, WORD PTR [esi]       ; 当前元素
    movzx ebx, WORD PTR [esi + 2]   ; 下一个元素
    
    ; 提取低8位进行比较
    mov edi, eax
    and edi, 0FFh               ; 当前元素的低8位
    push eax
    mov eax, ebx
    and eax, 0FFh               ; 下一个元素的低8位
    
    ; 比较低8位
    cmp edi, eax
    pop eax
    jbe NoSwap                  ; 如果 当前 <= 下一个，不交换
    
    ; 交换两个元素
    mov WORD PTR [esi], bx
    mov WORD PTR [esi + 2], ax
    mov swapped, 1              ; 设置交换标志
    
NoSwap:
    add esi, 2                  ; 移动到下一个元素
    dec edx
    jmp InnerLoop
    
CheckSwapped:
    cmp swapped, 0
    je SortDone                 ; 如果没有交换，提前退出
    dec ecx
    cmp ecx, 0
    jg OuterLoop
    
SortDone:
    pop edi
    pop esi
    pop edx
    pop ecx
    pop ebx
    ret
BubbleSort ENDP

main PROC
    LOCAL n:DWORD
    LOCAL i:DWORD
    LOCAL tempVal:DWORD
    
    ; 读取数组长度
    lea eax, n
    push eax
    push offset countFormat
    call crt_scanf
    add esp, 8
    
    ; 读取数组元素
    mov i, 0
    lea esi, array
    
ReadLoop:
    mov eax, i
    cmp eax, n
    jge ReadDone
    
    ; 读取一个 WORD 值
    lea eax, tempVal
    push eax
    push offset countFormat     ; 使用 %d 读取
    call crt_scanf
    add esp, 8
    
    ; 存储到数组中
    mov eax, tempVal
    mov WORD PTR [esi], ax
    
    add esi, 2
    inc i
    jmp ReadLoop
    
ReadDone:
    ; 调用 BubbleSort 进行排序
    INVOKE BubbleSort, ADDR array, n
    
    ; 输出排序后的数组
    mov i, 0
    lea esi, array
    
PrintLoop:
    mov eax, i
    cmp eax, n
    jge PrintDone
    
    movzx eax, WORD PTR [esi]
    push eax
    push offset outFormat
    call crt_printf
    add esp, 8
    
    add esi, 2
    inc i
    jmp PrintLoop
    
PrintDone:
    ; 输出换行
    push offset newLine
    call crt_printf
    add esp, 4
    
    ; 退出程序
    INVOKE ExitProcess, 0
main ENDP

END main
