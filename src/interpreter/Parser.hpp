#include "Stmt.hpp"
#include "Expr.hpp"
#include "Token.hpp"
#include "TokenType.hpp"
#include <vector>
#include "error.hpp"
#include <stdexcept>
#include <sstream>

struct Parser {
  std::vector<Token> tokens;
  int current { 0 };

  struct ParseError : std::runtime_error {
    ParseError() : std::runtime_error("") {}
  };

  Parser(std::vector<Token> &tokens) : tokens { tokens } {}
  std::shared_ptr<Expr> expression() {
    return assignment();
  }
  std::shared_ptr<Expr> assignment() {
    std::shared_ptr<Expr> expr = orExpr();
    if (match(TokenType::EQUAL)) {
      Token equals = previous();
      std::shared_ptr<Expr> value = assignment();

      if (Variable* varPtr = dynamic_cast<Variable*>(expr.get())) {
        Token name = varPtr->name;
        return std::make_shared<Assign>(name, value);
      }
      error(equals, "Invalid assignment target.");
    }
    return expr;
  }
  std::shared_ptr<Expr> orExpr() {
    std::shared_ptr<Expr> expr = andExpr();

    while (match(TokenType::OR)) {
      Token op = previous();
      std::shared_ptr<Expr> right = andExpr();
      expr = std::make_shared<Logical>(expr, op, right);
    }
    return expr;
  }

  std::shared_ptr<Expr> andExpr() {
    std::shared_ptr<Expr> expr = equality();

    while (match(TokenType::OR)) {
      Token op = previous();
      std::shared_ptr<Expr> right = equality();
      expr = std::make_shared<Logical>(expr, op, right);
    }
    return expr;
  }

  std::shared_ptr<Expr> equality() {
    if (match(TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL)) {
      comparison(); // discard
      error(peek(), "Expect finished expression.");
      throw ParseError();
    }
    std::shared_ptr<Expr> expr = comparison();
    while (match(TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL)) {
      Token op = previous();
      std::shared_ptr<Expr> right = comparison();
      expr = std::make_shared<Binary>(expr, op, right);
    }
    return expr;
  }
  std::shared_ptr<Expr> comparison() {
    if (match(TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL)) {
      term(); // discard
      error(peek(), "Expect finished expression.");
      throw ParseError();
    }
    std::shared_ptr<Expr> expr = term();
    while (match(TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL)) {
      Token op = previous();
      std::shared_ptr<Expr> right = term();
      expr = std::make_shared<Binary>(expr, op, right);
    }
    return expr;
  }

  std::shared_ptr<Expr> term() {
    if (match(TokenType::PLUS)) {
      factor(); // discard
      error(peek(), "Expect finished expression.");
      throw ParseError();
    }
    std::shared_ptr<Expr> expr = factor();
    while (match(TokenType::MINUS, TokenType::PLUS)) {
      Token op = previous();
      std::shared_ptr<Expr> right = factor();
      expr = std::make_shared<Binary>(expr, op, right);
    }
    return expr;
  }
  
  std::shared_ptr<Expr> factor() {
    if (match(TokenType::SLASH, TokenType::STAR)) {
      unary(); // discard
      error(peek(), "Expect finished expression.");
      throw ParseError();
    }
    std::shared_ptr<Expr> expr = unary();
    while (match(TokenType::SLASH, TokenType::STAR)) {
      Token op = previous();
      std::shared_ptr<Expr> right = unary();
      expr = std::make_shared<Binary>(expr, op, right);
    }
    return expr;
  }
  std::shared_ptr<Expr> unary() {
    if (match(TokenType::BANG, TokenType::MINUS)) {
      Token op = previous();
      std::shared_ptr<Expr> right = unary();
      return std::make_shared<Unary>(op, right);
    }
    return call();
  }

  std::shared_ptr<Expr>call() {
    std::shared_ptr<Expr> expr = primary();
    while (true) {
      if (match(TokenType::LEFT_PAREN))
        expr = finishCall(expr);
      else
        break;
    }
    return expr;
  }
  
  std::shared_ptr<Expr>finishCall(std::shared_ptr<Expr> &callee) {
    std::vector<std::shared_ptr<Expr>> args;
    if (!check(TokenType::RIGHT_PAREN)) {
      args.push_back(expression());
      while (match(TokenType::COMMA)) {
        if (args.size() >= 255)
          error(peek(), "Can't have more than 255 arguments.");
        args.push_back(expression());
      }
    }
    Token paren = consume(TokenType::RIGHT_PAREN, "Expect ')' after arguments.");
    return std::make_shared<Call>(callee, paren, args);
  }

  

  std::shared_ptr<Expr> primary() {
    if (match(TokenType::FALSE))
      return std::make_shared<Literal>(false);
    if (match(TokenType::TRUE))
      return std::make_shared<Literal>(true);
    if (match(TokenType::NIL))
      return std::make_shared<Literal>((void*) nullptr);
    if (match(TokenType::NUMBER, TokenType::STRING))
      return std::make_shared<Literal>(previous().literal);
    if (match(TokenType::IDENTIFIER)) {
      return std::make_shared<Variable>(previous());
    }
    if (match(TokenType::LEFT_PAREN)) {
      std::shared_ptr<Expr> expr = expression();
      consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
      return std::make_shared<Grouping>(expr);
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
  std::vector<std::shared_ptr<Stmt>> parse() {
    std::vector<std::shared_ptr<Stmt>> statements { };
    while (!isAtEnd())
      statements.push_back(declaration());
    return statements;
  }
  std::shared_ptr<Stmt> statement() {
    if (match(TokenType::IF)) 
      return ifStatement();
    if (match(TokenType::PRINT)) 
      return printStatement();
    if (match(TokenType::RETURN))
      return returnStatement();
    if (match(TokenType::WHILE))
      return whileStatement();
    if (match(TokenType::FOR))
      return forStatement();
    if (match(TokenType::LEFT_BRACE)) {
      std::vector<std::shared_ptr<Stmt>> stmt = block();
      return std::make_shared<Block>(stmt);
    }
    return expressionStatement();
  }
  std::shared_ptr<Stmt> printStatement() {
    std::shared_ptr<Expr> value = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after value.");
    return std::make_shared<Print>(value);
  }
  std::shared_ptr<Stmt> returnStatement() {
    Token keyword = previous();
    std::shared_ptr<Expr> value{ nullptr };
    if (!check(TokenType::SEMICOLON)) {
      value = expression();
    }
    consume(TokenType::SEMICOLON, "Expect ';' after return value.");
    return std::make_shared<Return>(keyword, value);
  }
  std::shared_ptr<Stmt> whileStatement() {
    consume(TokenType::LEFT_PAREN, "Expect '(' after if.");
    std::shared_ptr<Expr> condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after condition.");
    std::shared_ptr<Stmt> body { statement() };
    return std::make_shared<While>(condition, body);
  }
  std::shared_ptr<Stmt> forStatement() {
    consume(TokenType::LEFT_PAREN, "Expect '(' after if.");
    std::shared_ptr<Stmt> initializer;
    if (match(TokenType::SEMICOLON))
      initializer = nullptr;
    else if (match(TokenType::VAR))
      initializer = varDeclaration();
    else
      initializer = expressionStatement();

    std::shared_ptr<Expr> condition = nullptr;
    if (!check(TokenType::SEMICOLON))
      condition = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after loop condition.");

    std::shared_ptr<Expr> increment = nullptr;
    if (!check(TokenType::SEMICOLON))
      increment = expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after loop condition.");

    std::shared_ptr<Stmt> body = statement();
    if (increment != nullptr) {
      std::vector<std::shared_ptr<Stmt>> blockStatements;
      blockStatements.push_back(std::move(body));
      blockStatements.push_back(std::make_shared<Expression>(increment));
      body = std::make_shared<Block>(blockStatements);
    }

    if (condition == nullptr)
      condition = std::make_shared<Literal>(true);
    
    body = std::make_shared<While>(condition, body);

    if (initializer != nullptr) {
      std::vector<std::shared_ptr<Stmt>> blockStatements;
      blockStatements.push_back(std::move(initializer));
      blockStatements.push_back(std::move(body));
      body = std::make_shared<Block>(blockStatements);
    }

    return body;
  }
  std::shared_ptr<Stmt> ifStatement() {
    consume(TokenType::LEFT_PAREN, "Expect '(' after if.");
    std::shared_ptr<Expr> condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after condition.");
    std::shared_ptr<Stmt> thenBranch { statement() };
    std::shared_ptr<Stmt> elseBranch { nullptr };
    if (match(TokenType::ELSE))
        elseBranch = statement();
    return std::make_shared<If>(condition, thenBranch, elseBranch);
  }
  std::shared_ptr<Stmt> expressionStatement() {
    std::shared_ptr<Expr> value = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after value.");
    return std::make_shared<Expression>(value);
  }
  std::shared_ptr<Stmt> declaration() {
    try {
      if (match(TokenType::FUN)) 
        return function("function");
      if (match(TokenType::VAR)) 
        return varDeclaration();
      return statement();
    } catch (ParseError error) {
      synchronize();
      return nullptr;
    }
  }

  std::shared_ptr<Stmt> function(std::string_view kind) {
    std::ostringstream oss{};
    oss << "Expect " << kind << " name.";
    Token name = consume(TokenType::IDENTIFIER, oss.str());

    oss.str("");
    oss << "Expect '(' after " << kind << " name.";
    consume(TokenType::LEFT_PAREN, oss.str());

    std::vector<Token> parameters{};
    if (!check(TokenType::RIGHT_PAREN)) {
      parameters.push_back(consume(TokenType::IDENTIFIER, "Expect parameter name."));
      while (match(TokenType::COMMA)) {
        if (parameters.size() >= 255)
          error(peek(), "Can't have more than 255 parameters.");
        parameters.push_back(consume(TokenType::IDENTIFIER, "Expect parameter name."));
      }
    }
    consume(TokenType::RIGHT_PAREN, "Expect ')' after parameters.");

    oss.str("");
    oss << "Expect '{' before " << kind << " body.";
    consume(TokenType::LEFT_BRACE, oss.str());

    std::vector<std::shared_ptr<Stmt>> body = block();
    return std::make_shared<Function>(name, parameters, body);
  }

  std::shared_ptr<Stmt> varDeclaration() {
    Token name = consume(TokenType::IDENTIFIER, "Expect variable name.");
    std::shared_ptr<Expr> initializer = nullptr;
    if (match(TokenType::EQUAL))
      initializer = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after variable declaration.");
    return std::make_shared<Var>(name, initializer);
  }

  std::vector<std::shared_ptr<Stmt>> block() {
    std::vector<std::shared_ptr<Stmt>> statements;
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd())
      statements.push_back(declaration());
    consume(TokenType::RIGHT_BRACE, "Expect '}' after block");
    return statements;
  }
};
