#ifndef LATE_PARSER_HPP_INC
#define LATE_PARSER_HPP_INC

#include <string>
#include <tuple>
#include "late_core.hpp"
#include "late_ast.hpp"

//Grammar
// S := S "+" M | M
// M := M "*" T | T
// T := "1" | "2" | "3" | "4"

namespace late_parser{
  _file_ast parse(std::string input);
};

#endif
