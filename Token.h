#include <any>
#include <iostream>
#include <string>
#include <strstream>

enum TokenType {
  // Single-character tokens.
  LEFT_PAREN,
  RIGHT_PAREN,
  LEFT_BRACE,
  RIGHT_BRACE,
  COMMA,
  DOT,
  MINUS,
  PLUS,
  SEMICOLON,
  SLASH,
  STAR,

  // One or two character tokens.
  BANG,
  BANG_EQUAL,
  EQUAL,
  EQUAL_EQUAL,
  GREATER,
  GREATER_EQUAL,
  LESS,
  LESS_EQUAL,

  // Literals.
  IDENTIFIER,
  STRING,
  NUMBER,

  // Keywords.
  AND,
  CLASS,
  ELSE,
  FALSE,
  FUN,
  FOR,
  IF,
  NIL,
  OR,
  PRINT,
  RETURN,
  SUPER,
  THIS,
  TRUE,
  VAR,
  WHILE,

  END_OF_LINE,
};

class Token {

public:
  const TokenType type;
  const std::string lexeme;
  const std::any literal;
  const int line;

  Token(TokenType type, std::string lexeme, std::any literal, int line)
      : type(type), lexeme(lexeme), literal(literal), line(line){};

  std::string literalAsString() const {
    if (literal.type() == typeid(int)) {
      return std::to_string(std::any_cast<int>(literal));
    } else if (literal.type() == typeid(float)) {
      return std::to_string(std::any_cast<float>(literal));
    } else if (literal.type() == typeid(std::string)) {
      return std::any_cast<std::string>(literal);
    }
    return "?";
  }

  friend std::ostream &operator<<(std::ostream &os, const Token &t) {
    return std::cout << t.type << " " << t.lexeme << " " << t.literalAsString();
  }
};
