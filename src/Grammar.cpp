
#include "Grammar.h"


void Grammar::removeLeftRecursion() {
    removeDirectRecursion();
}

void Grammar::removeDirectRecursion() {
    for (const auto &production: productionMap) {
        const auto &nonTerminal = production.first;
        const auto &sequences = production.second;
        grammar::RuleSet nonRecursiveSequences;
        grammar::RuleSet recursiveSequences;
        for (const auto &sequence: sequences) {
            if (sequence.front() == nonTerminal) {
                grammar::Rule subSequence(sequence.begin() + 1, sequence.end());
                recursiveSequences.insert(subSequence);
            }
        }
        if (recursiveSequences.empty()) {
            continue;
        } else {
            for (const auto &sequence: sequences) {
                if (sequence.front() != nonTerminal) {
                    nonRecursiveSequences.insert(sequence);
                }
            }
            grammar::Symbol newNonTerminal = nonTerminal + "'";
            grammar::RuleSet newSequences;
            for (auto sequence: nonRecursiveSequences) {
                sequence.push_back(newNonTerminal);
                newSequences.insert(sequence);
            }
            nonTerminalSet.insert(newNonTerminal);
            productionMap[nonTerminal] = newSequences;
            grammar::RuleSet newNewSequences;
            for (auto sequence: recursiveSequences) {
                sequence.push_back(newNonTerminal);
                newNewSequences.insert(sequence);
            }
            newNewSequences.insert(grammar::Rule{epsilonSymbol});
            productionMap[newNonTerminal] = newNewSequences;
        }
    }

}

grammar::Sentence Grammar::tokenizeString(const std::string &str) {
    grammar::Sentence tokens;
    size_t startPos = 0;
    while (startPos < str.length()) {
        size_t found = std::string::npos;
        std::string matchedTerminal;
        for (const auto &terminal: terminalSet) {
            found = str.find(terminal, startPos);
            if (found != std::string::npos && (found == startPos)) {
                matchedTerminal = terminal;
                break;
            }
        }
        if (found != std::string::npos) {
            tokens.push_back(matchedTerminal);
            startPos = found + matchedTerminal.length();
        } else {
            startPos++;
        }
    }
    return tokens;
}

std::string Grammar::ruleToString(const grammar::Symbol &nonTerminal, const grammar::Rule &sequence) {
    std::stringstream ss;
    ss << nonTerminal << "->";
    for (const auto &symbol: sequence) {
        ss << symbol;
    }
    return ss.str();
}

void Grammar::printSymbolSetMap(const grammar::SymbolToSymbolSetMap &symbolSetMap, std::ostream &ostream) {
    for (const auto &row: symbolSetMap) {
        ostream << row.first << " : " << utils::join(row.second, " ") << std::endl;
    }
}

void Grammar::printGrammar(std::ostream &output) const {
    output << "Non-terminalSet: " << utils::join(nonTerminalSet, " ");
    output << std::endl;

    output << "Terminals: " << utils::join(terminalSet, " ");
    output << std::endl;

    output << "Productions:" << std::endl;
    for (const auto &production: productionMap) {
        const auto &nonTerminal = production.first;
        const auto &sequences = production.second;
        output << nonTerminal << " -> ";
        bool first = true;
        for (const auto &sequence: sequences) {
            if (first) {
                first = false;
            } else {
                output << '|';
            }
            for (const auto &symbol: sequence) {
                output << symbol;
            }
        }
        output << std::endl;
    }
    output << "Starting Symbol: " << startSymbol << std::endl;
}

bool Grammar::isTerminal(const grammar::Symbol &symbol) {
    return terminalSet.find(symbol) != terminalSet.end();
}

bool Grammar::isNonTerminal(const grammar::Symbol &symbol) {
    return nonTerminalSet.find(symbol) != nonTerminalSet.end();
}

Grammar::Grammar(grammar::SymbolSet nonTerminals, grammar::SymbolSet terminals,
                 grammar::SymbolToProductionRuleSetMap productions, grammar::Symbol startSymbol)
        : nonTerminalSet(std::move(nonTerminals)), terminalSet(std::move(terminals)),
          productionMap(std::move(productions)),
          startSymbol(std::move(startSymbol)) {}


void Grammar::constructFirstSets() {
    for (const auto &nonTerminal: nonTerminalSet) {
        grammar::SymbolSet symbolSet{nonTerminal};
        bool altered = true;
        while (altered) {
            altered = false;
            for (const auto &currentSymbol: symbolSet) {
                for (auto production: productionMap[currentSymbol]) {
                    auto firstSymbol = production.front();
                    if (isTerminal(firstSymbol)) {
                        utils::insert(firstSets[nonTerminal], firstSymbol, altered);
                        firstSets[currentSymbol].insert(firstSymbol);
                    } else if (isNonTerminal((firstSymbol))) {
                        utils::insert(symbolSet, firstSymbol, altered);
                    }
                }
            }
        }
    }
    firstSetsConstructed = true;
}

void Grammar::constructFollowSets() {
    followSets[startSymbol].insert(tailSign);
    bool altered = true;
    while (altered) {
        altered = false;
        for (const auto &nonTerminal: nonTerminalSet) {
            for (const auto &production: productionMap[nonTerminal]) {
                for (size_t i = 0; i < production.size() - 1; i++) {
                    const auto &currentSymbol = production[i];
                    if (isNonTerminal(currentSymbol)) {
                        for (size_t j = i + 1; j < production.size(); j++) {
                            auto nextSymbol = production[j];
                            if (isTerminal(nextSymbol)) {
                                utils::insert(followSets[currentSymbol], nextSymbol, altered);
                            } else if (isNonTerminal(nextSymbol)) {
                                for (auto symbol: firstSets[nextSymbol]) {
                                    if (symbol != epsilonSymbol) {
                                        utils::insert(followSets[currentSymbol], symbol, altered);
                                    }
                                }
                            }
                        }
                    }
                }
                for (auto reverseIterator = production.rbegin();
                     reverseIterator != production.rend(); reverseIterator++) {
                    const auto &lastSymbol = *reverseIterator;
                    if (isTerminal(lastSymbol) or lastSymbol == epsilonSymbol) {
                        break;
                    } else if (isNonTerminal(lastSymbol)) {
                        for (auto symbol: followSets[nonTerminal]) {
                            utils::insert(followSets[lastSymbol], symbol, altered);
                        }
                        if (not utils::contains(towardsEpsilonSet, lastSymbol)) {
                            break;
                        }
                    } else {
                        throw GrammarException("Invalid Symbol 2");
                    }
                }
            }
        }
    }
    followSetsConstructed = true;
}

grammar::SymbolSet Grammar::getFirstSet(const grammar::Symbol &symbol) {
    if (symbol == epsilonSymbol or symbol == tailSign or isTerminal(symbol)) {
        return grammar::SymbolSet{symbol};
    } else if (isNonTerminal(symbol)) {
        if (firstSetsConstructed) {
            return firstSets[symbol];
        }
        grammar::SymbolSet firstSet;
        grammar::SymbolSet symbolSet{symbol};
        bool altered = true;
        while (altered) {
            altered = false;
            for (auto currentSymbol: symbolSet) {
                if (isNonTerminal(currentSymbol)) {
                    for (auto production: productionMap[currentSymbol]) {
                        auto firstSymbol = production.front();
                        if (isTerminal(firstSymbol)) {
                            utils::insert(firstSet, firstSymbol, altered);
                        } else if (isNonTerminal(firstSymbol)) {
                            utils::insert(symbolSet, firstSymbol, altered);
                        }
                    }
                } else if (isTerminal(currentSymbol)) {
                    utils::insert(firstSet, currentSymbol, altered);
                }
            }
        }
        if (firstSet.empty()) {
            firstSet.insert(epsilonSymbol);
        }
        return firstSet;
    } else {
        throw GrammarException("Can't Find Symbol Set: Unrecognized Symbol");
    }
}

void Grammar::printFirstSet(std::ostream &ostream) const {
    printSymbolSetMap(firstSets, ostream);
}

void Grammar::printFollowSet(std::ostream &ostream) const {
    printSymbolSetMap(followSets, ostream);
}

void Grammar::printTowardsEpsilonSet(std::ostream &ostream) const {
    ostream << utils::join(towardsEpsilonSet, " ") << std::endl;
}

void Grammar::constructTowardsEpsilonSet() {

    bool altered = true;
    while (altered) {
        altered = false;
        for (auto nonTerminal: nonTerminalSet) {
            for (const auto &production: productionMap[nonTerminal]) {
                bool allSymbolsInEpsilonSet = std::all_of(production.begin(), production.end(),
                                                          [&](const auto &symbol) {
                                                              return utils::contains(towardsEpsilonSet, symbol);
                                                          });
                if (production == grammar::Sentence({epsilonSymbol}) or allSymbolsInEpsilonSet) {
                    utils::insert(towardsEpsilonSet, nonTerminal, altered);
                }
            }
        }
    }
    towardsEpsilonSetConstructed = true;
}

void Grammar::func(grammar::Sentence sentence, grammar::RuleSet &output) {
    for (long i = 0; i < sentence.size(); i++) {
        if (utils::contains(towardsEpsilonSet, sentence[i])) {
            grammar::Sentence newSentence;
            newSentence.assign(sentence.begin(), sentence.end());
            newSentence.erase(newSentence.begin() + i);
            output.insert(newSentence);
            func(newSentence, output);
        }
    }
}

void Grammar::removeEpsilon() {
    if (not towardsEpsilonSetConstructed) {
        constructTowardsEpsilonSet();
    }
    for (const auto &nonTerminal: nonTerminalSet) {
        for (const auto &production: productionMap[nonTerminal]) {
            func(production, productionMap[nonTerminal]);
        }
    }
    for (const auto &nonTerminal: nonTerminalSet) {
        auto itr = productionMap[nonTerminal].begin();
        while (itr != productionMap[nonTerminal].end()) {
            if ((*itr).front() == epsilonSymbol) {
                itr = productionMap[nonTerminal].erase(itr);
            } else {
                itr++;
            }
        }
    }
}
