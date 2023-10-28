
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

void Grammar::printSymbolSetMap(const grammar::SymbolSetMap &symbolSetMap, std::ostream &ostream) {
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
                 grammar::SymbolProductionMap productions, grammar::Symbol startSymbol)
        : nonTerminalSet(std::move(nonTerminals)), terminalSet(std::move(terminals)),
          productionMap(std::move(productions)),
          startSymbol(std::move(startSymbol)) {}

