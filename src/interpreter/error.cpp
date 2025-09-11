extern bool hadError;
extern bool hadRuntimeError;
#include <iostream>
#include "Token.hpp"
#include "RuntimeError.hpp"

void report(int line, std::string where, std::string message) {
  std::cout << "[line " << line << "] Error" << where << ": " << message
            << std::endl;
  hadError = true;
}

void error(int line, std::string message) { report(line, "", message); }
void error(Token token, std::string message) { 
  if (token.type == TokenType::END_OF_LINE)
    report(token.line, " at end", message);
  else
    report(token.line, " at '" + token.lexeme + "'", message);
}

void runtimeError(RuntimeError error) {
  std::cout << error.what() << "\n[line " << error.token.line << "]" << std::endl; 
  hadRuntimeError = true;
}
