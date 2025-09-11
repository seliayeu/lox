#include "Token.hpp"
#include "error.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include "Scanner.hpp"

bool Scanner::isAtEnd() { return current >= source.length(); }
void Scanner::scanToken() {
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

void Scanner::identifier() {
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

bool Scanner::isAlpha(char c) {
  return (c >= 'a' && c <= 'z') ||
    (c >= 'A' && c <= 'Z') ||
    (c == '_');
}

bool Scanner::isAlphaNumeric(char c) {
  return isAlpha(c) || isDigit(c);
}

char Scanner::advance() { return source[current++]; }

bool Scanner::match(char expected) {
  if (isAtEnd())
    return false;
  if (source[current] != expected)
    return false;
  current++;
  return true;
}

char Scanner::peek() {
  if (isAtEnd())
    return '\0';
  return source[current];
}

char Scanner::peekNext() {
  if (current + 1 >= source.length())
    return '\0';
  return source[current + 1];
}



void Scanner::string() {
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

bool Scanner::isDigit(char c) {
  return c >= '0' && c <= '9';
}

void Scanner::number() {
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

void Scanner::addToken(TokenType type, std::any literal) {
  std::string text = source.substr(start, current - start);
  tokens.push_back(Token(type, text, literal, line));
}

std::vector<Token> Scanner::scanTokens() {
  while (!isAtEnd() && !hadError) {
    start = current;
    scanToken();
  }
  tokens.push_back(Token(TokenType::END_OF_LINE, "", nullptr, line));
  return tokens;
}

int Scanner::getLine() { return line; }
