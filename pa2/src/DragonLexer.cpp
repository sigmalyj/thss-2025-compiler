#include "DragonLexer.hpp"
#include "Token.hpp"
#include <cctype>
#include <string>

DragonLexer::DragonLexer(const std::string &input)
  : Lexer(input) {}

Token DragonLexer::nextToken() {
  if (peek == EOF_CHAR) {
    return Token(TokenType::EOF_T, "EOF", line);
  }

  if (std::isspace(peek)) {
    return WS();
  }

  // 标识符或关键字
  if (std::isalpha(peek)) {
    return ID();
  }

  // 数字相关
  if (std::isdigit(peek)) {
    return NUMBER();
  }

  int cur = peek;
  switch (cur) {
    // 运算符和分隔符
    case '=':
      advance();
      return Token(TokenType::EQ, "=", line);
    case '<':
      advance();
      if (peek == '=') {
        advance();
        return Token(TokenType::LE, "<=", line);
      } else if (peek == '>') {
        advance();
        return Token(TokenType::NE, "<>", line);
      }
      return Token(TokenType::LT, "<", line);
    case '>':
      advance();
      if (peek == '=') {
        advance();
        return Token(TokenType::GE, ">=", line);
      }
      return Token(TokenType::GT, ">", line);
    case '(':
      advance(); return Token(TokenType::L_PAREN, "(", line);
    case ')':
      advance(); return Token(TokenType::R_PAREN, ")", line);
    case '{':
      advance(); return Token(TokenType::L_BRACE, "{", line);
    case '}':
      advance(); return Token(TokenType::R_BRACE, "}", line);
    case '[':
      advance(); return Token(TokenType::L_BRACKET, "[", line);
    case ']':
      advance(); return Token(TokenType::R_BRACKET, "]", line);
    case ',':
      advance(); return Token(TokenType::COMMA, ",", line);
    case ';':
      advance(); return Token(TokenType::SEMI, ";", line);
    case '+':
      advance(); return Token(TokenType::PLUS, "+", line);
    case '-':
      advance(); return Token(TokenType::MINUS, "-", line);
    case '*':
      advance(); return Token(TokenType::MUL, "*", line);
    case '/':
      advance(); return Token(TokenType::DIV, "/", line);
    case '.':
      advance(); return Token(TokenType::DOT, ".", line);
    case '\'':
      advance(); return Token(TokenType::SQUOTE, "'", line);
    default:
      char ch = static_cast<char>(peek);
      std::string s(1, ch);
      advance();
      return Token(TokenType::UNKNOWN, s, line);
  }
}

Token DragonLexer::WS() {
  int startLine = line;
  std::string ws;
  while (std::isspace(peek)) {
    ws += static_cast<char>(peek);
    advance();
  }
  return Token(TokenType::WS, ws, startLine);
}

Token DragonLexer::ID() {
  int startLine = line;
  std::string id;
  while (std::isalpha(peek) || std::isdigit(peek)) {
    id += static_cast<char>(peek);
    advance();
  }
  if (id == "if") {
    return Token(TokenType::IF, id, startLine);
  } else if (id == "else") {
    return Token(TokenType::ELSE, id, startLine);
  }
  return Token(TokenType::ID, id, startLine);
}

Token DragonLexer::NUMBER() {
  int startLine = line;
  std::string num;
  bool hasDot = false;
  bool hasExp = false;

  // 处理整数部分，允许数字间有单引号，但不能以单引号结尾
  bool lastIsDigit = false;
  while (std::isdigit(peek) || (peek == '\'' && lastIsDigit)) {
    if (peek == '\'') {
      if (!lastIsDigit) break;
      num += static_cast<char>(peek);
      advance();
      lastIsDigit = false;
      continue;
    }
    num += static_cast<char>(peek);
    advance();
    lastIsDigit = true;
  }

  // 如果最后一个字符是单引号，回退（不把结尾的单引号吞掉）
  if (!num.empty() && num.back() == '\'') {
    num.pop_back();
    resetPos(pos - 1);
    lastIsDigit = true;
  }

  // 检查小数点（只有当 '.' 后面有数字时才作为小数点）
  if (peek == '.') {
    int savePos = pos; // 保存 '.' 的位置
    advance(); // 目前 peek 已移到 '.' 后的字符
    if (std::isdigit(peek)) {
      hasDot = true;
      num += '.';
      // 处理小数部分，允许数字间有单引号，但不能以单引号结尾
      bool fracLastIsDigit = false;
      while (std::isdigit(peek) || (peek == '\'' && fracLastIsDigit)) {
        if (peek == '\'') {
          if (!fracLastIsDigit) break;
          num += static_cast<char>(peek);
          advance();
          fracLastIsDigit = false;
          continue;
        }
        num += static_cast<char>(peek);
        advance();
        fracLastIsDigit = true;
      }
      if (!num.empty() && num.back() == '\'') {
        // 回退最后的单引号到输入流，让下一个 token 处理它
        num.pop_back();
        resetPos(pos - 1);
      }
    } else {
      // 不是合法小数，回退到 '.' 未被吞
      resetPos(savePos);
    }
  }

  // 检查科学计数法（只有在 e/E 后面有合法的指数数字时才吞并）
  if (peek == 'e' || peek == 'E') {
    int savePos = pos; // 指向 'e' 或 'E'
    advance(); // 吞掉 'e' / 'E' 暂时向前看
    std::string expPart;
    if (peek == '+' || peek == '-') {
      expPart += static_cast<char>(peek);
      advance();
    }
    int expStartPos = pos; // 记录指数数字部分的起始位置
    bool expLastIsDigit = false;
    bool expHasDigit = false;
    while (std::isdigit(peek) || (peek == '\'' && expLastIsDigit)) {
      if (peek == '\'') {
        if (!expLastIsDigit) break;
        expPart += static_cast<char>(peek);
        advance();
        expLastIsDigit = false;
        continue;
      }
      expPart += static_cast<char>(peek);
      advance();
      expLastIsDigit = true;
      expHasDigit = true;
    }

    // 如果指数部分以单引号结尾，则该单引号无效，需要回退
    if (!expPart.empty() && expPart.back() == '\'' && !std::isdigit(peek)) {
        expPart.pop_back();
        resetPos(pos - 1);
    }

    if (expHasDigit) {
      hasExp = true;
      num += static_cast<char>(input[savePos]); // 加入 'e' 或 'E'
      num += expPart;
    } else {
      // 指数部分不合法，回退到 'e' 的位置（让后续作为 IDENT 处理）
      resetPos(savePos);
    }
  }
  
  if (hasExp) {
    return Token(TokenType::SCI, num, startLine);
  } else if (hasDot) {
    return Token(TokenType::REAL, num, startLine);
  } else {
    return Token(TokenType::INT, num, startLine);
  }
}
