# 汇编与编译原理  PA2: 手写词法分析器

## 1 实验描述

在PA1中，我们已经学习了ANTLR4 g4文件的写法，并利用ANTLR4自动化生成了词法分析器。而在本次PA中，我们希望同学们用 C++ 语言，自行实现一个简单的词法分析器。

* 目标：用 C++17 手动编写一个词法分析器，用于解析一个名为 `DragonLexerGrammar` 的特定词法规则

* 输入：遵循 `DragonLexerGrammar` 规则的源代码字符串（通过标准输入 stdin）

* 输出：识别出的每个词法单元（Token）的序列（通过标准输出 stdout），格式为：

  ```bash
  TOKEN类型 TOKEN内容 at Line 行数.
  ```

* 主要工作：在提供的 C++ 框架代码基础上，补全 `Token` 定义，并实现核心的词法分析逻辑，主要集中在 `DragonLexer.cpp` 文件中

## 2 环境配置

* 环境要求： C++17 开发环境（需安装 CMake，推荐使用 VSCode 或 CLion 作为 IDE）；要安装 Python3

* 使用 WSL2 Ubuntu22.04，执行以下指令配置环境

  ```bash
  sudo apt update
  sudo apt install build-essential cmake git python3
  ```

## 3 实现思路

### 3.1 添加 `SQUOTE` Token 类型

* 目标：在 `Token.hpp` 和 `Token.cpp` 中添加一个框架代码中为实现的 token 类型：`SQUOTE`(单引号)

* 修改 `Token.hpp`: 在 `TokenType` 枚举中，添加 `SQUOTE` 类型

  ```cpp
  // Token.hpp
  enum class TokenType {
    // ... (其他类型)
    PLUS, MINUS, MUL, DIV,
    DOT,
    // TODO: add more
    SQUOTE
  };
  ```

* 修改 `Token.cpp`: 为了让程序能正确地打印出 `SQUOTE` 的名字，需要在 `tokenTypeToString` 函数里的 `names` 哈希表中添加一个条目

  ```cpp
  // Token.cpp
  std::string tokenTypeToString(TokenType t) {
    static const std::unordered_map<TokenType, std::string> names = {
      // ... (其他映射)
      {TokenType::DOT, "DOT"},
      // TODO: add more token types
      {TokenType::SQUOTE, "SQUOTE"} 
    };
    // ...
  }
  ```

### 3.2 实现 `Lexer` 类的基础辅助函数

* 目标：实现 `Lexer` 类的基础辅助函数，词法分析器在输入字符串中移动的基本实现

* 实现 `Lexer::advance()`

  * 将指针 `pos` 前进一位，并更新 `peek` 为新位置的字符
  * 递增 `pos` ；检查 `pos` 是否已经超出了字符串的范围；如果超出了范围，说明已经到达文件末尾，将 `peek` 设置为 `EOF_CHAR` ；如果没有超出范围，就用 `pos` 索引到新字符，更新 `peek`

  ```cpp
  // Lexer.cpp
  void Lexer::advance() {
    if (peek == '\n') ++line;
    // TODO: finish me. The advance() method moves the pointer 'pos' forward by one position. 
    // When the end of the string is reached, set 'peek' to EOF_CHAR.
    // You should use static_cast<unsigned char> to ensure 'peek' stores an unsigned char value.
    pos++; // 指针前进
    if (pos >= input.length()) { // 检查是否到达末尾
      peek = EOF_CHAR;
    } else {
      peek = static_cast<unsigned char>(input[pos]); // 更新 peek
    }
  }
  ```

* 实现 `Lexer::resetPos(int newPos)`

  * 将指针 `pos` 重置到 `newPos`，并更新 `peek`

  ```cpp
  // Lexer.cpp
  void Lexer::resetPos(int newPos) {
    // TODO: finish me. The resetPos(int newPos) method resets 'pos' to newPos 
    // and updates 'peek' accordingly.
    pos = newPos;
    if (pos >= input.length()) {
      peek = EOF_CHAR;
    } else {
      peek = static_cast<unsigned char>(input[pos]); // 更新 peek
    }
  }
  ```

### 3.3 实现 `DragonLexer` 中字符消耗逻辑

* 实现 `DragonLexer::WS()`

  * 消耗掉所有连续的空白字符（空格、`\t`、`\r`、`\n`）

    ```
    WS : [ \t\r\n]+ -> skip;
    ```

  * 逻辑:
    1. 记录开始时的行号
    2. 使用 `std::isspace()` 判断当前 `peek` 是否是空白符
    3. 在一个 `while` 循环中，只要是空白符，就不断调用 `advance()` 向前推进
    4. 循环结束后，返回一个 `WS` 类型的 Token

  ```cpp
  // DragonLexer.cpp
  Token DragonLexer::WS() {
    // TODO: finish me. The WS() method should consume all whitespace characters (spaces, tabs, newlines)
    int start_line = line;
    std::string text;
    do {
        text += static_cast<char>(peek);
        advance();
    } while (std::isspace(peek));
    return Token(TokenType::WS, text, start_line);
  }
  ```

* 实现 `DragonLexer::ID()`

  * 消耗并识别一个标识符（Identifier）或关键字（Keyword）

    ```
    ID : LETTER (LETTER | DIGIT)* ;
    ```

  * 逻辑:
    1. `nextToken` 函数在调用 `ID()` 之前，应确保当前 `peek` 是一个字母
    2. 创建一个字符串，用 `while` 循环不断读取字符，只要当前 `peek` 是字母或数字 (`std::isalnum()`)，就将其追加到字符串中，并调用 `advance()`
    3. 循环结束后，你就得到了一个完整的单词（如 "if", "else", "myVar123"）
    4. 利用框架提供的 `kwTable.getKeyword(word)` 来判断这个单词是关键字还是普通标识符。这个函数会自动返回正确的 Token
    5. 注意： `kwTable` 返回的 Token 行号可能是0，你需要用当前的行号更新它

  ```cpp
  // DragonLexer.cpp
  Token DragonLexer::ID() {
    // TODO: finish me. The ID() method should consume an identifier or keyword.
    int start_line = line;
    std::string text;
    do {
      text += static_cast<char>(peek);
      advance();
    } while (std::isalnum(peek));
  
    Token t = kwTable.getKeyword(text);
    t.line = start_line; // 设置正确的行号
    return t;
  }
  ```

* 实现 `DragonLexer::NUMBER()`

  * 识别 `INT`, `REAL`, `SCI`，并正确处理数字中的单引号 `'`

  * 逻辑 (状态机思想):

    1. 初始假定 Token 类型为 `INT`
    2. 整数部分: 循环读取数字和单引号，存入字符串
    3. 小数部分: 如果遇到 `.`，将类型切换为 `REAL`，存下 `.`，然后继续循环读取数字和单引号
    4. 科学记数法部分: 如果遇到 `e` 或 `E`，这是一个潜在的科学记数法
       - 需要向前窥探（Lookahead）: 不能立即确定。例如 `123E-` 后面必须跟数字才合法
       - 实现方法: 保存当前位置 (`int saved_pos = pos;`)。然后尝试性地解析 `[eE][+-]?DIGITS`。如果成功（即 `[eE]` 之后确实跟了可选的 `[+-]` 和至少一个数字），则确认类型为 `SCI`，并继续前进。如果失败（如 `123Eabc`），则用 `resetPos(saved_pos)` 回溯到 `E` 之前的位置，此时这个 Token 就是 `REAL`（如果之前有小数点）或 `INT`。

  * 注意，单引号不能作为 NUMBER 的结尾

    ```
    fragment DIGITS : [0-9] ('\''? [0-9])* ;
    ```

  ```CPP
  // DragonLexer.cpp
  Token DragonLexer::NUMBER() {
    // TODO: finish me. The NUMBER() method should consume an integer, real number, or scientific notation number.
    int start_line = line;
    std::string text;
    TokenType type = TokenType::INT;
  
    // 消耗整数部分
    while (true) {
      if (std::isdigit(peek)) {
        text += static_cast<char>(peek);
        advance();
      } else if (peek == '\'') {
        // 向前“窥探”一位，只有当单引号后跟数字时，才将其视为数字的一部分
        if (pos + 1 < input.length() && std::isdigit(input[pos + 1])) {
          text += static_cast<char>(peek);
          advance();
        } else {
          // 单引号后不是数字，数字解析到此结束
          break;
        }
      } else {
        // 遇到其他任何非数字、非有效单引号的字符，都结束整数部分的解析
        break;
      }
    }
  
    // 消耗可能的小数部分
    // 检查小数点 '.' 是否为数字的一部分（即后面必须跟数字）
    if (peek == '.' && (pos + 1 < input.length() && std::isdigit(input[pos + 1]))) {
      type = TokenType::REAL;
      text += static_cast<char>(peek);
      advance();
  
      // 消耗小数部分，逻辑与整数部分完全相同
      while (true) {
        if (std::isdigit(peek)) {
          text += static_cast<char>(peek);
          advance();
        } else if (peek == '\'') {
          if (pos + 1 < input.length() && std::isdigit(input[pos + 1])) {
            text += static_cast<char>(peek);
            advance();
          } else {
            break;
          }
        } else {
          break;
        }
      }
    }
  
    // 消耗可能的科学记数法部分
    if (peek == 'e' || peek == 'E') {
      int saved_pos = pos;
      // 注意：一个健壮的回溯也应该保存和恢复行号，以防万一
      int saved_line = line;
  
      std::string sci_part;
      sci_part += static_cast<char>(peek);
      advance();
  
      if (peek == '+' || peek == '-') {
        sci_part += static_cast<char>(peek);
        advance();
      }
  
      // 确认 'e/E' 和可选的 '+/-' 之后紧跟着数字，才是合法的科学记数法
      if (std::isdigit(peek)) {
        type = TokenType::SCI;
        text += sci_part;
  
        // 消耗指数部分，逻辑与整数部分完全相同
        while (true) {
          if (std::isdigit(peek)) {
            text += static_cast<char>(peek);
            advance();
          } else if (peek == '\'') {
            if (pos + 1 < input.length() && std::isdigit(input[pos + 1])) {
              text += static_cast<char>(peek);
              advance();
            } else {
              break;
            }
          } else {
            break;
          }
        }
      } else { // 如果 'e/E' 后面没有跟数字，则回溯
        resetPos(saved_pos);
        line = saved_line; // 恢复行号
      }
    }
  
    return Token(type, text, start_line);
  }
  ```

### 3.4 完善 `DragonLexer::nextToken()` 的分发逻辑

* 根据当前 `peek` 的字符，决定调用哪个具体的解析函数（如 `ID`, `NUMBER`），或者处理哪个操作符

  ```cpp
  // DragonLexer.cpp
  Token DragonLexer::nextToken() {
    if (peek == EOF_CHAR) {
      return Token(TokenType::EOF_T, "EOF", line);
    }
    if (std::isspace(peek)) {
      return WS();
    }
  
    // TODO: finish me. You need to consider when to call ID()
    if (std::isalpha(peek)) { // <--- 调用 ID()
      return ID();
    }
  
    if (std::isdigit(peek)) {
      return NUMBER();
    }
  
    int cur = peek;
    switch (cur) {
      // TODO: finish me. You need to consider EQ, NE, LT, LE, GT, GE
      case '=':
        advance(); return Token(TokenType::EQ, "=", line);
      case '<':
        advance(); // 先消耗 '<'
        if (peek == '=') {
          advance(); // 消耗 '='
          return Token(TokenType::LE, "<=", line);
        }
        if (peek == '>') {
          advance(); // 消耗 '>'
          return Token(TokenType::NE, "<>", line);
        }
        return Token(TokenType::LT, "<", line); // 只是 '<'
      case '>':
        advance(); // 先消耗 '>'
        if (peek == '=') {
          advance(); // 消耗 '='
          return Token(TokenType::GE, ">=", line);
        }
        return Token(TokenType::GT, ">", line); // 只是 '>'
  
      case '\'': // <--- 处理 SQUOTE
          advance(); return Token(TokenType::SQUOTE, "'", line);
  
      // ... (其他 case) ...
    }
    // ...
  }
  ```

  

## 4 运行与测试

* PA2采用 CMake 构建项目，可以自行编译所有的 .cpp 文件。下面以 CMake 构建为例，在项目根目录中执行下面的命令：

  ```bash
  mkdir build
  cd build
  cmake ..
  make -j4
  ```

  此时，可以看到`build/`目录下生成了可执行文件`dragon_lexer`或`dragon_lexer.exe`(Windows)，执行`./dragon_lexer`运行词法分析器。

* 执行以下指令，运行词法分析器

  ```bash
  ./dragon_lexer
  ```

  输入样例

  ```
  123
  0543
  123+xyz
  3<=4
  3abc3
  123.45E-67
  123.45-67
  123.45E-
  123'123'
  123.4'56
  ```

  linux系统中，按 `Ctrl+D` 结束输入，得到输出

  ```
  INT 123 at Line 1.
  INT 0543 at Line 2.
  INT 123 at Line 3.
  PLUS + at Line 3.
  IDENT xyz at Line 3.
  INT 3 at Line 4.
  LE <= at Line 4.
  INT 4 at Line 4.
  INT 3 at Line 5.
  IDENT abc3 at Line 5.
  SCI 123.45E-67 at Line 6.
  REAL 123.45 at Line 7.
  MINUS - at Line 7.
  INT 67 at Line 7.
  REAL 123.45 at Line 8.
  IDENT E at Line 8.
  MINUS - at Line 8.
  INT 123'123 at Line 9.
  SQUOTE ' at Line 9.
  REAL 123.4'56 at Line 10.
  ```

* 在项目根目录运行 `python3 package.py` 生成ZIP压缩包，将压缩包上传到 GradeScope 上