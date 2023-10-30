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
#include "../include/utils.h"

namespace grammar {
    using Symbol = std::string;
    using SymbolSet = std::set<Symbol>;
    using Rule = std::deque<Symbol>;
    using RuleSet = std::set<Rule>;
    using SymbolToSymbolSetMap = std::map<grammar::Symbol, grammar::SymbolSet>;
    using SymbolToProductionRuleSetMap = std::map<grammar::Symbol, std::set<grammar::Rule>>;
    using Sentence = std::deque<Symbol>;
    using StringVector = std::vector<std::string>;

    struct ProductionRule {
        Symbol from;
        Rule to;

        bool operator<(const ProductionRule &other) const {
            if (from < other.from) {
                return true;
            }
            return to < other.to;
        }

        bool operator==(const ProductionRule &other) const {
            return from == other.from and to == other.to;
        }

        [[nodiscard]] std::string toString() const {
            std::stringstream result;
            result << from << "->" << utils::join(to, "");
            return result.str();
        }
    };
}

class Grammar {
private:
protected:
    grammar::SymbolSet terminalSet;
    grammar::SymbolSet nonTerminalSet;
    grammar::SymbolToProductionRuleSetMap productionMap;
    grammar::Symbol startSymbol;
    grammar::Symbol epsilonSymbol = "e";
    grammar::SymbolToSymbolSetMap firstSets;
    grammar::SymbolToSymbolSetMap followSets;
    grammar::SymbolSet towardsEpsilonSet;
    grammar::Symbol tailSign = "$";
    bool firstSetsConstructed = false;
    bool followSetsConstructed = false;
    bool towardsEpsilonSetConstructed = false;

    void removeLeftRecursion();

    void constructFirstSets();

    void constructFollowSets();

    void removeDirectRecursion();


    void constructTowardsEpsilonSet();

    void removeEpsilon();

    void func(grammar::Sentence sentence, grammar::RuleSet &output);

    grammar::SymbolSet getFirstSet(const grammar::Symbol &symbol);

public:
    Grammar(grammar::SymbolSet nonTerminals,
            grammar::SymbolSet terminals,
            grammar::SymbolToProductionRuleSetMap productions,
            grammar::Symbol startSymbol);

    virtual void init() = 0;

    virtual void printAll(std::ostream &ostream) = 0;

    virtual void parse(grammar::Sentence tokens, std::ostream &ostream) = 0;

    void printFirstSet(std::ostream &ostream = std::cout) const;

    void printFollowSet(std::ostream &ostream = std::cout) const;

    void printTowardsEpsilonSet(std::ostream &ostream = std::cout) const;

    bool isNonTerminal(const grammar::Symbol &symbol);

    bool isTerminal(const grammar::Symbol &symbol);

    void printGrammar(std::ostream &output = std::cout) const;

    static void printSymbolSetMap(const grammar::SymbolToSymbolSetMap &symbolSetMap, std::ostream &ostream);

    static std::string ruleToString(const grammar::Symbol &nonTerminal, const grammar::Rule &sequence);

    grammar::Rule tokenizeString(const std::string &str);
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
