#include "Expr.hpp"

struct ReversePolishVisitor : public Visitor {
  std::any visitBinaryExpr(Binary &expr) override {
    std::string leftVal = std::any_cast<std::string>(expr.left->accept(*this));
    std::string rightVal = std::any_cast<std::string>(expr.right->accept(*this));
    return std::any_cast<std::string>(leftVal + " " + rightVal + " " + expr.op.lexeme);
  }
  std::any visitUnaryExpr(Unary &expr) override {
    std::string val = std::any_cast<std::string>(expr.right->accept(*this));
    return std::any_cast<std::string>(val + " " + expr.op.lexeme);
  }
  std::any visitLiteralExpr(Literal &expr) override {
    std::any literal = expr.value;
    if (literal.type() == typeid(int)) {
      return std::to_string(std::any_cast<int>(literal));
    } else if (literal.type() == typeid(float)) {
      return std::to_string(std::any_cast<float>(literal));
    }
    std::cerr << "Invalid literal encountered. Exiting." << std::endl;
    exit(-1);
  };
};
