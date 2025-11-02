#pragma once

#include "Expr.hpp"
#include "Interpreter.hpp"
#include "Stmt.hpp"
#include <memory>
#include <any>
#include <vector>

class Resolver : public ExprVisitor, StmtVisitor {
private:
  std::shared_ptr<Interpreter> interpreter;
  std::vector<std::unordered_map<std::string, bool>> scopes {};

  enum class FunctionType {
    NONE,
    FUNCTION
  };

  FunctionType currentFunction{ FunctionType::NONE };

  void beginScope();
  void endScope();
  void declare(Token name);
  void define(Token name);
  void resolveLocal(Expr& expr, Token name);
  void resolveFunction(Function& function, FunctionType type);
public:
  Resolver(std::shared_ptr<Interpreter> interpreter) : interpreter(interpreter) {}
  std::any visitVariableExpr(Variable& expr);
  std::any visitAssignExpr(Assign& expr);
  std::any visitBinaryExpr(Binary& expr);
  std::any visitCallExpr(Call& expr);
  std::any visitGroupingExpr(Grouping& expr);
  std::any visitLiteralExpr(Literal& expr);
  std::any visitUnaryExpr(Unary& expr);
  std::any visitLogicalExpr(Logical& expr);
  std::any visitBlockStmt(Block& stmt);
  std::any visitVarStmt(Var& stmt);
  std::any visitFunctionStmt(Function& stmt);
  std::any visitIfStmt(If& stmt);
  std::any visitExpressionStmt(Expression& stmt);
  std::any visitWhileStmt(While& stmt);
  std::any visitPrintStmt(Print& stmt);
  std::any visitReturnStmt(Return& stmt);
  void resolve(std::vector<std::shared_ptr<Stmt>> statements);
  void resolve(std::shared_ptr<Stmt> stmt);
  void resolve(std::shared_ptr<Expr> expr);
};
