#include <any>
#include <vector>
#include "Interpreter.hpp"
#include "ReturnException.hpp"
#include "LoxFunction.hpp"

std::any LoxFunction::call(Interpreter &interpreter, std::vector<std::any> arguments) {
  std::shared_ptr<Environment> environment{ new Environment(closure) };
  
  for (int i = 0; i < declaration.get()->params.size(); ++i) {
    environment->define(declaration.get()->params[i].lexeme, arguments[i]);
  }

  try {
    interpreter.executeBlock(declaration.get()->body, environment);
  } catch (ReturnException& returnValue) {

      return returnValue.value;
  };
  return nullptr;
}

int LoxFunction::arity() {
  return declaration.get()->params.size();
}

std::ostream& operator<<(std::ostream& out, const LoxFunction& function) {
  std::cout << "<fn " << function.declaration.get()->name.lexeme << ">";
  return out;
}

