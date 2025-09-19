#pragma once
#include "Expr.hpp"
#include "Stmt.hpp"
#include "Environment.hpp"
#include <any>
#include <vector>
#include <memory>

class Interpreter : public ExprVisitor, public StmtVisitor {
public:
  std::shared_ptr<Environment> globals { std::make_shared<Environment>() };
private:
  std::shared_ptr<Environment> environment { globals };
public:
  Interpreter() {
    // globals.define("clock", ClockBuiltIn
  };

  std::any visitLiteralExpr(Literal &expr) override;
  std::any visitGroupingExpr(Grouping &expr) override;
  std::any visitUnaryExpr(Unary &expr) override;
  std::any visitBinaryExpr(Binary &expr) override;
  std::any visitCallExpr(Call &expr) override;
  std::any visitVariableExpr(Variable &expr) override;
  std::any visitAssignExpr(Assign &expr) override;
  std::any visitExpressionStmt(Expression &stmt) override;
  std::any visitIfStmt(If &stmt) override;
  std::any visitWhileStmt(While &stmt) override;
  std::any visitPrintStmt(Print &stmt) override;
  std::any visitLogicalExpr(Logical &expr) override;
  std::any visitVarStmt(Var &stmt) override;
  std::any visitFunctionStmt(Function &stmt) override;
  std::any visitBlockStmt(Block &stmt) override;
  std::any visitReturnStmt(Return &stmt) override;

  void interpret(std::vector<std::shared_ptr<Stmt>> &statements);
  void executeBlock(std::vector<std::shared_ptr<Stmt>> &statements, std::shared_ptr<Environment> environment);

  std::any evaluate(std::shared_ptr<Expr> &expr);
  bool isTruthy(std::any value);
  bool isEqual(std::any a, std::any b);
  void checkNumberOperand(Token op, std::any operand);
  void checkNumberOperand(Token op, std::any operand1, std::any operand2);
  std::string stringify(std::any value);
  void execute(std::shared_ptr<Stmt> &stmt);
};
