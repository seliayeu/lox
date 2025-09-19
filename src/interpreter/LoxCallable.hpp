#pragma once
#include <any>
#include <vector>
#include "Interpreter.hpp"

class LoxCallable {
public:
  virtual int arity() = 0;
  virtual std::any call(Interpreter &interpreter, std::vector<std::any> arguments) = 0;
};
