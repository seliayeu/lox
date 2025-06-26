#pragma once

#include <vector>
#include "Token.hpp"
#include "Expr.hpp"

struct StmtVisitor;

struct Stmt {
  virtual void accept(StmtVisitor &visitor) = 0;
  virtual ~Stmt() = default;
};

struct Block;
struct Var;
struct Expression;
struct Print;

struct StmtVisitor {
  virtual void visitBlockStmt(Block &stmt) = 0;
  virtual void visitVarStmt(Var &stmt) = 0;
  virtual void visitExpressionStmt(Expression &stmt) = 0;
  virtual void visitPrintStmt(Print &stmt) = 0;
};

struct Block : public Stmt {
  std::vector<std::unique_ptr<Stmt>> statements;
  Block(std::vector<std::unique_ptr<Stmt>> statements) : statements{ std::move(statements) } {};

  void accept(StmtVisitor &visitor) override {
    return visitor.visitBlockStmt(*this);
  }
};

struct Var : public Stmt {
  Token name;
  std::unique_ptr<Expr> initializer;
  Var(Token &name, std::unique_ptr<Expr> &initializer) : name { name }, initializer { std::move(initializer) } {};

  void accept(StmtVisitor &visitor) override {
    return visitor.visitVarStmt(*this);
  }
};

struct Expression : public Stmt {
  std::unique_ptr<Expr> expr;
  Expression(std::unique_ptr<Expr> &expr) : expr { std::move(expr) } {};

  void accept(StmtVisitor &visitor) override {
    return visitor.visitExpressionStmt(*this);
  }
};

struct Print : public Stmt {
  std::unique_ptr<Expr> expr;
  Print(std::unique_ptr<Expr> &expr) : expr { std::move(expr) } {};

  void accept(StmtVisitor &visitor) override {
    return visitor.visitPrintStmt(*this);
  }
};

