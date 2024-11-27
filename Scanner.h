#include "Token.h"
#include <string>
#include <vector>

class Scanner {
  const std::string source;
  std::vector<Token> tokens;
  int start = 0;
  int current = 0;
  int line = 1;

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
    }
  }

  char advance() { return source[current++]; }

  void addToken(TokenType type, std::any literal = nullptr) {
    std::string text = source.substr(start, current - start);
    tokens.push_back(Token(type, text, literal, line));
  }

public:
  Scanner(std::string source) : source(source) {}
  std::vector<Token> scanTokens() {
    while (!isAtEnd()) {
      start = current;
      scanToken();
    }
    tokens.push_back(Token(TokenType::END_OF_LINE, "", nullptr, line));
    return tokens;
  }
};
