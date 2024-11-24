#include <cstdlib>
#include <fstream>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

#include "Scanner.h"
#include "Token.h"

bool hadError = false;

void report(int line, std::string where, std::string message) {
  std::cout << "[line " << line << "] Error" << where << ": " << message
            << std::endl;
  hadError = true;
}

void error(int line, std::string message) { report(line, "", message); }

void run(std::string source) {
  Scanner scanner(source);
  std::vector<Token> tokens;

  for (Token t : tokens) {
    std::cout << t << std::endl;
  }
}

void runPrompt() {
  std::string line;
  while (true) {
    std::cout << "> " << std::endl;
    std::cin >> line;
    if (line.empty())
      break;
    hadError = false;
  }
}

void runFile(std::string path) {
  std::ifstream f(path);
  std::stringstream buff;
  buff << f.rdbuf();
  run(buff.str());
  if (hadError)
    exit(-1);
}

int main(int argc, char *argv[]) {
  if (argc > 1) {
    std::cout << "Usage: lox [script]" << std::endl;
    return -1;
  } else if (argc == 1) {
    runFile(argv[0]);
  } else {
    runPrompt();
  }
}
