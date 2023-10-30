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
        SHIFT, REDUCE, ACCEPT, ERROR
    };
    using State = size_t;
    using Action = struct Action {
        ActionType type;
        size_t value;

        bool operator==(const Action &other) const {
            return type == other.type and value == other.value;
        }
    };
}
class LR1Grammar : public Grammar {
private:
    grammar::LR1ItemCollection itemCollection;
    grammar::Symbol previousStartSymbol;
    grammar::ProductionRule originalProduction;
    grammar::Symbol delimiter = ".";
    grammar::LR1ItemSetMap itemSetMap;
    size_t itemSetCount = 0;
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

    void indexProductions();

    void printEdges(std::ostream &ostream = std::cout) {
        for (const auto &p: edges) {
            for (const auto &q: p.second) {
                ostream << p.first << "\t" << q.first << "\t" << q.second << std::endl;
            }
        }
    }


public:
    void init() override;

    void parse(grammar::Sentence tokens, std::ostream &ostream) override;

    void printAll(std::ostream &ostream) override;

    void printItemSetsCollection(std::ostream &ostream = std::cout) const;

    void printParsingTable(std::ostream &ostream = std::cout);

    void printProductions(std::ostream &ostream);

    LR1Grammar(grammar::SymbolSet nonTerminals, grammar::SymbolSet terminals,
               grammar::SymbolToProductionRuleSetMap &productions, grammar::Symbol startSymbol);

    static std::string getParseStep(const std::deque<grammar::State> &stack, const grammar::Sentence &symbols,
                                    const std::_Deque_iterator<grammar::Symbol, const grammar::Symbol &, const grammar::Symbol *> &itr,
                                    const grammar::Sentence &tokens, std::string action);
};


#endif //LR1GRAMMAR_H
