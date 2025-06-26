#include <any>
#include <iostream>
#include <string>
#include "TokenType.hpp"

#ifndef TOKEN
#define TOKEN
class Token {

public:
  const TokenType type;
  const std::string lexeme;
  const std::any literal;
  const int line;

  Token(TokenType type, std::string lexeme, std::any literal, int line)
      : type(type), lexeme(lexeme), literal(literal), line(line){};

  std::string literalAsString() const {
    if (literal.type() == typeid(int)) {
      return std::to_string(std::any_cast<int>(literal));
    } else if (literal.type() == typeid(double)) {
      return std::to_string(std::any_cast<double>(literal));
    } else if (literal.type() == typeid(std::string)) {
      return std::any_cast<std::string>(literal);
    }
    return lexeme;
  }

  friend std::ostream &operator<<(std::ostream &os, const Token &t) {
    return std::cout << t.type << " " << t.lexeme << " " << t.literalAsString();
  }
};
#endif
