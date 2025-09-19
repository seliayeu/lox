#pragma once
#include "LoxCallable.hpp"
#include "Stmt.hpp"
#include "Interpreter.hpp"
#include "Environment.hpp"

class LoxFunction : public LoxCallable {
  std::shared_ptr<Function> declaration;
  std::shared_ptr<Environment> closure;
public:
  LoxFunction(std::shared_ptr<Function> declaration, std::shared_ptr<Environment> closure) : declaration{ declaration }, closure{ closure } {};
  std::any call(Interpreter &interpreter, std::vector<std::any> arguments) override;
  int arity() override;
  friend std::ostream& operator<<(std::ostream& out, const LoxFunction& function);
};
