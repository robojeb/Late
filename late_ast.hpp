#ifndef LATE_AST_HPP_INC
#define LATE_AST_HPP_INC

#include <string>
#include <vector>

//Predeclare structures
struct _production_data;
struct _symbol_data;

//We use typedefs so that you can easily replace structures with
//basic data types
typedef _production_data _production_ast;
typedef std::vector<_production_ast> _file_ast;
typedef _symbol_data _symbol_ast;
typedef std::vector<_symbol_ast> _sub_production_ast;
typedef std::vector<_sub_production_ast> _rhs_ast;
typedef std::string _non_terminal_ast;
typedef std::string _string_ast;
typedef std::string _regex_ast;
typedef std::string _option_ast;
typedef std::string _option_type_ast;
typedef std::string _ws_ast;

struct _production_data {
  std::string name_;
  bool nullable_;
  std::vector<_sub_production_ast> sub_productions_;
};

struct _symbol_data {
  enum type {NON_TERMINAL, TERMINAL, REGEX} type_;
  std::string value_;
};



#endif
