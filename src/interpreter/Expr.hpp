#pragma once

#include <any>
#include "Token.hpp"

struct ExprVisitor;

struct Expr {
  virtual std::any accept(ExprVisitor &visitor) = 0;
  virtual ~Expr() = default;
};

struct Assign;
struct Grouping;
struct Binary;
struct Literal;
struct Logical;
struct Logic;
struct Unary;
struct Variable;

struct ExprVisitor {
  virtual std::any visitAssignExpr(Assign &expr) = 0;
  virtual std::any visitGroupingExpr(Grouping &expr) = 0;
  virtual std::any visitBinaryExpr(Binary &expr) = 0;
  virtual std::any visitLiteralExpr(Literal &expr) = 0;
  virtual std::any visitLogicalExpr(Logical &expr) = 0;
  virtual std::any visitUnaryExpr(Unary &expr) = 0;
  virtual std::any visitVariableExpr(Variable &expr) = 0;
};

struct Assign : public Expr {
  Token name;
  std::unique_ptr<Expr> value;
  Assign(Token &name, std::unique_ptr<Expr> &value) : name { name }, value { std::move(value) } {};

  std::any accept(ExprVisitor &visitor) override {
    return visitor.visitAssignExpr(*this);
  }
};

struct Grouping : public Expr {
  std::unique_ptr<Expr> expr;
  Grouping(std::unique_ptr<Expr> &expr) : expr { std::move(expr) } {};

  std::any accept(ExprVisitor &visitor) override {
    return visitor.visitGroupingExpr(*this);
  }
};

struct Binary : public Expr {
  std::unique_ptr<Expr> left;
  Token op;
  std::unique_ptr<Expr> right;
  Binary(std::unique_ptr<Expr> &left, Token &op, std::unique_ptr<Expr> &right) : left { std::move(left) }, op { op }, right { std::move(right) } {};

  std::any accept(ExprVisitor &visitor) override {
    return visitor.visitBinaryExpr(*this);
  }
};

struct Literal : public Expr {
  std::any value;
  Literal(std::any value) : value { value } {};

  std::any accept(ExprVisitor &visitor) override {
    return visitor.visitLiteralExpr(*this);
  }
};

struct Logical : public Expr {
  std::unique_ptr<Expr> right;
  std::unique_ptr<Expr> left;
  Token op;
  Logical(std::unique_ptr<Expr> &left, Token op, std::unique_ptr<Expr> &right) : right { std::move(right)}, op { op }, left { std::move(left) } {};

  std::any accept(ExprVisitor &visitor) override {
    return visitor.visitLogicalExpr(*this);
  }
};


struct Unary : public Expr {
  Token op;
  std::unique_ptr<Expr> right;
  Unary(Token &op, std::unique_ptr<Expr> &right) : op { op }, right { std::move(right) } {};

  std::any accept(ExprVisitor &visitor) override {
    return visitor.visitUnaryExpr(*this);
  }
};

struct Variable : public Expr {
  Token name;
  Variable(Token name) : name { name } {};

  std::any accept(ExprVisitor &visitor) override {
    return visitor.visitVariableExpr(*this);
  }
};

