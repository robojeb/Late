#ifndef PTEST_HPP_INC
#define PTEST_HPP_INC

#import <string>
#import <vector>

namespace ptest {

  /*
   * These help us dictate how productions should work
   */
  enum SymbolType {
    NONE,
    NONTERMINAL,
    TERMINAL
  };

  struct Symbol {
    Symbol(std::string value, SymbolType st):
      value_{value},
      type_{st}
    {}
    virtual SymbolType type() const {return type_;}

    bool operator==(const Symbol& rhs) const {
      return value_ == rhs.value_;
    }

    std::string value_;
    SymbolType type_;
  };

  Symbol operator""_nt (const char* s, size_t len) {
    return Symbol(std::string{s}, SymbolType::NONTERMINAL);
  }

  Symbol operator""_t (const char* s, size_t len) {
    return Symbol(std::string{s}, SymbolType::TERMINAL);
  }

  /*
   * Defining a class for the production
   */
  class Production {
  public:
    Production(std::string symbol, std::vector<Symbol> symbols);

    void advance();
    bool complete() const;
    Symbol next() const;
    std::string getSymbol() const;

    bool operator==(const Production& rhs) const;

    friend std::ostream& operator<<(std::ostream& out, const Production& p);
  private:
    std::string symbol_;
    std::vector<Symbol> symbols_;
    size_t position_;

  };

  std::ostream& operator<<(std::ostream& out, const Production& p);
};

#endif
