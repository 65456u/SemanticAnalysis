#include <iostream>
#include "Grammar.h"
#include "LR1Grammar.h"

int main() {
    using namespace std;
    grammar::SymbolSet terminals = {"+", "-", "*", "/", "(", ")", "num"};
    grammar::SymbolSet nonTerminals = {"E", "T", "F"};
    map<grammar::Symbol, set<grammar::Rule>> productions = {
            {"E", {{"E", "+", "T"}, {"E", "-", "T"}, {"T"}}},
            {"T", {{"T", "*", "F"}, {"T", "/", "F"}, {"F"}}},
            {"F", {{"(", "E", ")"}, {"num"}}}
    };
    grammar::Symbol startSymbol = "E";
    LR1Grammar g(nonTerminals, terminals, productions, startSymbol);
    g.init();
    g.printAll();
    while (true) {
        try {
            string input;
            cin >> input;
            auto tokens = g.tokenizeString(input);
            g.parse(tokens, cout);
        } catch (GrammarException &e) {
            cerr << e.what() << endl;
        }
    }
}
