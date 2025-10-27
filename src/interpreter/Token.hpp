#pragma once

#include <any>
#include <iostream>
#include <string>
#include "TokenType.hpp"
#include <utility>

class Token {
public:
  const TokenType type;
  const std::string lexeme;
  const std::any literal;
  const size_t line;

  Token(TokenType type, std::string lexeme, std::any literal, size_t line)
      : type(type), lexeme(lexeme), literal(literal), line(line){};

  std::string literalAsString() const;
  friend std::ostream &operator<<([[maybe_unused]] std::ostream &os, const Token &t) {
    return std::cout << std::to_underlying(t.type) << " " << t.lexeme << " " << t.literalAsString();
  }
};
