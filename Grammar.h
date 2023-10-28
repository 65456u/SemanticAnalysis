//
// Created by niets on 10/25/2023.
//

#ifndef GRAMMAR_H
#define GRAMMAR_H

#include <set>
#include <string>
#include <algorithm>
#include <string>
#include <map>
#include <deque>
#include <iostream>
#include <utility>
#include <set>
#include <stack>
#include <vector>
#include "utils.h"

namespace grammar {
    using Symbol = std::string;
    using SymbolSet = std::set<Symbol>;
    using Rule = std::deque<Symbol>;
    using RuleSet = std::set<Rule>;
    using SymbolSetMap = std::map<grammar::Symbol, grammar::SymbolSet>;
    using SymbolProductionMap = std::map<grammar::Symbol, std::set<grammar::Rule>>;
    using LL1ParsingTable = std::map<grammar::Symbol, std::map<grammar::Symbol, grammar::Rule>>;
    using Sentence = std::deque<Symbol>;
    using StringVector = std::vector<std::string>;
}

class Grammar {
private:
protected:
    grammar::SymbolSet terminalSet;
    grammar::SymbolSet nonTerminalSet;
    grammar::SymbolProductionMap productionMap;
    grammar::Symbol startSymbol;
    grammar::Symbol epsilonSymbol = "e";

    void removeLeftRecursion();

    void removeDirectRecursion();

public:
    Grammar(grammar::SymbolSet nonTerminals,
            grammar::SymbolSet terminals,
            grammar::SymbolProductionMap productions,
            grammar::Symbol startSymbol);

    virtual void init() = 0;

    bool isNonTerminal(const grammar::Symbol &symbol);

    bool isTerminal(const grammar::Symbol &symbol);

    void printGrammar(std::ostream &output) const;

    static void printSymbolSetMap(const grammar::SymbolSetMap &symbolSetMap, std::ostream &ostream);

    static std::string ruleToString(const grammar::Symbol &nonTerminal, const grammar::Rule &sequence);

    grammar::Rule tokenizeString(const std::string &str);

    virtual void parse(grammar::Sentence tokens, std::ostream &ostream) = 0;
};

class GrammarException : public std::exception {
private:
    std::string message_;

public:
    explicit GrammarException(std::string message) : message_(std::move(message)) {}

    [[nodiscard]] const char *what() const noexcept override {
        return message_.c_str();
    }
};

#endif //GRAMMAR_H
