#ifndef DELTA_HH
#define DELTA_HH

#include <vector>

#include "mata/utils/ord-vector.hh"
#include "types.hh"

namespace mata::nfa {

// TODO: Add description.
class SymbolPost {
public:
    Symbol symbol{};
    StateSet targets{};

    SymbolPost() = default;
    SymbolPost(Symbol symbol, const StateSet& targets) : symbol(symbol), targets(targets) {}
};

// TODO: Add description.
class StatePost {
public:
    utils::OrdVector<SymbolPost> transitions;

    StatePost() = default;
    StatePost(const utils::OrdVector<SymbolPost>& transitions) : transitions(transitions) {}

    void addTransition(Symbol symbol, const StateSet& targets);
};

// TODO: Add description.
class Delta {
public:
    std::vector<StatePost> states;

    Delta() = default;
    Delta(const std::vector<StatePost>& states) : states(states) {}

    void addState(const StatePost& statePost);
    void addTransition(State source, Symbol symbol, const StateSet& targets);
};

} // namespace mata::nfa.

#endif // DELTA_HH
