#pragma once
#include <any>
#include <vector>
#include "Interpreter.hpp"

class LoxCallable {
public:
  virtual size_t arity() = 0;
  virtual std::any call(Interpreter &interpreter, std::vector<std::any> arguments) = 0;
  virtual ~LoxCallable() = 0;
};

inline LoxCallable::~LoxCallable() = default;
