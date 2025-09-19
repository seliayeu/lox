#pragma once
#include <stdexcept>
#include <any>

class ReturnException : public std::runtime_error {
public:
  std::any value;
  ReturnException(std::any value) : std::runtime_error("return"), value{ value } {}
};
