#include <iostream>
#include <cstring>
#include "Grammar.h"
#include "LR1Grammar.h"
#include "LL1Grammar.h"

int main(int argc, char **argv) {
    using namespace std;
    grammar::SymbolSet terminals = {"+", "-", "*", "/", "(", ")", "num"};
    grammar::SymbolSet nonTerminals = {"E", "T", "F"};
    map<grammar::Symbol, set<grammar::Rule>> productions = {
            {"E", {{"E", "+", "T"}, {"E", "-", "T"}, {"T"}}},
            {"T", {{"T", "*", "F"}, {"T", "/", "F"}, {"F"}}},
            {"F", {{"(", "E", ")"}, {"num"}}}
    };
    grammar::Symbol startSymbol = "E";
    char *type = nullptr;
    if (argc > 1) {
        type = argv[1];
    }

    unique_ptr<Grammar> grammarParser;

    if (type and strcmp(type, "LL") == 0) {
        grammarParser = make_unique<LL1Grammar>(nonTerminals, terminals, productions, startSymbol);
    } else {
        grammarParser = make_unique<LR1Grammar>(nonTerminals, terminals, productions, startSymbol);
    }

    grammarParser->init();
    grammarParser->printAll(cout);

    while (true) {
        try {
            string input;
            cin >> input;
            auto tokens = grammarParser->tokenizeString(input);
            grammarParser->parse(tokens, cout);
        } catch (GrammarException &e) {
            cerr << e.what() << endl;
        }
    }
}
