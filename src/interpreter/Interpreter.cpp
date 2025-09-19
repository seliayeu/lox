#include "Expr.hpp"
#include "Stmt.hpp"
#include "RuntimeError.hpp"
#include "TokenType.hpp"
#include "Environment.hpp"
#include "LoxCallable.hpp"
#include "ReturnException.hpp"
#include "LoxFunction.hpp"
#include <any>
#include <vector>
#include <cmath>
#include <sstream>
#include <memory>
#include "error.hpp"


std::any Interpreter::visitLiteralExpr(Literal &expr) {
  return expr.value;
}
std::any Interpreter::visitGroupingExpr(Grouping &expr) {
  return evaluate(expr.expr);
}
std::any Interpreter::visitUnaryExpr(Unary &expr) {
  std::any right = evaluate(expr.right);
  switch (expr.op.type) {
    case TokenType::MINUS:
      checkNumberOperand(expr.op, right);
      return -(std::any_cast<double>(right));
    case TokenType::BANG:
      return !isTruthy(right);
    default:
      break;
  }
  return (void*) nullptr;
}
std::any Interpreter::visitBinaryExpr(Binary &expr) {
  std::any left = evaluate(expr.left);
  std::any right = evaluate(expr.right);

  switch (expr.op.type) {
    case TokenType::MINUS:
      checkNumberOperand(expr.op, left, right);
      return std::any_cast<double>(left) - std::any_cast<double>(right);
    case TokenType::SLASH:
      checkNumberOperand(expr.op, left, right);
      if (std::any_cast<double>(right) == 0)
        throw RuntimeError(expr.op, "Division by 0 not supported.");
      return std::any_cast<double>(left) / std::any_cast<double>(right);
    case TokenType::STAR:
      checkNumberOperand(expr.op, left, right);
      return std::any_cast<double>(left) * std::any_cast<double>(right);
    case TokenType::PLUS:
      if ((left.type() == typeid(double)) && (right.type() == typeid(double)))
        return std::any_cast<double>(left) + std::any_cast<double>(right);
      if ((left.type() == typeid(std::string)) && (right.type() == typeid(std::string)))
        return std::any_cast<std::string>(left) + std::any_cast<std::string>(right);
      if ((left.type() == typeid(std::string)) && (right.type() == typeid(double))) {
        std::ostringstream oss;
        oss << std::any_cast<std::string>(left) << std::any_cast<double>(right);
        return oss.str();
      }
      throw RuntimeError(expr.op, "Operands must be two numbers or two strings.");
    case TokenType::GREATER:
      checkNumberOperand(expr.op, left, right);
      return std::any_cast<double>(left) > std::any_cast<double>(right);
    case TokenType::GREATER_EQUAL:
      checkNumberOperand(expr.op, left, right);
      return std::any_cast<double>(left) >= std::any_cast<double>(right);
    case TokenType::LESS:
      checkNumberOperand(expr.op, left, right);
      return std::any_cast<double>(left) < std::any_cast<double>(right);
    case TokenType::LESS_EQUAL:
      checkNumberOperand(expr.op, left, right);
      return std::any_cast<double>(left) <= std::any_cast<double>(right);
    case TokenType::BANG_EQUAL:
      return !isEqual(left, right);
    case TokenType::EQUAL_EQUAL:
      return isEqual(left, right);
    default: 
      break;
  }

  return (void*) nullptr;
}

std::any Interpreter::visitCallExpr(Call &expr) {
  std::any callee = evaluate(expr.callee);
  std::vector<std::any> args;
  for (auto &arg : expr.arguments)
    args.push_back(evaluate(arg));

  if (callee.type() != typeid(std::shared_ptr<LoxCallable>))
    throw RuntimeError(expr.paren, "Can only call functions and classes.");

  std::shared_ptr<LoxCallable> function = std::any_cast<std::shared_ptr<LoxCallable>>(callee);
  if (args.size() != function->arity()) {
    std::ostringstream oss;
    oss << "Expected " << function->arity() << " arguments but got " << args.size() << ".";
    throw RuntimeError(expr.paren, oss.str());
  }

  return function->call(*this, args);
}

std::any Interpreter::visitVariableExpr(Variable &expr) {
  return environment->get(expr.name);
}

std::any Interpreter::visitAssignExpr(Assign &expr) {
  std::any value = evaluate(expr.value);
  environment->assign(expr.name, value);
  return value;
}

std::any Interpreter::visitExpressionStmt(Expression &stmt) {
  evaluate(stmt.expr);
  return nullptr;
}

std::any Interpreter::visitFunctionStmt(Function &stmt) {
  std::string lexeme = stmt.name.lexeme;
  std::shared_ptr<LoxCallable> function = std::make_shared<LoxFunction>(std::make_shared<Function>(std::move(stmt)), environment);
  environment->define(lexeme, function);
  return nullptr;
}

std::any Interpreter::visitIfStmt(If &stmt) {
  if (isTruthy(evaluate(stmt.condition)))
    execute(stmt.thenBranch);
  else if (stmt.elseBranch != nullptr)
    execute(stmt.elseBranch);
  return nullptr;
}

std::any Interpreter::visitWhileStmt(While &stmt) {
  while (isTruthy(evaluate(stmt.condition)))
    execute(stmt.body);
  return nullptr;
}

std::any Interpreter::visitPrintStmt(Print &stmt) {
  std::any val = evaluate(stmt.expr);
  std::cout << stringify(val) << std::endl;
  return nullptr;
}

std::any Interpreter::visitLogicalExpr(Logical &expr) {
  std::any left = evaluate(expr.left);
  if (expr.op.type == TokenType::OR) {
    if (isTruthy(left))
      return left;
  } else {
    if (!isTruthy(left))
      return left;
  }
  return evaluate(expr.right);
}

std::any Interpreter::visitVarStmt(Var &stmt) {
  std::any val = (void*) nullptr;
  if (stmt.initializer != nullptr)
    val = evaluate(stmt.initializer);
  environment->define(stmt.name.lexeme, val);
  return nullptr;
}

std::any Interpreter::visitBlockStmt(Block &stmt) {
  executeBlock(stmt.statements, std::make_shared<Environment>(Environment(environment)));
  return nullptr;
}

std::any Interpreter::visitReturnStmt(Return &stmt) {
  std::any value{ nullptr };
  if (value.type() != typeid(void*))
    value = evaluate(stmt.value);
  
  throw ReturnException(value);
}

void Interpreter::interpret(std::vector<std::shared_ptr<Stmt>> &statements) {
  try {
    for (std::shared_ptr<Stmt> &stmt : statements)
      execute(stmt);
  } catch (RuntimeError error) {
    runtimeError(error);
  }
}

void Interpreter::executeBlock(std::vector<std::shared_ptr<Stmt>> &statements, std::shared_ptr<Environment> environment) {
  std::shared_ptr<Environment> previous = this->environment;
  try {
    this->environment = environment;
    for (auto &stmt : statements) {
      execute(stmt);
    }
  } catch (ReturnException& re) {
    this->environment = previous;
    throw;
  } catch (std::runtime_error &e) {
    this->environment = previous;
    throw;
  }
  this->environment = previous;
}
std::any Interpreter::evaluate(std::shared_ptr<Expr> &expr) {
  return expr->accept(*this);
}
bool Interpreter::isTruthy(std::any value) {
  if (value.type() == typeid(void*))
    return false;
  if (value.type() == typeid(bool))
    return std::any_cast<bool>(value);
  return true;
}
bool Interpreter::isEqual(std::any a, std::any b) {
  if (a.type() != b.type())
    return false;
  if (a.type() == typeid(double))
    return std::any_cast<double>(a) == std::any_cast<double>(b);
  if (a.type() == typeid(int))
    return std::any_cast<int>(a) == std::any_cast<int>(b);
  if (a.type() == typeid(std::string))
    return std::any_cast<std::string>(a) == std::any_cast<std::string>(b);
  if (a.type() == typeid(void*))
    return true;
  return false;
}
void Interpreter::checkNumberOperand(Token op, std::any operand) {
  if (operand.type() == typeid(double))
    return;
  throw RuntimeError(op, "Operand must be a number.");
}
void Interpreter::checkNumberOperand(Token op, std::any operand1, std::any operand2) {
  if (operand1.type() == typeid(double) && operand2.type() == typeid(double))
    return;
  throw RuntimeError(op, "Operands must be numbers.");
}
std::string Interpreter::stringify(std::any value) {
  if (value.type() == typeid(void*))
    return "nil";
  if (value.type() == typeid(double)) {
    double num = std::any_cast<double>(value);
    std::ostringstream oss;
    if (num == std::floor(num))
      oss << std::defaultfloat << num;
    else
      oss << num;
    return oss.str();
  }
  if (value.type() == typeid(std::string)) {
    return std::any_cast<std::string>(value);
  }
  if (value.type() == typeid(int)) {
    return std::to_string(std::any_cast<int>(value));
  }
  if (value.type() == typeid(bool)) {
    return std::any_cast<bool>(value) ? "true" : "false";
  }
  if (value.type() == typeid(LoxFunction)) {
    std::ostringstream oss;
    LoxFunction func = std::any_cast<LoxFunction>(value);
    oss << func;
    return oss.str();
  }
  return "nil";
}
void Interpreter::execute(std::shared_ptr<Stmt> &stmt) {
  stmt->accept(*this);
}

