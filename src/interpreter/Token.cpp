#pragma once

#include <any>
#include <string>
#include "Token.hpp"

std::string Token::literalAsString() const {
  if (literal.type() == typeid(int)) {
    return std::to_string(std::any_cast<int>(literal));
  } else if (literal.type() == typeid(double)) {
    return std::to_string(std::any_cast<double>(literal));
  } else if (literal.type() == typeid(std::string)) {
    return std::any_cast<std::string>(literal);
  }
  return lexeme;
}
