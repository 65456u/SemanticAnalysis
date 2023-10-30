#ifndef LL1GRAMMAR_H
#define LL1GRAMMAR_H

#include <stack>
#include <utility>
#include <array>
#include "Grammar.h"

namespace grammar {
    using LL1ParsingTable = std::map<grammar::Symbol, std::map<grammar::Symbol, grammar::Rule>>;
}
class LL1Grammar : public Grammar {
private:

    grammar::LL1ParsingTable parsingTable;
    grammar::Symbol syncSign = "sync";

    bool parsingTableConstructed = false;


    void constructParsingTable();

    void insertParsingTableEntry(const grammar::Symbol &nonTerminal,
                                 const grammar::Symbol &terminal,
                                 grammar::Rule production);

    static std::string getParseStep(grammar::Sentence &tokens,
                                    const decltype(tokens.begin()) &itr,
                                    const grammar::Sentence &stack,
                                    const std::string &action);

public:

    void printParsingTable(std::ostream &ostream);

    void printAll(std::ostream &ostream) override;

    LL1Grammar(grammar::SymbolSet nonTerminals,
               grammar::SymbolSet terminals,
               grammar::SymbolToProductionRuleSetMap &productions,
               grammar::Symbol startSymbol);

    grammar::SymbolSet getFirstSet(const grammar::Rule &symbol);

    void parse(grammar::Sentence tokens, std::ostream &ostream) override;

    void init() override;
};


#endif //LL1GRAMMAR_H
