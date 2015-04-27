#ifndef LATE_CORE_HPP_INC
#define LATE_CORE_HPP_INC

#include <iostream>
#include <string>
#include <utility>
#include <vector>

namespace late_core {
  /*
   * Generic parser declarations
   */
  enum SymbolType {
    NONTERMINAL, //A symbol that indicates another production
    TERMINAL, //A pure string match
    REGEX //match the next characters based on a regex
  };

  //Productions are made up of symbols
  class Symbol {
  public:
    Symbol(std::string value, SymbolType type, bool nullable);
    //Get the type of the symbol
    SymbolType type() const;

    //Get the value of this symbol
    std::string value() const;

    //Is this production nullable?
    bool nullable() const;

    //Check if this symbol matches the next N characters and return the length
    //of the match. This should only be called on TERMINAL and REGEX types
    std::pair<bool, size_t> matchesInput(std::string input, size_t position) const;

    bool operator==(const Symbol& rhs) const;

    friend std::ostream& operator<<(std::ostream& out, const Symbol& p);
  private:
    std::string value_;
    SymbolType type_;
    bool nullable_;
  };

  //These custom literals make defining productions easier
  Symbol operator""_nt (const char* s, size_t len);
  Symbol operator""_t  (const char* s, size_t len);
  Symbol operator""_r  (const char* s, size_t len);
  //These indicate nullable productions
  Symbol operator""_nt_n (const char* s, size_t len);

  //A production indicates one rule for the grammar
  class Production {
  public:
    Production();
    Production(std::string name, size_t number, std::vector<Symbol> rhs);

    //Advances over a terminal/regex symbol
    void advance();
    //Advances over a non-terminal and points to where that was completed
    //We use int64_t so we can know if it is an empty production or if
    //it was actually parsed
    void advance(std::pair<int64_t, size_t> ptr);

    //Is this production completed
    bool isComplete() const;

    //Get the next symbol
    Symbol nextSymbol() const;

    //Get the name of this production
    std::string name() const;

    //Which version of this named production is this
    size_t number() const;

    std::pair<int64_t, size_t> symbolInfo(size_t symbolNumber) const;

    bool operator==(const Production& rhs) const;

    friend std::ostream& operator<<(std::ostream& out, const Production& p);

  private:
    std::string name_;
    size_t number_;
    std::vector<Symbol> rhs_;
    //Where are we in the current production 0-#symbols
    size_t position_;
    std::vector<std::pair<int64_t, size_t>> backPtrs_;
  };

  //A grammar is just a list of productions
  typedef std::vector<Production> Grammar;

  //A state is a production and an origin point
  typedef std::pair<Production, size_t> State;
  //We want this so we can pring debugging nicely
  std::ostream& operator<< (std::ostream& out, const State& s);


  class Chart {
  public:
    Chart(size_t inputSize);
    ~Chart();

    //Adds a state to the set at index setIndex only if it is not already
    //in that set
    void addToSet(size_t setIndex, State s);

    size_t numSets() const;
    size_t setSize(size_t setIndex) const;

    State getState(size_t setIndex, size_t stateIndex) const;

    bool parsed(Production p) const;

    Production parsedProduction(Production p) const;

    friend std::ostream& operator<< (std::ostream& out, const Chart& c);
  private:
    std::vector<State>* chart_;
    size_t numSets_;
  };

  void parse_engine(std::string input, Grammar& grammar,
                    Production startRule, Chart& chart);

};

#endif
