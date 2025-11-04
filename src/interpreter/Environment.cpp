#include <unordered_map>
#include <string>
#include <any>
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
  if (values.find(name.lexeme) != values.end()) {
    values[name.lexeme] = value;
    return;
  }
  if (enclosing != nullptr) {
    enclosing->assign(name, value);
    return;
  }
  throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
}

std::shared_ptr<Environment> Environment::ancestor(size_t distance) {
  auto env{ shared_from_this() };
  for (size_t i = 0; i < distance; ++i)
    env = env->enclosing;

  return env;
}

std::any Environment::getAt(size_t distance, std::string name) {
  return ancestor(distance)->values[name];
}
