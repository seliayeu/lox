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
class While;
class Expression;
class Function;
class If;
class Print;
class Return;

class StmtVisitor {
public:
  virtual std::any visitBlockStmt(Block &stmt) = 0;
  virtual std::any visitVarStmt(Var &stmt) = 0;
  virtual std::any visitWhileStmt(While &stmt) = 0;
  virtual std::any visitExpressionStmt(Expression &stmt) = 0;
  virtual std::any visitFunctionStmt(Function &stmt) = 0;
  virtual std::any visitIfStmt(If &stmt) = 0;
  virtual std::any visitPrintStmt(Print &stmt) = 0;
  virtual std::any visitReturnStmt(Return &stmt) = 0;
};

class Block : public Stmt {
public:
  std::vector<std::shared_ptr<Stmt>> statements;
  Block(std::vector<std::shared_ptr<Stmt>> &statements) : statements{ std::move(statements) } {};

  std::any accept(StmtVisitor &visitor) override {
    return visitor.visitBlockStmt(*this);
  }
};

class Var : public Stmt {
public:
  Token name;
  std::shared_ptr<Expr> initializer;
  Var(Token &name, std::shared_ptr<Expr> &initializer) : name { name }, initializer { std::move(initializer) } {};

  std::any accept(StmtVisitor &visitor) override {
    return visitor.visitVarStmt(*this);
  }
};

class While : public Stmt {
public:
  std::shared_ptr<Expr> condition;
  std::shared_ptr<Stmt> body;
  While(std::shared_ptr<Expr> &condition, std::shared_ptr<Stmt> &body) : condition { std::move(condition) }, body { std::move(body) } {};

  std::any accept(StmtVisitor &visitor) override {
    return visitor.visitWhileStmt(*this);
  }
};

class Expression : public Stmt {
public:
  std::shared_ptr<Expr> expr;
  Expression(std::shared_ptr<Expr> &expr) : expr { std::move(expr) } {};

  std::any accept(StmtVisitor &visitor) override {
    return visitor.visitExpressionStmt(*this);
  }
};

class Function : public Stmt {
public:
  Token name;
  std::vector<Token> params;
  std::vector<std::shared_ptr<Stmt>> body;
  Function(Token name, std::vector<Token> &params, std::vector<std::shared_ptr<Stmt>> &body) : name { name }, params { params }, body { std::move(body) } {};

  std::any accept(StmtVisitor &visitor) override {
    return visitor.visitFunctionStmt(*this);
  }
};

class If : public Stmt {
public:
  std::shared_ptr<Expr> condition;
  std::shared_ptr<Stmt> thenBranch;
  std::shared_ptr<Stmt> elseBranch;
  If(std::shared_ptr<Expr> &condition, std::shared_ptr<Stmt> &thenBranch, std::shared_ptr<Stmt> &elseBranch) : condition { condition }, thenBranch { thenBranch }, elseBranch { elseBranch} {};

  std::any accept(StmtVisitor &visitor) override {
    return visitor.visitIfStmt(*this);
  }
};

class Print : public Stmt {
public:
  std::shared_ptr<Expr> expr;
  Print(std::shared_ptr<Expr> &expr) : expr { std::move(expr) } {};

  std::any accept(StmtVisitor &visitor) override {
    return visitor.visitPrintStmt(*this);
  }
};

class Return : public Stmt {
public:
  Token keyword;
  std::shared_ptr<Expr> value;
  Return(Token keyword, std::shared_ptr<Expr> &value) : keyword{ keyword }, value{ value } {};

  std::any accept(StmtVisitor &visitor) override {
    return visitor.visitReturnStmt(*this);
  }
};
