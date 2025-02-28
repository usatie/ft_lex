/*
 * regex_compiler.cpp
 *
 * This program is a C++ implementation of the third phase of the Ken Thompson's
 * regular expression compiler. Aim of this C++ program is to visualize the
 * process of compiling a regular expression into IBM 7094 assembly code.
 *
 * The program is originally written in ALGOL-60 by Ken Thompson in 1968.
 * It is a regular expression compiler that translates a regular expression
 * into IBM 7094 assembly code.
 * https://www.oilshell.org/archive/Thompson-1968.pdf
 *
 * This web page by Russ Cox describes the IBM 7094 and its instructions needed
 * to understand the paper.
 * https://swtch.com/~rsc/regexp/ibm7094.html
 *
 */
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

class Instruction {
public:
  Instruction() {
    opcode = "";
    addr = "";
    index = 0;
    decr = 0;
  }
  Instruction(std::string opcode, std::string addr, int index, int decr) {
    this->opcode = opcode;
    this->addr = addr;
    this->index = index;
    this->decr = decr;
  }
  std::string opcode;
  std::string addr;
  int index;
  int decr;
};

std::ostream &operator<<(std::ostream &os, const Instruction &inst) {
  std::stringstream ss;
  if (inst.opcode == "TRA") {
    ss << std::left << std::setw(8) << inst.opcode << inst.addr;
  } else if (inst.opcode == "TSX") {
    ss << std::left << std::setw(8) << inst.opcode << inst.addr << ","
       << inst.index;
  } else if (inst.opcode == "TXL") {
    ss << std::left << std::setw(8) << inst.opcode << inst.addr << ","
       << inst.index << ",-'" << std::string(1, -(inst.decr + 1)) << "'-1";
  } else if (inst.opcode == "TXH") {
    ss << std::left << std::setw(8) << inst.opcode << inst.addr << ","
       << inst.index << ",-'" << std::string(1, -inst.decr) << "'";
  } else {
    ss << std::left << std::setw(8) << inst.opcode << inst.addr << ","
       << inst.index << "," << inst.decr;
  }
  os << ss.str();
  return os;
}

enum { E_ALPHA, E_JUXTA, E_CLOSURE, E_OR, E_EOF };

int index(int c) {
  if (isalpha(c)) {
    return E_ALPHA;
  } else if (c == '&') {
    return E_JUXTA;
  } else if (c == '*') {
    return E_CLOSURE;
  } else if (c == '|') {
    return E_OR;
  } else if (c == '\0') {
    return E_EOF;
  } else {
    return -1;
  }
}

void print_code(const std::vector<Instruction> &code, int pc) {
  for (int i = 0; i < pc; ++i) {
    if (i == 0) {
      std::cout << "CODE    " << std::left << std::setw(24) << code[i] << "    "
                << i << std::endl;
    } else {
      std::cout << "        " << std::left << std::setw(24) << code[i] << "    "
                << i << std::endl;
    }
  }
}

int main(int argc, char *argv[]) {
  std::string regex = "abc|*&d&";
  if (argc == 2) {
    regex = argv[1];
  }
  std::vector<int> stack(10);
  std::vector<Instruction> code(300);
  int lc = 0, pc = 0;
  int i = 0;
advance:
  print_code(code, pc);
  char c = regex[i++];
  switch (index(c)) {
  case E_ALPHA:
    std::cout << "alpha(" << c << ")" << std::endl;
    code[pc] = Instruction("TRA", "CODE+" + std::to_string(pc + 1), 0, 0);
    code[pc + 1] = Instruction("TXL", "FAIL", 1, -c - 1);
    code[pc + 2] = Instruction("TXH", "FAIL", 1, -c);
    code[pc + 3] = Instruction("TSX", "NNODE", 4, 0);
    stack[lc] = pc;
    pc = pc + 4;
    lc = lc + 1;
    goto advance;
  case E_JUXTA:
    std::cout << "juxta(" << c << ")" << std::endl;
    lc = lc - 1;
    goto advance;
  case E_CLOSURE:
    std::cout << "closure(" << c << ")" << std::endl;
    code[pc] = Instruction("TSX", "CNODE", 4, 0);
    code[pc + 1] = code[stack[lc - 1]];
    code[stack[lc - 1]] =
        Instruction("TRA", "CODE+" + std::to_string(pc), 0, 0);
    pc = pc + 2;
    goto advance;
  case E_OR:
    std::cout << "or(" << c << ")" << std::endl;
    code[pc] = Instruction("TRA", "CODE+" + std::to_string(pc + 4), 0, 0);
    code[pc + 1] = Instruction("TSX", "CNODE", 4, 0);
    code[pc + 2] = code[stack[lc - 1]];
    code[pc + 3] = code[stack[lc - 2]];
    code[stack[lc - 2]] =
        Instruction("TRA", "CODE+" + std::to_string(pc + 1), 0, 0);
    code[stack[lc - 1]] =
        Instruction("TRA", "CODE+" + std::to_string(pc + 4), 0, 0);
    pc = pc + 4;
    lc = lc - 1;
    goto advance;
  case E_EOF:
    std::cout << "eof(" << c << ")" << std::endl;
    code[pc] = Instruction("TRA", "FOUND", 0, 0);
    pc = pc + 1;
  }
  print_code(code, pc);
}
