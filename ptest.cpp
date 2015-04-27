#import "ptest.hpp"
#import <tuple>
#import <iostream>

using namespace std;
using namespace ptest;

typedef vector<Production> grammar;
typedef tuple<Production, int> state;
typedef vector<state> chart_t;

Production::Production(string symbol, vector<Symbol> symbols):
  symbol_{symbol},
  symbols_{symbols},
  position_{0}
{
  //Nothing to do
}

bool Production::complete() const {
  return position_ == symbols_.size();
}

void Production::advance() {
  ++position_;
}

Symbol Production::next() const {
  return symbols_[position_];
}

string Production::getSymbol() const {
  return symbol_;
}

ostream& ptest::operator<<(ostream& out, const Production& p) {
  out << p.symbol_ << " -> ";
  for(int i = 0; i < p.symbols_.size(); ++i) {
    if(p.position_ == i) {
      out << "•";
    }
    out << p.symbols_[i].value_;
  }
  if(p.position_ == p.symbols_.size()) {
    out << "•";
  }
  return out;
}

bool Production::operator==(const Production& rhs) const {
  return symbol_ == rhs.symbol_ && symbols_ == rhs.symbols_ && position_ == rhs.position_;
}

bool inChart(state s, chart_t& chart) {
  for (auto x : chart) {
    if (x == s) {
      return true;
    }
  }
  return false;
}


void printChart(chart_t* chart, size_t chartLen) {
  for(int i = 0; i <= chartLen; ++i) {
    cout << "Set: " << i << endl;
    for (auto x: chart[i]){
      Production p = get<0>(x);
      size_t origin = get<1>(x);
      cout << p << ", " << origin << endl;
    }
    cout << endl;
  }
}
//Define the parsing functions

void predictor(state s, size_t position, grammar g, chart_t* chart) {
  Production p = get<0>(s);
  //cout << "Predicting: " << p << endl;
  string nextSymbol = p.next().value_;
  //cout << "Next Symbol: " << nextSymbol << endl;
  for(auto x : g) {
    if (x.getSymbol() == nextSymbol) {
      if (!inChart(state{x, position}, chart[position])) {
        chart[position].push_back(state{x, position});
      }
    }
  }
}

void scanner(state s, size_t position, string input, chart_t* chart) {
  Production p = get<0>(s);
  size_t j = get<1>(s);
  //cout << "Scanning: " << p << endl;
  //cout << "Expected: " << p.next().value_ << " Next: " << string{input[position]} << endl;
  if (p.next().value_ == string{input[position]}) {
    //cout << "Success" << endl;
    Production nextProduction{p};
    nextProduction.advance();
    if (!inChart(state{nextProduction, position}, chart[position+1])){
      chart[position+1].push_back(state{nextProduction, j});
    }
  }
}

void completer(state s, size_t position, chart_t* chart) {
  Production p = get<0>(s);
  size_t j = get<1>(s);
  //cout << "Completing: " << p << " @ " << j << endl;
  for(auto x : chart[j]) {
    Production xp = get<0>(x);
    size_t i = get<1>(x);
    if(!xp.complete() && xp.next().value_ == p.getSymbol()) {
      //cout << "Completes: " << xp << ", " << i << endl;
      Production nextProduction = xp;
      nextProduction.advance();
      if (!inChart(state{nextProduction, i}, chart[position])) {
        chart[position].push_back(state{nextProduction, i});
      }
    }
  }
}


// This is the test language
// E := E + E | T
// T := 1 | 2 | 3

bool parse(string input) {
  //Build the grammar

  // Production start{"__start", {"E"_nt}};
  // Production _E_0{"E", {"E"_nt, "+"_t, "E"_nt}};
  // Production _E_1{"E", {"T"_nt}};
  // Production _T_0{"T", {"1"_t}};
  // Production _T_1{"T", {"2"_t}};
  // Production _T_2{"T", {"3"_t}};

  Production start{"P", {"S"_nt}};
  Production _S_0{"S", {"S"_nt, "+"_t, "M"_nt}};
  Production _S_1{"S", {"M"_nt}};
  Production _M_0{"M", {"M"_nt, "*"_t, "T"_nt}};
  Production _M_1{"M", {"T"_nt}};
  Production _T_0{"T", {"1"_t}};
  Production _T_1{"T", {"2"_t}};
  Production _T_2{"T", {"3"_t}};
  Production _T_3{"T", {"4"_t}};

  //Not put in the grammar only used for checking termination
  Production endProduction = start;
  endProduction.advance(); //Make end the completed start state

  state end{endProduction, 0};

  // grammar g{start, _E_0, _E_1, _T_0, _T_1, _T_2};
  grammar g{start, _S_0, _S_1, _M_0, _M_1, _T_0, _T_1, _T_2, _T_3};

  //Create the chart
  chart_t* chart = new chart_t[input.length()+1];
  //Perform the parse
  chart[0].push_back(state{start, 0});
  for (int i = 0; i < input.length()+1; ++i) {
    for(int j = 0; j < chart[i].size(); ++j){
      state s = chart[i][j];
      Production p = get<0>(s);
      size_t origin = get<1>(s);
      if (i == input.length() && s == end) {
        printChart(chart, input.length());
        delete[] chart;
        return true;
      }
      //cout << p << ", " << origin << " @ " << i << endl;
      if (!p.complete()) {
        if(p.next().type() == SymbolType::NONTERMINAL) {
          predictor(s, i, g, chart);
        } else {
          scanner(s, i, input, chart);
        }
      } else {
        completer(s, i, chart);
      }
    }
  }
  printChart(chart, input.length());
  delete[] chart;
  return false;
}

int main() {
  cout << parse("2+3*4*3*3*3+2+1*2+1") << endl;
  return 1;
}
