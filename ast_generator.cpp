#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
#include <unordered_map>
#include <vector>
#include <utility>

using namespace std;

void defineAst(string outputDir, string baseName, 
  unordered_map<string, vector<pair<string, string>>> types,
  string visitReturnType,
  vector<string> extraIncludes = { }) {
  // TODO
  // I'm currently using std::any for the output but there's probably a more idiomatic
  // way to do this (AI suggests std::variant)
  // When I learn about it in learncpp I should migrate the code to use it 
  ofstream outFile { outputDir + "/" + baseName + ".hpp" };
  if (!outFile) {
    cerr << "Failed to open file" << endl;
    exit(-1);
  }
  outFile << "#pragma once" << endl;
  outFile << endl;
  outFile << "#include <any>" << endl;
  outFile << "#include <vector>" << endl;
  outFile << "#include \"Token.hpp\"" << endl;
  for (auto s : extraIncludes)
    outFile << "#include \"" << s << "\"" << endl;
  outFile << endl;
  outFile << "struct " << baseName << "Visitor;" << endl;
  outFile << endl;
  outFile << "struct " << baseName << " {" << endl;
  outFile << "  " << "virtual " << visitReturnType << " accept(" << baseName << "Visitor &visitor) = 0;" << endl;
  outFile << "  " << "virtual ~" << baseName << "() = default;" << endl;
  outFile << "};" << endl;
  outFile << endl;

  for (auto t : types) {
    string className = t.first;
    outFile << "struct " << className << ";" << endl;
  }

  outFile << endl;

  outFile << "struct " << baseName << "Visitor {" << endl;
  for (auto t : types) {
    string className = t.first;
    string baseNameLower = baseName;
    std::transform(baseNameLower.begin(), baseNameLower.end(), baseNameLower.begin(), [](unsigned char c) { return std::tolower(c);});
    outFile << "  virtual " << visitReturnType << " visit" << className << baseName << "(" << className << " &" << baseNameLower <<  ") = 0;" << endl;
  }

  outFile << "};" << endl;
  outFile << endl;


  for (auto t : types) {
    string className = t.first;
    vector<pair<string, string>> vars = t.second;
    
    outFile << "struct " << className << " : public " << baseName << " {" << endl;
    for (auto v : vars) {
      string type = v.first;
      string id = v.second;
      outFile << "  " << ((type == baseName | type == "Expr") ? "std::unique_ptr<" : "") 
          << type << ((type == baseName | type == "Expr") ? ">" : "") << " " << id << ";" << endl;

    }
    outFile << "  " << className << "(";
    for (int i = 0; i < vars.size(); ++i) {
      string type = vars[i].first;
      string id = vars[i].second;
      outFile << ((type == baseName | type == "Expr") ? "std::unique_ptr<" : "") 
          << type << ((type == baseName | type == "Expr") ? ">" : "") 
          << ((className == "Literal" || className == "Variable" || className == "Block") ? " " : " &") << id;
      if (i != vars.size() - 1)
          outFile << ", ";
    }

    outFile << ") : ";

    for (int i = 0; i < vars.size(); ++i) {
      string type = vars[i].first;
      string id = vars[i].second;
      outFile << id << " { " 
          << ((type == baseName | type == "Expr") ? "std::move(" : "") << id 
          << ((type == baseName | type == "Expr") ? ")" : "") << " }";
      if (i != vars.size() - 1)
          outFile << ", ";
    }
    outFile << " {};" << endl;
    outFile << endl;
    outFile << "  " << visitReturnType << " accept(" << baseName << "Visitor &visitor) override {" << endl;
    outFile << "    " << "return visitor.visit" << className << baseName + "(*this);" << endl;
    outFile << "  }" << endl;
    outFile << "};" << endl;
    outFile << endl;
  }
}

int main(int argc, char** argv) {
  if (argc != 2) {
    cout << "Usage: ast_generator <output dir>" << endl;
    return -1;
  }
  string outputDir { argv[1] };

  unordered_map<string, vector<pair<string, string>>> exprTypes = {
    {"Assign", {{"Token", "name"}, {"Expr", "value"}}},
    {"Binary", {{"Expr", "left"}, {"Token", "op"}, {"Expr", "right"}}},
    {"Grouping", {{"Expr", "expr"}}},
    {"Literal", {{"std::any", "value"}}},
    {"Unary", {{"Token", "op"}, {"Expr", "right"}}},
    {"Variable", {{"Token", "name"}}},
  };
  unordered_map<string, vector<pair<string, string>>> stmtTypes = {
    {"Block", {{"std::vector<std::unique_ptr<Stmt>>", "statements"}}},
    {"Expression", {{"Expr", "expr"}}},
    {"Print", {{"Expr", "expr"}}},
    {"Var", {{"Token", "name"}, {"Expr", "initializer"}}},
  };

  defineAst(outputDir, "Expr", exprTypes, "std::any");
  defineAst(outputDir, "Stmt", stmtTypes, "void", {"Expr.hpp"});

  return 0;
}
