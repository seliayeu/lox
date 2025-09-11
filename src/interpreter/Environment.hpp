#pragma once
#include <unordered_map>
#include <string>
#include <any>
#include <memory>
#include "Token.hpp"

class Environment {
  std::unordered_map<std::string, std::any> values;
public:
  std::shared_ptr<Environment> enclosing;

  Environment(std::shared_ptr<Environment> &enclosing) : enclosing { enclosing } {}
  Environment() {}

  void define(std::string name, std::any value);
  std::any get(Token name);
  void assign(Token name, std::any value);
};
