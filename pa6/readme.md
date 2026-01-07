## Task 1 显示过程参数

写一个名称为`ShowParams`的过程，使其能够显示**调用`ShowParams`的过程**的运行时栈（run-time stack）中32位参数的**地址**和**十六进制数值**。

例如，假设我们有以下语句调用了`MySample`，并且传递了三个参数：

```
INVOKE MySample, 1234h, 5000h, 6543h
```

`MySample`内部用以下方法调用了`ShowParams`：

```
MySample PROC first:DWORD, second:DWORD, third:DWORD
paramCount = 3
INVOKE ShowParams, paramCount
```

ShowParams应当以以下格式进行输出：（地址数字仅供参考，但是你输出的地址码之间应差值为4）

```
Address 0012FF80 = 00001234
Address 0012FF84 = 00005000
Address 0012FF88 = 00006543
```

在你提交的代码中，应当包含`ShowParams`过程及上述调用`ShowParams`的示例代码。

## Task 2 字符串连接

写一个名为`Str_concat`的过程，把源字符串（sourceStr）连接在目标字符串（targetStr）之后（调用该过程时，会保证目标字符串中有足够的空间容纳源字符串；实现`Str_concat`时不能调用`sprintf`等字符串处理的C语言库函数以及Irvine提供的`Str_copy`等字符串操作函数）。传递的参数是指向源字符串和目标字符串的指针（请注意，0是字符串结束标志）。下面是一个可能的调用示例：

```
.data
targetStr BYTE "ABCDE",10 DUP(0)
sourceStr BYTE "FGH",0
.code
INVOKE Str_concat, ADDR targetStr, ADDR sourceStr
```

提交的代码中，应当包含`Str_concat`过程及上述调用`Str_concat`的示例代码，要求按顺序分别输出源字符串、目的字符串和连接得到的字符串结果，每个占一行（可以使用 printf 函数进行输出，调用 printf 的语句请勿放在 Str_concat 过程中）。

## Task 3 数组低8位冒泡排序

用汇编程序设计语言实现针对 WORD 数组的`BubbleSort`过程，将该 WORD 数组按照每个元素低八位的大小从小到大的顺序进行排序（即：将数组中每个元素的低8位取出，当作一个无符号整数，作为数组原始元素大小的判断标准）。要求在 BubbleSort 过程中存在一个变量，在内层循环中如果一对值进行了交换，则把该变量置 1，如果在某一遍处理后发现没有交换任何数值，则退出排序过程。调用 BubbleSort 时会传入数组的地址与长度作为参数。

程序需支持在终端动态输入数据

### 输入格式

第一行输入一个整数 , 表示数组中数的个数

随后输入 个整数作为数组元素，数组元素用空格或换行符分隔。

### 输出格式

程序执行后需输出排序后的数组内容，数字间用空格隔开。程序应根据低 8 位排序规则输出处理后的结果。