#pragma once
#include <unordered_map>
#include <string>
#include <any>
#include <memory>
#include "Token.hpp"
#include "RuntimeError.hpp"

struct Environment {
  std::shared_ptr<Environment> enclosing;

  Environment(std::shared_ptr<Environment> &enclosing) : enclosing { enclosing } {}
  Environment() {}

  void define(std::string name, std::any value) {
    values[name] = value;
  }
  std::any get(Token name) {
    if (values.find(name.lexeme) != values.end())
      return values.at(name.lexeme);
    if (enclosing != nullptr)
      return enclosing->get(name);
    throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
  }
  void assign(Token name, std::any value) {
    if (values.find(name.lexeme) != values.end())
      values[name.lexeme] = value;
    if (enclosing != nullptr)
      return enclosing->assign(name, value);
    throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
  }
private:
  std::unordered_map<std::string, std::any> values;

};
