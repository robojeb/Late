#include "late_parser.hpp"
#include "late_ast.hpp"
#include <iostream>
#include <fstream>
#include <streambuf>
#include <iterator>

using namespace late_core;
using namespace late_parser;
using namespace std;

_file_ast handle_file(string& input, Production& p, Chart& chart);
_production_ast handle_production(string& input, Production& p, Chart& chart);
_non_terminal_ast handle_non_terminal(string& input, Production& p, Chart& chart);

_file_ast handle_file(string& input, Production& p, Chart& chart)
{
  int64_t setIndex;
  size_t stateIndex;
  Production production;
  _production_ast p_ast;
  _file_ast out;
  switch (p.number()){
    case 0: //production ws ";" ws file
      //production
      tie(setIndex, stateIndex) = p.symbolInfo(0);
      production = chart.getState(setIndex, stateIndex).first;
      p_ast = handle_production(input, production, chart);
      //We don't care about whitespace or the symbol in this case
      tie(setIndex, stateIndex) = p.symbolInfo(4);
      production = chart.getState(setIndex, stateIndex).first;
      out = handle_file(input, production, chart);
      out.push_back(p_ast);
      return out;
    case 1: // production ws ";" ws
      //This is the terminal file make a new
      //production
      tie(setIndex, stateIndex) = p.symbolInfo(0);
      production = chart.getState(setIndex, stateIndex).first;
      p_ast = handle_production(input, production, chart);
      out = _file_ast({p_ast});
      return out;
    default:
      break;
  }
  return _file_ast{};
}

_production_ast handle_production(string& input, Production& p, Chart& chart)
{
  int64_t setIndex;
  size_t stateIndex;
  Production production;
  _production_ast out = _production_ast();
  //No cases so no switch
  //non_terminal option ws ":=" ws rhs

  tie(setIndex, stateIndex) = p.symbolInfo(0);
  production = chart.getState(setIndex, stateIndex).first;
  _non_terminal_ast name = handle_non_terminal(input, production, chart);

  out.name_ = name;

  // tie(setIndex, stateIndex) = p.symbolInfo(1);
  // production = chart.getState(setIndex, stateIndex).first;
  // _option_ast option = handle_option(input, production, chart);
  //
  // if (option == "nullable") {
  //   out.nullable_ = true;
  // } else {
  //   out.nullable_ = false;
  // }
  //
  // tie(setIndex, stateIndex) = p.symbolInfo(5);
  // production = chart.getState(setIndex, stateIndex).first;
  // _rhs_ast rhs = handle_rhs(input, production, chart);
  //
  // out.sub_productions_ = rhs;

  return out;
}

_non_terminal_ast handle_non_terminal(string& input, Production& p, Chart& chart)
{
  int64_t stringStart;
  size_t stringLen;

  tie(stringStart, stringLen) = p.symbolInfo(0);
  return input.substr(stringStart, stringLen);
}

_file_ast late_parser::parse(string input)
{
  Production start {"start", 0,
    {"_file"_nt}};
  Production _file_0 {"_file", 0,
    {"_production"_nt, "_ws"_nt_n, ";"_t, "_ws"_nt_n, "_file"_nt}};
  Production _file_1 {"_file", 1,
    {"_production"_nt, "_ws"_nt_n, ";"_t, "_ws"_nt_n}};
  Production _production_0 {"_production", 0,
    {"_non_terminal"_nt, "_option"_nt_n, "_ws"_nt_n, ":="_t, "_ws"_nt_n, "_rhs"_nt}};
  Production _non_terminal_0 {"_non_terminal", 0,
    {"[a-zA-Z][a-zA-Z0-9_-]*"_r}};
  Production _rhs_0 {"_rhs", 0,
    {"_sub_production"_nt, "_ws"_nt_n, "|"_t, "_ws"_nt_n, "_rhs"_nt}};
  Production _rhs_1 {"_rhs", 1,
    {"_sub_production"_nt}};
  Production _sub_production_0 {"_sub_production", 0,
    {"_symbol"_nt, "_ws"_nt_n, "_sub_production"_nt}};
  Production _sub_production_1 {"_sub_production", 1,
    {"_symbol"_nt}};
  Production _symbol_0 {"_symbol", 0,
    {"_non_terminal"_nt}};
  Production _symbol_1 {"_symbol", 1,
    {"_string"_nt}};
  Production _symbol_2 {"_symbol", 2,
    {"_regex"_nt}};
  Production _string_0 {"_string", 0,
    {"\"((?:\\\"|(?:.))*)\""_r}};
  Production _regex_0 {"_regex", 0,
    {"r\"((?:\\\"|(?:.))*)\""_r}};
  Production _option_0 {"_option", 0,
    {":"_t, "_option_type"_nt}};
  Production _option_1 {"_option", 1,
    {""_t}};
  Production _option_type_0 {"_option_type", 0,
    {"nullable"_t}};
  Production _ws_0 {"_ws", 0,
    {"\\s*"_r}};

  Grammar g{
    start,
    _file_0,_file_1,
    _production_0,
    _non_terminal_0,
    _rhs_0,_rhs_1,
    _sub_production_0,_sub_production_1,
    _symbol_0, _symbol_1, _symbol_2,
    _string_0,
    _regex_0,
    _option_0, _option_1,
    _option_type_0,
    _ws_0};
  Chart c{input.length()};

  parse_engine(input, g, start, c);

  cout << "Parse: " << (c.parsed(start)?"Succeeded":"Failed") << endl;

  //cout<< c << endl;

  if(c.parsed(start)) {
    Production bootstrap = c.parsedProduction(start);
    size_t setIndex;
    size_t stateIndex;
    tie(setIndex, stateIndex) = bootstrap.symbolInfo(0);
    Production userStart = c.getState(setIndex, stateIndex).first;

    //_file_ast file = handle_file(input, userStart, c);

    return _file_ast{};
  }

  return _file_ast{};
}

int main() {
  ifstream grammar("testGrammars/testHuge.bnf");
  string input{istreambuf_iterator<char>(grammar), istreambuf_iterator<char>()};

  cout << "Input length: " << input.length() << endl;
  _file_ast parsed = parse(input);

  // for(auto p : parsed) {
  //   cout << p.name_ << endl;
  // }
  return 0;
}
