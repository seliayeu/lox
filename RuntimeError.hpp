#ifndef RUNTIMEERROR
#define RUNTIMEERROR
#include <stdexcept>
#include "Token.hpp"

struct RuntimeError : public std::runtime_error {
  Token token;
  RuntimeError(Token token, std::string message) : std::runtime_error(message), token(token) {}
};
#endif
