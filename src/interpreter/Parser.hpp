#include "Stmt.hpp"
#include "Expr.hpp"
#include "Token.hpp"
#include "TokenType.hpp"
#include <vector>
#include "error.hpp"
#include <stdexcept>

struct Parser {
  std::vector<Token> tokens;
  int current { 0 };

  struct ParseError : std::runtime_error {
    ParseError() : std::runtime_error("") {}
  };

  Parser(std::vector<Token> &tokens) : tokens { tokens } {}
  std::unique_ptr<Expr> expression() {
    return assignment();
  }
  std::unique_ptr<Expr> assignment() {
    std::unique_ptr<Expr> expr = term();
    if (match(TokenType::EQUAL)) {
      Token equals = previous();
      std::unique_ptr<Expr> value = assignment();

      if (Variable* varPtr = dynamic_cast<Variable*>(value.get())) {
        Token name = varPtr->name;
        return std::make_unique<Assign>(name, value);
      }
      error(equals, "Invalid assignment target.");
    }
    return expr;
  }
  std::unique_ptr<Expr> equality() {
    if (match(TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL)) {
      comparison(); // discard
      error(peek(), "Expect finished expression.");
      throw ParseError();
    }
    std::unique_ptr<Expr> expr = comparison();
    while (match(TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL)) {
      Token op = previous();
      std::unique_ptr<Expr> right = comparison();
      expr = std::make_unique<Binary>(expr, op, right);
    }
    return expr;
  }
  std::unique_ptr<Expr> comparison() {
    if (match(TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL)) {
      term(); // discard
      error(peek(), "Expect finished expression.");
      throw ParseError();
    }
    std::unique_ptr<Expr> expr = term();
    while (match(TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL)) {
      Token op = previous();
      std::unique_ptr<Expr> right = term();
      expr = std::make_unique<Binary>(expr, op, right);
    }
    return expr;
  }

  std::unique_ptr<Expr> term() {
    if (match(TokenType::PLUS)) {
      factor(); // discard
      error(peek(), "Expect finished expression.");
      throw ParseError();
    }
    std::unique_ptr<Expr> expr = factor();
    while (match(TokenType::MINUS, TokenType::PLUS)) {
      Token op = previous();
      std::unique_ptr<Expr> right = factor();
      expr = std::make_unique<Binary>(expr, op, right);
    }
    return expr;
  }
  
  std::unique_ptr<Expr> factor() {
    if (match(TokenType::SLASH, TokenType::STAR)) {
      unary(); // discard
      error(peek(), "Expect finished expression.");
      throw ParseError();
    }
    std::unique_ptr<Expr> expr = unary();
    while (match(TokenType::SLASH, TokenType::STAR)) {
      Token op = previous();
      std::unique_ptr<Expr> right = unary();
      expr = std::make_unique<Binary>(expr, op, right);
    }
    return expr;
  }
  std::unique_ptr<Expr> unary() {
    if (match(TokenType::BANG, TokenType::MINUS)) {
      Token op = previous();
      std::unique_ptr<Expr> right = unary();
      return std::make_unique<Unary>(op, right);
    }
    return primary();
  }

  std::unique_ptr<Expr> primary() {
    if (match(TokenType::FALSE))
      return std::make_unique<Literal>(false);
    if (match(TokenType::TRUE))
      return std::make_unique<Literal>(true);
    if (match(TokenType::NIL))
      return std::make_unique<Literal>((void*) nullptr);
    if (match(TokenType::NUMBER, TokenType::STRING))
      return std::make_unique<Literal>(previous().literal);
    if (match(TokenType::IDENTIFIER))
      return std::make_unique<Variable>(previous());
    if (match(TokenType::LEFT_PAREN)) {
      std::unique_ptr<Expr> expr = expression();
      consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
      return std::make_unique<Grouping>(expr);
    }
    error(peek(), "Expect expression.");
    throw ParseError();
  }

  Token consume(TokenType type, std::string message) {
    if (check(type)) 
      return advance();
    error(peek(), message);
    throw ParseError();
  }
 
  template<typename... Args>
  bool match(Args... types) {
    for (TokenType type : { types... }) {
      if (check(type)) {
        advance();
        return true;
      }
    }
    return false;
  }
  bool check(TokenType type) {
    if (isAtEnd()) return false;
    return peek().type == type;
  }
  Token advance() {
    if (!isAtEnd())
      current++;
    return previous();
  }
  bool isAtEnd() {
    return peek().type == TokenType::END_OF_LINE;
  }
  Token peek() {
    return tokens[current];
  }
  Token previous() {
    return tokens[current - 1];
  }
  void synchronize() {
    advance();
    while (!isAtEnd()) {
      if (previous().type == TokenType::SEMICOLON) return;
      switch (peek().type) {
        case TokenType::CLASS:
        case TokenType::FUN:
        case TokenType::VAR:
        case TokenType::FOR:
        case TokenType::IF:
        case TokenType::WHILE:
        case TokenType::PRINT:
        case TokenType::RETURN:
          return;
        default:
          break;
      }
      advance();
    }
  }
  std::vector<std::unique_ptr<Stmt>> parse() {
    std::vector<std::unique_ptr<Stmt>> statements { };
    while (!isAtEnd())
      statements.push_back(declaration());
    return statements;
  }
  std::unique_ptr<Stmt> statement() {
    if (match(TokenType::PRINT)) 
      return printStatement();
    if (match(TokenType::LEFT_BRACE)) {
      std::vector<std::unique_ptr<Stmt>> stmt = block();
      return std::make_unique<Block>(stmt);
    }
    return expressionStatement();
  }
  std::unique_ptr<Stmt> printStatement() {
    std::unique_ptr<Expr> value = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after value.");
    return std::make_unique<Print>(value);
  }
  std::unique_ptr<Stmt> expressionStatement() {
    std::unique_ptr<Expr> value = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after value.");
    return std::make_unique<Expression>(value);
  }
  std::unique_ptr<Stmt> declaration() {
    try {
      if (match(TokenType::VAR)) 
        return varDeclaration();
      return statement();
    } catch (ParseError error) {
      synchronize();
      return nullptr;
    }
  }
  std::unique_ptr<Stmt> varDeclaration() {
    Token name = consume(TokenType::IDENTIFIER, "Expect variable name.");
    std::unique_ptr<Expr> initializer = nullptr;
    if (match(TokenType::EQUAL))
      initializer = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after variable declaration.");
    return std::make_unique<Var>(name, initializer);
  }

  std::vector<std::unique_ptr<Stmt>> block() {
    std::vector<std::unique_ptr<Stmt>> statements;
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd())
      statements.push_back(declaration());
    consume(TokenType::RIGHT_BRACE, "Expect '}' after block");
    return statements;
  }
};
