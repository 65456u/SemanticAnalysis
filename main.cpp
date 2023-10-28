
#include "Grammar.h"
#include "LL1Grammar.h"

int main() {
    // Define non-terminalSet, terminalSet, productionMap, and start symbol
    using namespace std;
    grammar::SymbolSet nonTerminals = {"E", "T", "F"};
    grammar::SymbolSet terminals = {"+", "-", "*", "/", "(", ")", "num"};
    map<grammar::Symbol, set<grammar::Rule>> productions = {
            {"E", {{"E", "+", "T"}, {"E", "-", "T"}, {"T"}}},
            {"T", {{"T", "*", "F"}, {"T", "/", "F"}, {"F"}}},
            {"F", {{"(", "E", ")"}, {"num"}}}
    };
    grammar::Symbol startSymbol = "E";
    LL1Grammar grammar(nonTerminals, terminals, productions, startSymbol);
    grammar.print();
    grammar.init();
    grammar.print();
    auto tokens = grammar.tokenizeString("*num*+num");
    grammar.parse(tokens, cout);
}