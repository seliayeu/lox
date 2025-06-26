#pragma once
#include "Token.hpp"
#include "error.hpp"
#include <string>
#include <vector>
#include <unordered_map>

class Scanner {
  const std::string source;
  std::vector<Token> tokens;
  int start = 0;
  int current = 0;
  int line = 1;
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
  

  bool isAtEnd() { return current >= source.length(); }
  void scanToken() {
    char c = advance();
    switch (c) {
    case '(':
      addToken(TokenType::LEFT_PAREN);
      break;
    case ')':
      addToken(TokenType::RIGHT_PAREN);
      break;
    case '{':
      addToken(TokenType::LEFT_BRACE);
      break;
    case '}':
      addToken(TokenType::RIGHT_BRACE);
      break;
    case ',':
      addToken(TokenType::COMMA);
      break;
    case '.':
      addToken(TokenType::DOT);
      break;
    case '-':
      addToken(TokenType::MINUS);
      break;
    case '+':
      addToken(TokenType::PLUS);
      break;
    case ';':
      addToken(TokenType::SEMICOLON);
      break;
    case '*':
      addToken(TokenType::STAR);
      break;
    case ':':
      addToken(TokenType::COLON);
      break;
    case '?':
      addToken(TokenType::QUESTION);
      break;
    case '!':
      addToken(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG);
      break;
    case '=':
      addToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL);
      break;
    case '<':
      addToken(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS);
      break;
    case '>':
      addToken(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER);
      break;
    case '/':
      if (match('/')) {
        while (peek() != '\n' && !isAtEnd())
          advance();
      } else if (match('*')) {
        while (!(peek() == '*' && peekNext() == '/') && !isAtEnd())
          advance();
        if (isAtEnd()) {
          error(line, std::string("Unterminated comment."));
          return;
        }
        advance();
        advance();
      } else {
        addToken(TokenType::SLASH);
      }
      break;
    case '"':
      string();
      break;
    case ' ':
    case '\r':
    case '\t':
      break;
    case '\n':
      line++;
      break;
    default:
      if (isDigit(c))
        number();
      else if (isAlpha(c))
        identifier();
      else
        error(line, std::string("Unexpected character"));
      break;
    }
  }

  void identifier() {
    while (isAlphaNumeric(peek()))
      advance();
    std::string text = source.substr(start, current - start);
    TokenType type;
    if (!keywords.count(text))
      type = TokenType::IDENTIFIER;
    else
      type = keywords[text];

    addToken(type);
  }
  
  bool isAlpha(char c) {
    return (c >= 'a' && c <= 'z') ||
      (c >= 'A' && c <= 'Z') ||
      (c == '_');
  }
  
  bool isAlphaNumeric(char c) {
    return isAlpha(c) || isDigit(c);
  }

  char advance() { return source[current++]; }

  bool match(char expected) {
    if (isAtEnd())
      return false;
    if (source[current] != expected)
      return false;
    current++;
    return true;
  }

  char peek() {
    if (isAtEnd())
      return '\0';
    return source[current];
  }

  char peekNext() {
    if (current + 1 >= source.length())
      return '\0';
    return source[current + 1];
  }



  void string() {
    while (peek() != '"' && !isAtEnd()) {
      if (peek() == '\n')
        line++;
      advance();
    }
    if (isAtEnd()) {
      error(line, std::string("Unterminated string."));
      return;
    }
    advance();

    std::string value = source.substr(start + 1, current - start - 2);
    addToken(TokenType::STRING, value);
  }

  bool isDigit(char c) {
    return c >= '0' && c <= '9';
  }

  void number() {
    while (isDigit(peek()))
      advance();

    if (peek() == '.' && isDigit(peekNext())) {
      advance();
      while (isDigit(peek()))
        advance();
    }
    
    std::string num = source.substr(start, current - start);
    addToken(TokenType::NUMBER, std::stod(num));
  }

  void addToken(TokenType type, std::any literal = nullptr) {
    std::string text = source.substr(start, current - start);
    tokens.push_back(Token(type, text, literal, line));
  }

public:
  Scanner(std::string source) : source(source) {}
  std::vector<Token> scanTokens() {
    while (!isAtEnd() && !hadError) {
      start = current;
      scanToken();
    }
    tokens.push_back(Token(TokenType::END_OF_LINE, "", nullptr, line));
    return tokens;
  }
  int getLine() { return line; }
};
