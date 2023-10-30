#include "LL1Grammar.h"


grammar::SymbolSet LL1Grammar::getFirstSet(const grammar::Rule &sequence) {
    grammar::SymbolSet firstSet;
    const auto &firstSymbol = sequence.front();
    if (isTerminal(firstSymbol) or firstSymbol == epsilonSymbol) {
        firstSet.insert(firstSymbol);
        return firstSet;
    } else if (isNonTerminal(firstSymbol)) {
        return firstSets[firstSymbol];
    } else {
        throw GrammarException("Unrecognized Symbol 1");
    }
}

void LL1Grammar::constructParsingTable() {
    parsingTable.clear();
    auto newTerminalSet = utils::unionSet(terminalSet, grammar::SymbolSet{tailSign});
    for (const auto &nonTerminal: nonTerminalSet) {
        auto const &productions = productionMap[nonTerminal];
        for (const auto &production: productions) {
            auto firstSet = getFirstSet(production);
            for (const auto &symbol: firstSet) {
                if (isTerminal(symbol)) {
                    insertParsingTableEntry(nonTerminal, symbol, production);
                }
            }
            if (utils::contains(firstSet, epsilonSymbol)) {
                for (const auto &b: followSets[nonTerminal]) {
                    insertParsingTableEntry(nonTerminal, b, production);
                }
            }
        }
    }
    for (const auto &A: nonTerminalSet) {
        for (const auto &b: followSets[A]) {
            if (parsingTable[A][b].empty()) {
                parsingTable[A][b] = grammar::Rule{syncSign};
            }
        }
    }
    parsingTableConstructed = true;
}

void LL1Grammar::parse(grammar::Sentence tokens, std::ostream &ostream) {
    ostream << "Matched" << "\t" << "Stack" << "\t" << "Input" << "\t" << "Action" << std::endl;
    grammar::Sentence stack;
    tokens.push_back(tailSign);
    auto itr = tokens.begin();
    stack.push_back(tailSign);
    stack.push_back(startSymbol);
    while (not stack.empty()) {
        auto X = stack.back();
        auto a = *itr;
        if (a.empty()) {
            a = tailSign;
        }
        if (X == a) {
            stack.pop_back();
            itr++;
            ostream << getParseStep(tokens, itr, stack, "match " + a) << std::endl;
        } else if (isTerminal(X)) {
            throw GrammarException("Parsing Error: Unexpected Terminal");
        } else {
            auto rule = parsingTable[X][a];
            if (rule.empty()) {
                ostream << getParseStep(tokens, itr, stack, "Error Occurred, Skipping " + a)
                        << std::endl;
                itr++;
            } else if (rule.front() == syncSign) {
                ostream << getParseStep(tokens, itr, stack, "Error Occurred, Popping " + X)
                        << std::endl;
                stack.pop_back();
            } else {
                ostream << getParseStep(tokens, itr, stack, "Output " + ruleToString(X, rule))
                        << std::endl;
                stack.pop_back();
                {
                    for (auto reverseIterator = rule.rbegin(); reverseIterator != rule.rend(); reverseIterator++) {
                        if (*reverseIterator != epsilonSymbol) {
                            stack.push_back(*reverseIterator);
                        }
                    }
                }
            }
        }
    }
}


std::string LL1Grammar::getParseStep(grammar::Sentence &tokens,
                                     const std::_Deque_iterator<grammar::Symbol, grammar::Symbol &, grammar::Symbol *> &itr,
                                     const grammar::Sentence &stack, const std::string &action) {
    return utils::join(
            grammar::StringVector{
                    utils::join(tokens.begin(), itr),
                    utils::join(stack.rbegin(), stack.rend()),
                    utils::join(itr, tokens.end()),
                    action
            }, "\t");
}


void LL1Grammar::printParsingTable(std::ostream &ostream) {
    auto newTerminalSet = utils::unionSet(terminalSet, grammar::SymbolSet{tailSign});
    ostream << "" << "\t" << utils::join(newTerminalSet, "\t") << std::endl;
    for (const auto &nonTerminal: nonTerminalSet) {
        ostream << nonTerminal;
        for (const auto &terminal: newTerminalSet) {
            auto rule = parsingTable[nonTerminal][terminal];
            ostream << "\t";
            if (rule.empty()) {
            } else if (rule.front() == syncSign) {
                ostream << syncSign;
            } else {
                ostream << ruleToString(nonTerminal, rule);
            }
        }
        ostream << std::endl;
    }
}

void LL1Grammar::insertParsingTableEntry(const grammar::Symbol &nonTerminal, const grammar::Symbol &terminal,
                                         grammar::Rule production) {
    if (utils::contains(parsingTable[nonTerminal], terminal)) {
        throw GrammarException("Entry Conflict Detected. Not a LL1 Grammar.");
    }
    parsingTable[nonTerminal][terminal] = std::move(production);
}

LL1Grammar::LL1Grammar(grammar::SymbolSet nonTerminals, grammar::SymbolSet terminals,
                       grammar::SymbolToProductionRuleSetMap &productions, grammar::Symbol startSymbol)
        : Grammar(std::move(nonTerminals), std::move(terminals), productions, std::move(startSymbol)) {

}

void LL1Grammar::printAll(std::ostream &ostream) {
    printGrammar(ostream);
    if (firstSetsConstructed) {
        ostream << "First Set:" << std::endl;
        printFirstSet(ostream);
    }
    if (followSetsConstructed) {
        ostream << "Follow Set:" << std::endl;
        printFollowSet(ostream);
    }
    if (towardsEpsilonSetConstructed) {
        ostream << "Symbols Towards Epsilon: ";
        printTowardsEpsilonSet(ostream);
    }
    if (parsingTableConstructed) {
        ostream << "Parsing Table:" << std::endl;
        printParsingTable(ostream);
    }

}

void LL1Grammar::init() {
    removeLeftRecursion();
    constructTowardsEpsilonSet();
    printGrammar();
    printGrammar();
    constructFirstSets();
    constructFollowSets();
    constructParsingTable();

}
