#ifndef LL1GRAMMAR_H
#define LL1GRAMMAR_H

#include <stack>
#include <utility>
#include <array>
#include "Grammar.h"

class LL1Grammar : public Grammar {
private:
    grammar::SymbolSetMap firstSets;
    grammar::SymbolSetMap followSets;
    grammar::Symbol tailSign = "$";
    grammar::LL1ParsingTable parsingTable;
    grammar::SymbolSet towardsEpsilonSet;
    grammar::Symbol syncSign = "sync";

    bool firstSetsConstructed = false;
    bool followSetsConstructed = false;
    bool towardsEpsilonSetConstructed = false;
    bool parsingTableConstructed = false;

    void constructFirstSets();

    void constructFollowSets();

    void constructTowardsEpsilonSet();

    void constructParsingTable();

    void insertParsingTableEntry(const grammar::Symbol &nonTerminal,
                                 const grammar::Symbol &terminal,
                                 grammar::Rule production);

    static std::string getParseStep(grammar::Sentence &tokens,
                                    const decltype(tokens.begin()) &itr,
                                    const grammar::Sentence &stack,
                                    const std::string &action);

public:
    grammar::SymbolSet getFirstSet(const grammar::Symbol &symbol);

    void printFirstSet(std::ostream &ostream = std::cout) const;

    void printFollowSet(std::ostream &ostream = std::cout) const;

    void printTowardsEpsilonSet(std::ostream &ostream = std::cout) const;

    void printParsingTable(std::ostream &ostream);

    void print(std::ostream &ostream = std::cout);

    LL1Grammar(grammar::SymbolSet nonTerminals,
               grammar::SymbolSet terminals,
               grammar::SymbolProductionMap &productions,
               grammar::Symbol startSymbol);

    grammar::SymbolSet getFirstSet(const grammar::Rule &symbol);

    void parse(grammar::Sentence tokens, std::ostream &ostream) override;

    void init() override;
};


#endif //LL1GRAMMAR_H
