#pragma once

#include <vector>
#include <any>
#include "Token.hpp"
#include "Expr.hpp"

class StmtVisitor;

class Stmt {
public:
  virtual std::any accept(StmtVisitor &visitor) = 0;
  virtual ~Stmt() = default;
};

class Block;
class Var;
class Expression;
class Print;

class StmtVisitor {
public:
  virtual std::any visitBlockStmt(Block &stmt) = 0;
  virtual std::any visitVarStmt(Var &stmt) = 0;
  virtual std::any visitExpressionStmt(Expression &stmt) = 0;
  virtual std::any visitPrintStmt(Print &stmt) = 0;
};

class Block : public Stmt {
public:
  std::vector<std::unique_ptr<Stmt>> statements;
  Block(std::vector<std::unique_ptr<Stmt>> &statements) : statements{ std::move(statements) } {};

  std::any accept(StmtVisitor &visitor) override {
    return visitor.visitBlockStmt(*this);
  }
};

class Var : public Stmt {
public:
  Token name;
  std::unique_ptr<Expr> initializer;
  Var(Token &name, std::unique_ptr<Expr> &initializer) : name { name }, initializer { std::move(initializer) } {};

  std::any accept(StmtVisitor &visitor) override {
    return visitor.visitVarStmt(*this);
  }
};

class Expression : public Stmt {
public:
  std::unique_ptr<Expr> expr;
  Expression(std::unique_ptr<Expr> &expr) : expr { std::move(expr) } {};

  std::any accept(StmtVisitor &visitor) override {
    return visitor.visitExpressionStmt(*this);
  }
};

class Print : public Stmt {
public:
  std::unique_ptr<Expr> expr;
  Print(std::unique_ptr<Expr> &expr) : expr { std::move(expr) } {};

  std::any accept(StmtVisitor &visitor) override {
    return visitor.visitPrintStmt(*this);
  }
};
