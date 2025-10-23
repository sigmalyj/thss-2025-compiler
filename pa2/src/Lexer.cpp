#include "Lexer.hpp"

Lexer::Lexer(const std::string &input)
  : input(input), pos(0), peek(0), line(1) {
  if (input.empty()) {
    peek = EOF_CHAR;
  } else {
    peek = static_cast<unsigned char>(input[0]);
  }
}

void Lexer::advance() {
  if (peek == '\n') ++line;
  ++pos;
  if (pos >= input.size()) {
    peek = EOF_CHAR;
  } else {
    peek = static_cast<unsigned char>(input[pos]);
  }
}

void Lexer::resetPos(int newPos) {
  pos = newPos;
  if (pos >= input.size()) {
    peek = EOF_CHAR;
  } else {
    peek = static_cast<unsigned char>(input[pos]);
  }
}