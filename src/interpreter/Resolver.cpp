#include "Expr.hpp"
#include "Token.hpp"
#include "Stmt.hpp"
#include "error.hpp"
#include <memory>
#include <any>
#include <unordered_map>
#include "Resolver.hpp"

std::any Resolver::visitCallExpr(Call& expr) {
  resolve(expr.callee);
  for (auto arg : expr.arguments)
    resolve(arg);
  return nullptr;
}

std::any Resolver::visitLiteralExpr(Literal&) {
  return nullptr;
}

std::any Resolver::visitGroupingExpr(Grouping& expr) {
  resolve(expr.expr);
  return nullptr;
}

std::any Resolver::visitLogicalExpr(Logical& expr) {
  resolve(expr.left);
  resolve(expr.right);
  return nullptr;
}

std::any Resolver::visitUnaryExpr(Unary& expr) {
  resolve(expr.right);
  return nullptr;
}

std::any Resolver::visitAssignExpr(Assign& expr) {
  resolve(expr.value);
  resolveLocal(expr, expr.name);

  return nullptr;
}
std::any Resolver::visitBinaryExpr(Binary& expr) {
  resolve(expr.left);
  resolve(expr.right);
  return nullptr;
}

std::any Resolver::visitVariableExpr(Variable& expr) {
  if (!scopes.empty() && scopes.back()[expr.name.lexeme] == false)
    error(expr.name, "Can't read local variable in its own initializer.");
  resolveLocal(expr, expr.name);

  return nullptr;
}

std::any Resolver::visitFunctionStmt(Function& stmt) {
  declare(stmt.name);
  define(stmt.name);
  resolveFunction(stmt, FunctionType::FUNCTION);
  return nullptr;
}

std::any Resolver::visitBlockStmt(Block& stmt) {
  beginScope();
  resolve(stmt.statements);
  endScope();
  return nullptr;
}

std::any Resolver::visitVarStmt(Var& stmt) {
  declare(stmt.name);
  if (stmt.initializer != nullptr)
    resolve(stmt.initializer);
  define(stmt.name);
  return nullptr;
}

std::any Resolver::visitIfStmt(If& stmt) {
  resolve(stmt.condition);
  resolve(stmt.thenBranch);
  if (stmt.elseBranch != nullptr) resolve(stmt.elseBranch);
  return nullptr;
}

std::any Resolver::visitExpressionStmt(Expression& stmt) {
  resolve(stmt.expr);
  return nullptr;
}

std::any Resolver::visitPrintStmt(Print& stmt) {
  resolve(stmt.expr);
  return nullptr;
}

std::any Resolver::visitReturnStmt(Return& stmt) {
  if (currentFunction == FunctionType::NONE)
    error(stmt.keyword, "Cannot return from top-level code.");
  if (stmt.value != nullptr)
    resolve(stmt.value);
  return nullptr;
}

std::any Resolver::visitWhileStmt(While& stmt) {
  resolve(stmt.condition);
  resolve(stmt.body);
  return nullptr;
}

void Resolver::resolve(std::vector<std::shared_ptr<Stmt>> statements) {
  for (auto statement : statements) {
    resolve(statement);
  }
}

void Resolver::resolve(std::shared_ptr<Stmt> stmt) {
  stmt->accept(*this);
}

void Resolver::resolve(std::shared_ptr<Expr> expr) {
  expr->accept(*this);
}

void Resolver::beginScope() {
  scopes.push_back(std::unordered_map<std::string, bool>());
}

void Resolver::endScope() {
  scopes.pop_back();
}

void Resolver::declare(Token name) {
  if (scopes.empty())
      return;
  auto& scope = scopes.back();
  if (scope.find(name.lexeme) != scope.end())
    error(name, "Already a variable with this name in scope.");
  scopes.back()[name.lexeme] = false;
}
void Resolver::define(Token name) {
  if (scopes.empty())
      return;
  scopes.back()[name.lexeme] = true;
}
void Resolver::resolveLocal(Expr& expr, Token name) {
  for (long i = std::ssize(scopes) - 1; i >= 0; i--) {
    if (scopes.at(static_cast<size_t>(i)).find(name.lexeme) != scopes[static_cast<size_t>(i)].end()) {
      interpreter->resolve(expr, static_cast<size_t>(std::ssize(scopes) - 1 - i));
      return;
    }
  }
}

void Resolver::resolveFunction(Function& function, FunctionType type) {
  FunctionType enclosingFunction{ currentFunction };
  currentFunction = type;

  beginScope();
  for (Token param : function.params) {
    declare(param);
    define(param);
  }
  resolve(function.body);
  endScope();
  currentFunction = enclosingFunction;
}
