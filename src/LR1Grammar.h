#ifndef LR1GRAMMAR_H
#define LR1GRAMMAR_H

#include "Grammar.h"

#include <utility>
#include "../include/utils.h"

namespace grammar {
    struct LR1Item {
        ProductionRule productionRule;
        long read;

        bool operator<(const LR1Item &other) const {
            if (productionRule < other.productionRule) {
                return true;
            }
            if (read < other.read) {
                return true;
            }
            return follow < other.follow;
        }

        grammar::Symbol nextSymbolToRead() {
            if (read < productionRule.to.size()) {
                return productionRule.to[read];
            } else {
                return "";
            }
        }

        bool operator==(const LR1Item &other) const {
            return productionRule == other.productionRule and
                   read == other.read and
                   follow == other.follow;
        }

        [[nodiscard]] grammar::Sentence afterNext() const {
            auto rule = productionRule.to;
            if (read < rule.size() - 1) {
                auto startIndex = rule.begin() + read + 1;
                auto endIndex = rule.end();
                return {startIndex, endIndex};
            } else {
                return grammar::Sentence{};
            }
        }

        void print(std::ostream &ostream = std::cout, const Symbol &delimiter = ".") const {
            ostream << "[" << productionRule.from << "->";
            const auto &rule = productionRule.to;
            for (size_t i = 0; i < rule.size(); i++) {
                if (i == read) {
                    ostream << delimiter;
                }
                ostream << rule[i];
            }
            if (read == rule.size()) {
                ostream << delimiter;
            }
            ostream << "," << follow << "]" << std::endl;
        }

        LR1Item(ProductionRule rule,
                long position,
                grammar::Symbol nextSymbol) :
                productionRule(std::move(rule)),
                read(position),
                follow(std::move(nextSymbol)) {}

        grammar::Symbol follow;


    };

    using LR1ItemSet = std::set<LR1Item>;
    using LR1ItemCollection = std::map<size_t, LR1ItemSet>;
    using LR1ItemSetMap = std::map<size_t, LR1ItemSet>;
    using ProductionVector = std::vector<ProductionRule>;
    enum class ActionType {
        SHIFT, REDUCE, REJECT, ACCEPT, ERROR
    };
    using State = size_t;
    using Action = struct Action {
        ActionType type;
        size_t value;

        bool operator==(const Action &other) const {
            return type == other.type and value == other.value;
        }
    };
    enum class StackItemType {
        SYMBOL, STATE
    };
    using StackUnion = class StackUnion {
    private:
        StackItemType type;
        Symbol symbol;
        State state = 0;
    public:
        explicit StackUnion(Symbol s) {
            type = StackItemType::SYMBOL;
            symbol = std::move(s);
            state = 0;
        }

        explicit StackUnion(State s) {
            type = StackItemType::STATE;
            state = s;
            symbol = "";
        }

        Symbol getSymbol() {
            if (type == StackItemType::SYMBOL) {
                return symbol;
            } else {
                return "";
            }
        }

        State getState() {
            if (type == StackItemType::STATE) {
                return state;
            } else {
                return 2952;
            }
        }

        StackItemType getType() {
            return type;
        }
    };
    using StateStack = std::deque<StackUnion>;

}
class LR1Grammar : public Grammar {
private:
    grammar::LR1ItemCollection itemCollection;
    grammar::Symbol previousStartSymbol;
    grammar::ProductionRule originalProduction;
    grammar::Symbol delimiter = ".";
    grammar::LR1ItemSetMap itemSetMap;
    size_t itemSetCount = 0;
    size_t productionCount = 0;
    std::map<size_t, std::map<grammar::Symbol, size_t>> edges;
    grammar::ProductionVector productionVector;
    std::map<std::string, size_t> productionIndices;
    std::map<grammar::State, std::map<grammar::Symbol, grammar::Action >> actionTable;
    std::map<grammar::State, std::map<grammar::Symbol, grammar::State >> gotoTable;
    grammar::SymbolSet extendedTerminalSet;
    bool productionsIndexed = false;
    bool canonicalItemSetCollectionConstructed = false;
    bool parsingTableConstructed = false;

    void constructAugmentedGrammar();

    void constructCanonicalItemCollection();

    void constructParsingTable();

    grammar::LR1ItemSet closure(grammar::LR1ItemSet itemSet);

    grammar::LR1ItemSet go(const grammar::LR1ItemSet &I, const grammar::Symbol &x);

    void indexProductions() {
        size_t count = 0;
        productionIndices.clear();
        productionVector.clear();
        for (const auto &nonTerminal: nonTerminalSet) {
            for (const auto &rule: productionMap[nonTerminal]) {
                auto productionRule = grammar::ProductionRule{nonTerminal, rule};
                productionVector.push_back(productionRule);
                productionIndices[productionRule.toString()] = count;
                ++count;
            }
        }
        productionCount = count;
        productionsIndexed = true;
    }

    void printEdges(std::ostream &ostream = std::cout) {
        for (const auto &p: edges) {
            for (const auto &q: p.second) {
                ostream << p.first << "\t" << q.first << "\t" << q.second << std::endl;
            }
        }
    }

    static std::string stackToString(grammar::StateStack stack) {
        std::stringstream result;
        for (auto itr = stack.rbegin(); itr != stack.rend(); itr++) {
            auto item = *itr;
            if (item.getType() == grammar::StackItemType::STATE) {
                result << item.getState();
            } else {
                result << item.getSymbol();
            }
        }
        return result.str();
    }


public:
    void init() override;

    void parse(grammar::Sentence tokens, std::ostream &ostream) override;

    void printAll(std::ostream &ostream = std::cout);

    void printItemSetsCollection(std::ostream &ostream = std::cout) const {
        for (const auto &pair: itemSetMap) {
            ostream << pair.first << std::endl;
            for (const auto &item: pair.second) {
                item.print();
            }
        }
    }

    void printParsingTable(std::ostream &ostream = std::cout) {
        ostream << "\t" << utils::join(extendedTerminalSet, "\t");
        for (const auto &nonTerminal: nonTerminalSet) {
            if (nonTerminal != startSymbol) {
                ostream << "\t" << nonTerminal;
            }
        }
        ostream<<std::endl;
        for (size_t i = 0; i < itemSetCount; i++) {
            ostream << i << "\t";
            for (const auto &t: extendedTerminalSet) {
                auto action = actionTable[i][t];
                switch (action.type) {
                    case grammar::ActionType::ERROR:
                        ostream << "\t";
                        break;
                    case grammar::ActionType::ACCEPT:
                        ostream << "ACC\t";
                        break;
                    case grammar::ActionType::SHIFT:
                        ostream << "S" << action.value << "\t";
                        break;
                    case grammar::ActionType::REDUCE:
                        ostream << "R" << action.value << "\t";
                        break;
                    case grammar::ActionType::REJECT:
                        ostream << "REJ\t";
                        break;
                }
            }
            for (const auto &nonTerminal: nonTerminalSet) {
                if (nonTerminal != startSymbol) {
                    if (utils::contains(gotoTable[i], nonTerminal)) {
                        ostream << gotoTable[i][nonTerminal];
                    }
                    ostream << "\t";
                }
            }
            ostream << std::endl;
        }
    }

    void printProductions(std::ostream &ostream) {
        for (size_t i = 0; i < productionVector.size(); i++) {
            ostream << i << "\t" << productionVector[i].toString() << std::endl;
        }
    }

    LR1Grammar(grammar::SymbolSet nonTerminals, grammar::SymbolSet terminals,
               grammar::SymbolToProductionRuleSetMap &productions, grammar::Symbol startSymbol);

    static std::string getParseStep(const std::deque<grammar::State> &stack, const grammar::Sentence &symbols,
                                    const std::_Deque_iterator<grammar::Symbol, const grammar::Symbol &, const grammar::Symbol *> &itr,
                                    const grammar::Sentence &tokens, std::string action);
};


#endif //LR1GRAMMAR_H
