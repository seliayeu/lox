#pragma once
#include "Token.hpp"
#include "error.hpp"
#include <string>
#include <vector>
#include <unordered_map>

class Scanner {
  const std::string source;
  std::vector<Token> tokens;
  int start { 0 };
  int current { 0 };
  int line { 1 };
  std::unordered_map<std::string, TokenType> keywords = {
    {"and", TokenType::AND},
    {"class", TokenType::CLASS},
    {"else", TokenType::ELSE},
    {"false", TokenType::FALSE},
    {"for", TokenType::FOR},
    {"fun", TokenType::FUN},
    {"if", TokenType::IF},
    {"nil", TokenType::NIL},
    {"or", TokenType::OR},
    {"print", TokenType::PRINT},
    {"return", TokenType::RETURN},
    {"super", TokenType::SUPER},
    {"this", TokenType::THIS},
    {"true", TokenType::TRUE},
    {"var", TokenType::VAR},
    {"while", TokenType::WHILE},
  };
  

  bool isAtEnd();
  void scanToken();
  void identifier();
  bool isAlpha(char c);
  bool isAlphaNumeric(char c);
  char advance();
  bool match(char expected);
  char peek();
  char peekNext();
  void string();
  bool isDigit(char c);
  void number();
  void addToken(TokenType type, std::any literal = nullptr);

public:
  Scanner(std::string source) : source(source) {}
  std::vector<Token> scanTokens();
  int getLine();
};
