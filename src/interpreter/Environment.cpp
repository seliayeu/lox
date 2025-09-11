#include <unordered_map>
#include <string>
#include <any>
#include <memory>
#include "Token.hpp"
#include "RuntimeError.hpp"
#include "Environment.hpp"

void Environment::define(std::string name, std::any value) {
  values[name] = value;
}

std::any Environment::get(Token name) {
  if (values.find(name.lexeme) != values.end())
    return values.at(name.lexeme);
  if (enclosing != nullptr)
    return enclosing->get(name);
  throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
}

void Environment::assign(Token name, std::any value) {
  if (values.find(name.lexeme) != values.end())
    values[name.lexeme] = value;
  if (enclosing != nullptr)
    return enclosing->assign(name, value);
  throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
}
