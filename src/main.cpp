#include <cstdlib>
#include <fstream>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

#include "interpreter/Scanner.hpp"
#include "interpreter/error.hpp"
#include "interpreter/Parser.hpp"
#include "interpreter/Interpreter.hpp"

bool hadError = false;
bool hadRuntimeError = false;
Interpreter interpreter { };

void run(std::string source) {
  Scanner scanner(source);
  std::vector<Token> tokens = scanner.scanTokens();
  Parser parser { tokens };
  std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

  if (hadError)
    return;
  interpreter.interpret(statements);
}

void runPrompt() {
  std::string line;
  while (true) {
    std::cout << "> ";
    std::getline(std::cin, line);
    if (line.empty())
      break;
    run(line);
    hadError = false;
  }
}

void runFile(std::string path) {
  std::ifstream f(path);
  std::stringstream buff;
  buff << f.rdbuf();
  run(buff.str());
  if (hadError)
    exit(65);
  if (hadRuntimeError)
    exit(70);
}

int main(int argc, char *argv[]) {
  if (argc > 2) {
    std::cout << "Usage: lox [script]" << std::endl;
    return -1;
  } else if (argc == 2) {
    runFile(argv[1]);
  } else {
    runPrompt();
  }
}
