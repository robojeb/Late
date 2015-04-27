#include <iterator>
#include <iostream>
#include <regex>
#include <tuple>

#include "late_core.hpp"

using namespace std;
using namespace late_core;
/*
 * Generic stuff in all late parsers.
 */

 /*
  * Define code for Symbol
  */

Symbol::Symbol(string value, SymbolType type, bool nullable):
  value_{value},
  type_{type},
  nullable_{nullable}
{
  //Nothing to do
}

SymbolType Symbol::type() const
{
  return type_;
}

string Symbol::value() const
{
  return value_;
}

bool Symbol::nullable() const
{
  return nullable_;
}

pair<bool, size_t> Symbol::matchesInput(string input, size_t position) const
{
  //TODO make this match more than one character.
  //For now we only check if the next character is our value
  //cout << "SCANNING" << endl;
  if (type_ == SymbolType::TERMINAL){
    string nextChars = input.substr(position, value_.length());
    if(nextChars == value_){
      cout << "String Matched: " << value_ << endl;
      return make_pair(true,value_.length());
    }
    cout << "No Match: " << value_ << endl;
  } else if (type_ == SymbolType::REGEX) {
    //Make a regular expression out of the value;
    regex valueRegex{value_, regex_constants::ECMAScript};
    smatch match;
    string remaining = input.substr(position);
    if(regex_search(remaining, match, valueRegex)){
      if(distance(match.prefix().first, match[0].first) == 0) {
        //cout << "Regex matched: " << value_ << " " << match[0] << endl;
        //cout << distance(match[0].first, match[0].second) << endl;
        return make_pair(true, distance(match[0].first, match[0].second));
      }
    }
  }
  return make_pair(false, 0);
}

bool Symbol::operator==(const Symbol& rhs) const
{
  return value_ == rhs.value_ && type_ == rhs.type_;
}

ostream& late_core::operator<<(ostream& out, const Symbol& p)
{
  out << p.value_;
  return out;
}

//Non-nullable symbols
Symbol late_core::operator""_nt (const char* s, size_t len)
{
  return Symbol(string{s}, SymbolType::NONTERMINAL, false);
}

Symbol late_core::operator""_t (const char* s, size_t len)
{
  return Symbol(string{s}, SymbolType::TERMINAL, false);
}

Symbol late_core::operator""_r (const char* s, size_t len)
{
  return Symbol(string{s}, SymbolType::REGEX, false);
}

//Nullable symbols
Symbol late_core::operator""_nt_n (const char* s, size_t len)
{
  return Symbol(string{s}, SymbolType::NONTERMINAL, true);
}

/*
 * Define code for productions
 */

Production::Production():
  name_{},
  number_{},
  rhs_{},
  position_{0},
  backPtrs_{}
{
}

Production::Production(string name, size_t number, vector<Symbol> rhs):
  name_{name},
  number_{number},
  rhs_{rhs},
  position_{0}, //New productions have never consumed any input
  backPtrs_{}
{
  //Nothing to do
}

void Production::advance()
{
  position_ += 1;
  //We consumed a terminal symbol so it doesn't matter where our back-pointer
  //goes
  backPtrs_.push_back(make_pair(-1,0));
}

void Production::advance(pair<int64_t, size_t> ptr)
{
  position_ += 1;
  backPtrs_.push_back(ptr);
}

bool Production::isComplete() const
{
  //If our position is past the last element of the production we are done
  return position_ >= rhs_.size();
}

Symbol Production::nextSymbol() const
{
  return rhs_[position_];
}

string Production::name() const
{
  return name_;
}

size_t Production::number() const
{
  return number_;
}

pair<int64_t, size_t> Production::symbolInfo(size_t symbolNumber) const
{
  return backPtrs_[symbolNumber];
}

bool Production::operator==(const Production& rhs) const
{
  //We don't care about the back pointers being the same.
  //We just don't want duplicate positions in the production
  return name_ == rhs.name_ && rhs_ == rhs.rhs_ && position_ == rhs.position_;
}

ostream& late_core::operator<<(ostream& out, const Production& p)
{
  out << p.name_ << " -> ";
  for (int i = 0; i < p.rhs_.size(); ++i) {
    if (i == p.position_) {
      cout << "•";
    }
    if(p.backPtrs_.size() > i){
      cout << "<" << p.backPtrs_[i].first << "," << p.backPtrs_[i].second << ">";
    }
    out << p.rhs_[i];
  }
  if (p.position_ == p.rhs_.size()) {
    cout << "•";
  }
  return out;
}

/*
 * Helper print function for states
 */

ostream& late_core::operator<<(ostream& out, const State& s)
{
  out << s.first << ", " << s.second;
  return out;
}

/*
 * Define the code for Chart
 */

Chart::Chart(size_t inputSize):
  chart_{new vector<State>[inputSize+1]}, //We need columns 0-inputSize
  numSets_{inputSize+1}
{
  //Nothing to do
}

Chart::~Chart()
{
  delete[] chart_;
}

void Chart::addToSet(size_t setIndex, State s)
{
  //If this state has been added then skip it
  for (State other : chart_[setIndex]) {
    if (other == s) return;
  }
  cout << "Adding to "<< setIndex << ": " << s << endl;
  chart_[setIndex].push_back(s);
}

size_t Chart::numSets() const
{
  return numSets_;
}

size_t Chart::setSize(size_t setIndex) const
{
  return chart_[setIndex].size();
}

State Chart::getState(size_t setIndex, size_t stateIndex) const
{
  return chart_[setIndex][stateIndex];
}

bool Chart::parsed(Production p) const
{
  p.advance();
  for(State other : chart_[numSets_-1]) {
    if(other.first == p) {
      return true;
    }
  }
  return false;
}

Production Chart::parsedProduction(Production p) const
{
  p.advance();
  for(State other : chart_[numSets_-1]) {
    if(other.first == p) {
      return other.first;
    }
  }
  return Production{};
}

ostream& late_core::operator<<(ostream& out, const Chart& c)
{
  for(int i = 0; i < c.numSets_; ++i) {
    cout << "State Set: " << i << endl;
    for (State s : c.chart_[i]) {
      cout << s << endl;
    }
    cout << endl;
  }

  return out;
}

/*
 *  The parsing functions
 *  Some of the names here might be a bit weird to match the names in the
 *  psuedo code found here:
 *  http://en.wikipedia.org/wiki/Earley_parser#Pseudocode
 */

void predictor(State s, size_t j, Grammar& grammar, Chart& chart)
{
  cout << "Predicting " << s << endl;

  for(Production p : grammar) {
    //If this production is the next symbol in the state we are predicting
    if (p.name() == s.first.nextSymbol().value()) {
      //Add (s, j) to set j
      chart.addToSet(j, State{p, j});
    }
  }

  //Magically advance over nullable symbols
  if (s.first.nextSymbol().nullable()) {
    Production nextProduction{s.first};
    nextProduction.advance(); //Consume one symbol
    chart.addToSet(j, State{nextProduction, s.second});
  }
}

void scanner(State s, size_t j, string input, Chart& chart)
{
  cout << "Scanning " << s << endl;
  bool match;
  size_t matchSize;
  tie(match, matchSize) = s.first.nextSymbol().matchesInput(input, j);
  if (match) {
    cout << "Scan success: " << matchSize << endl;
    Production nextProduction{s.first};
    nextProduction.advance(make_pair(j, matchSize)); //Consume one symbol
    chart.addToSet(j+matchSize, State{nextProduction, s.second});
  }
}

void completer(State s, size_t k, Chart& chart, pair<size_t, size_t> pos)
{
  cout << "Completing " << s << endl;
  size_t j = s.second;
  for (int stateIndex = 0; stateIndex < chart.setSize(j); ++stateIndex) {
    State otherState = chart.getState(j, stateIndex);
    //If other state is not complete and the next symbol in otherState is a
    //non-terminal and the next symbol in other state is the name of the state
    //we just completed. then we perform a completion action on otherState
    if (!otherState.first.isComplete() &&
        otherState.first.nextSymbol().type() == SymbolType::NONTERMINAL &&
        otherState.first.nextSymbol().value() == s.first.name()) {
          Production nextProduction{otherState.first};
          nextProduction.advance(pos);
          chart.addToSet(k, State{nextProduction, otherState.second});
    }
  }
}

//The parse function will take an input, a grammar, a startRule, and a chart
//and will fill the chart accortind to the algorithm rules. The chart is assumed
//to be empty. The grammar is assumed to have a bootstraping rule which is
//passed into startRule
void late_core::parse_engine(string input, Grammar& grammar,
                  Production startRule, Chart& chart)
{
  //Add the start rule to seed the chart
  chart.addToSet(0, State{startRule, 0});
  for (int i = 0; i < input.length()+1; ++i) {
    cout << i << endl;
    for(int j = 0; j < chart.setSize(i); ++j) {
      State s = chart.getState(i, j);
      if(s.first.isComplete()) {
        //If the production is complete call the completer
        completer(s, i, chart, make_pair(i, j));
      } else {
        if (s.first.nextSymbol().type() == SymbolType::NONTERMINAL) {
          //Predict on non-terminals
          predictor(s, i, grammar, chart);
        } else {
          //Scan on terminals
          scanner(s, i, input, chart);
        }
      }
    }
  }
  //cout << chart << endl;
}
