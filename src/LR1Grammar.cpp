#include "LR1Grammar.h"

#include <utility>

void LR1Grammar::parse(grammar::Sentence tokens, std::ostream &ostream) {
    ostream << utils::join(grammar::StringVector{
            "STACK", "SYMBOLS", "INPUT", "ACTION"
    }, "\t") << std::endl;
    std::deque<grammar::State> stateStack;
    grammar::Sentence symbolStack;
    stateStack.push_back(0);
    tokens.push_back(tailSign);
    auto itr = tokens.begin();
    while (true) {
        auto s = stateStack.back();
        auto token = *itr;
        auto action = actionTable[s][token];
        if (action.type == grammar::ActionType::SHIFT) {
            auto t = action.value;
            ostream << getParseStep(stateStack, symbolStack, itr, tokens, "SHIFT") << std::endl;
            stateStack.push_back(t);
            symbolStack.push_back(token);
            itr++;
        } else if (action.type == grammar::ActionType::REDUCE) {
            auto production = productionVector[action.value];
            ostream << getParseStep(stateStack, symbolStack, itr, tokens, "REDUCE " + production.toString())
                    << std::endl;
            for (size_t i = 0; i < production.to.size(); i++) {
                if (symbolStack.back() == production.to[production.to.size() - 1 - i]) {
                    symbolStack.pop_back();
                    stateStack.pop_back();
                } else {
                    ostream << getParseStep(stateStack, symbolStack, itr, tokens, "ERROR") << std::endl;
                    throw GrammarException("Parsing Error: Symbols Do Not Match Production");
                }
            }
            symbolStack.push_back(production.from);
            auto previousState = stateStack.back();
            if (utils::contains(gotoTable[previousState], production.from)) {
                stateStack.push_back(gotoTable[previousState][production.from]);
            }
        } else if (action.type == grammar::ActionType::ACCEPT) {
            ostream << getParseStep(stateStack, symbolStack, itr, tokens, "ACCEPT") << std::endl;
            break;
        } else {
            ostream << getParseStep(stateStack, symbolStack, itr, tokens, "ERROR") << std::endl;
            throw GrammarException("Syntax Error");
        }
    }
}

void LR1Grammar::init() {
    constructAugmentedGrammar();
    removeEpsilon();
    extendedTerminalSet = utils::unionSet(terminalSet, {tailSign});
    indexProductions();
    constructCanonicalItemCollection();
    constructParsingTable();
}

void LR1Grammar::constructAugmentedGrammar() {
    auto newStartingSymbol = startSymbol + "'";
    nonTerminalSet.insert(newStartingSymbol);
    auto firstRule = grammar::Rule{startSymbol};
    productionMap[newStartingSymbol].insert(firstRule);
    originalProduction = grammar::ProductionRule{newStartingSymbol, firstRule};
    previousStartSymbol = startSymbol;
    startSymbol = newStartingSymbol;

}

LR1Grammar::LR1Grammar(grammar::SymbolSet nonTerminals, grammar::SymbolSet terminals,
                       grammar::SymbolToProductionRuleSetMap &productions, grammar::Symbol startSymbol)
        : Grammar(std::move(nonTerminals), std::move(terminals), productions, std::move(startSymbol)) {

}

void LR1Grammar::constructCanonicalItemCollection() {
    auto firstItem = grammar::LR1Item(originalProduction, 0, tailSign);
    auto C = closure({firstItem});
    itemSetMap[0] = C;
    itemSetCount = 1;
    const grammar::SymbolSet allSymbols = utils::unionSet(extendedTerminalSet, nonTerminalSet);
    for (size_t i = 0; i < itemSetCount; i++) {
        auto itemSet = itemSetMap[i];
        for (const auto &symbol: allSymbols) {
            auto newItemSet = go(itemSet, symbol);
            if (newItemSet.empty()) {
                continue;
            }
            size_t newItemSetIndex = itemSetCount;
            if (not utils::findKey(itemSetMap, newItemSet, newItemSetIndex)) {
                itemSetMap[newItemSetIndex] = newItemSet;
                itemSetCount++;
            }
            edges[i][symbol] = newItemSetIndex;
        }
    }
    canonicalItemSetCollectionConstructed = true;
}

grammar::LR1ItemSet LR1Grammar::closure(grammar::LR1ItemSet itemSet) {
    bool altered = true;
    while (altered) {
        altered = false;
        for (auto item: itemSet) {
            auto nextSymbol = item.nextSymbolToRead();
            if (isNonTerminal(nextSymbol)) {
                auto afterRule = item.afterNext();
                afterRule.push_back(item.follow);
                auto newNextSet = getFirstSet(afterRule.front());
                for (const auto &rule: productionMap[nextSymbol]) {
                    auto newRule = grammar::ProductionRule{nextSymbol, rule};
                    for (const auto &b: newNextSet) {
                        auto newItem = grammar::LR1Item(newRule, 0, b);
                        utils::insert(itemSet, newItem, altered);
                    }
                }
            }
        }
    }
    return itemSet;
}

grammar::LR1ItemSet LR1Grammar::go(const grammar::LR1ItemSet &I, const grammar::Symbol &x) {
    grammar::LR1ItemSet newItemSet;
    for (auto item: I) {
        if (item.nextSymbolToRead() == x) {
            item.read++;
            newItemSet.insert(item);
        }
    }
    return closure(newItemSet);
}

void LR1Grammar::constructParsingTable() {
    for (size_t i = 0; i < itemSetCount; i++) {
        for (auto item: itemSetMap[i]) {
            auto a = item.nextSymbolToRead();
            auto production = item.productionRule;
            if (a.empty()) {
                auto A = production.from;
                if (A == startSymbol) {
                    if (production.to == grammar::Sentence{previousStartSymbol}) {
                        actionTable[i][tailSign] = {grammar::ActionType::ACCEPT, 0};
                    } else {
                        throw GrammarException("Invalid Starting Symbol Action");
                    }
                } else {
                    {
                        if (not utils::contains(productionIndices, production.toString())) {
                            std::cerr << production.toString() << std::endl;
                            throw GrammarException("Error Finding Production");
                        }
                        auto newAction = grammar::Action{
                                grammar::ActionType::REDUCE,
                                productionIndices[production.toString()]
                        };
                        if (utils::contains(actionTable[i], item.follow) and
                            actionTable[i][item.follow] == newAction) {
                            throw GrammarException("Conflict Action, Not a LR1 Grammar");
                        } else {
                            actionTable[i][item.follow] = {
                                    grammar::ActionType::REDUCE,
                                    productionIndices[item.productionRule.toString()]
                            };
                        }
                    }
                }
            } else {
                if (utils::contains(edges[i], a)) {
                    auto j = edges[i][a];
                    actionTable[i][a] = {grammar::ActionType::SHIFT, j};
                }
            }
            for (const auto &nt: nonTerminalSet) {
                if (utils::contains(edges[i], nt)) {
                    gotoTable[i][nt] = edges[i][nt];
                }
            }
            for (const auto &t: extendedTerminalSet) {
                if (not utils::contains(actionTable[i], t)) {
                    actionTable[i][t] = {grammar::ActionType::ERROR, 0};
                }
            }
        }
    }
    parsingTableConstructed = true;
}

std::string LR1Grammar::getParseStep(const std::deque<grammar::State> &stack, const grammar::Sentence &symbols,
                                     const std::_Deque_iterator<grammar::Symbol, const grammar::Symbol &, const grammar::Symbol *> &itr,
                                     const grammar::Sentence &tokens, std::string action) {
    return utils::join(grammar::StringVector{
            utils::join(stack, ""),
            utils::join(symbols, ""),
            utils::join(itr, tokens.end(), ""),
            std::move(action)
    }, "\t");
}

void LR1Grammar::printAll(std::ostream &ostream) {
    printGrammar(ostream);
    if (productionsIndexed) {
        ostream << "Indexed Productions:" << std::endl;
        printProductions(ostream);
    }
    if (canonicalItemSetCollectionConstructed) {
        ostream << "Canonical Item Sets Collection:" << std::endl;
        printItemSetsCollection(ostream);
        ostream << "GOTO:" << std::endl;
        printEdges(ostream);
    }
    if (parsingTableConstructed) {
        ostream << "Parsing Table:" << std::endl;
        printParsingTable(ostream);
    }
}

void LR1Grammar::printParsingTable(std::ostream &ostream) {
    ostream << "\t" << utils::join(extendedTerminalSet, "\t");
    for (const auto &nonTerminal: nonTerminalSet) {
        if (nonTerminal != startSymbol) {
            ostream << "\t" << nonTerminal;
        }
    }
    ostream << std::endl;
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

void LR1Grammar::printProductions(std::ostream &ostream) {
    for (size_t i = 0; i < productionVector.size(); i++) {
        ostream << i << "\t" << productionVector[i].toString() << std::endl;
    }
}

void LR1Grammar::printItemSetsCollection(std::ostream &ostream) const {
    for (const auto &pair: itemSetMap) {
        ostream << pair.first << std::endl;
        for (const auto &item: pair.second) {
            item.print();
        }
    }
}

void LR1Grammar::indexProductions() {
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
    productionsIndexed = true;
}


