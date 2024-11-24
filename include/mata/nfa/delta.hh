#ifndef DELTA_HH
#define DELTA_HH

#include <vector>

using State = unsigned long;
using Symbol = unsigned;
using StateSet = std::vector<State>;

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
    std::vector<SymbolPost> transitions;

    StatePost() = default;
    StatePost(const std::vector<SymbolPost>& transitions) : transitions(transitions) {}

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

#endif // DELTA_HH
