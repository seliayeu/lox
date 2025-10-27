#ifndef ERROR
#define ERROR
#include <string>
#include "Token.hpp"
#include "RuntimeError.hpp"

extern bool hadError;
extern bool hadRuntimeError;

void report(size_t line, std::string where, std::string message);
void error(size_t line, std::string message);
void error(Token token, std::string message);
void runtimeError(RuntimeError error);

#endif
