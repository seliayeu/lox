#pragma once
#include "Expr.hpp"
#include "Stmt.hpp"
#include "RuntimeError.hpp"
#include "TokenType.hpp"
#include "Environment.hpp"
#include <any>
#include <vector>
#include <cmath>
#include <sstream>
#include <memory>
#include "error.hpp"

struct Interpreter : public ExprVisitor, public StmtVisitor {
  std::any visitLiteralExpr(Literal &expr) override {
    return expr.value;
  }
  std::any visitGroupingExpr(Grouping &expr) override {
    return evaluate(expr.expr);
  }
  std::any visitUnaryExpr(Unary &expr) override {
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
  std::any visitBinaryExpr(Binary &expr) override {
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
        throw RuntimeError(expr.op, "Operands must two numbers or two strings.");
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

  std::any visitVariableExpr(Variable &expr) override {
    return environment->get(expr.name);
  }

  std::any visitAssignExpr(Assign &expr) override {
    std::any value = evaluate(expr.value);
    environment->assign(expr.name, value);
    return value;
  }

  std::any visitExpressionStmt(Expression &stmt) override {
    evaluate(stmt.expr);
    return nullptr;
  }

  std::any visitPrintStmt(Print &stmt) override {
    std::any val = evaluate(stmt.expr);
    std::cout << stringify(val) << std::endl;
    return nullptr;
  }

  std::any visitVarStmt(Var &stmt) override {
    std::any val = (void*) nullptr;
    if (stmt.initializer != nullptr)
      val = evaluate(stmt.initializer);
    environment->define(stmt.name.lexeme, val);
    return nullptr;
  }

  std::any visitBlockStmt(Block &stmt) override {
    executeBlock(stmt.statements, std::make_shared<Environment>(environment));
    return nullptr;
  }

  void interpret(std::vector<std::unique_ptr<Stmt>> &statements) {
    try {
      for (std::unique_ptr<Stmt> &stmt : statements)
        execute(stmt);
    } catch (RuntimeError error) {
      runtimeError(error);
    }
  }

  void executeBlock(std::vector<std::unique_ptr<Stmt>> &statements, std::shared_ptr<Environment> environment) {
    std::shared_ptr<Environment> previous = this->environment;
    try {
      this->environment = environment;
      for (auto &stmt : statements)
        execute(stmt);
    } catch (std::runtime_error &e) {
      this->environment = previous;
      throw e;
    }
    this->environment = previous;
  }
private:
  std::shared_ptr<Environment> environment = std::make_shared<Environment>();

  std::any evaluate(std::unique_ptr<Expr> &expr) {
    return expr->accept(*this);
  }
  bool isTruthy(std::any value) {
    if (value.type() == typeid(void*))
      return false;
    if (value.type() == typeid(bool))
      return std::any_cast<bool>(value);
    return true;
  }
  bool isEqual(std::any a, std::any b) {
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
  void checkNumberOperand(Token op, std::any operand) {
    if (operand.type() == typeid(double))
      return;
    throw RuntimeError(op, "Operand must be a number.");
  }
  void checkNumberOperand(Token op, std::any operand1, std::any operand2) {
    if (operand1.type() == typeid(double) && operand2.type() == typeid(double))
      return;
    throw RuntimeError(op, "Operands must be numbers.");
  }
  std::string stringify(std::any value) {
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
    return "nil";
  }
  void execute(std::unique_ptr<Stmt> &stmt) {
    stmt->accept(*this);
  }
};
