#include "../../include/mata/nfa/delta.hh"

void StatePost::addTransition(Symbol symbol, const StateSet& targets) {
    transitions.emplace_back(symbol, targets);
}

void Delta::addState(const StatePost& statePost) {
    states.push_back(statePost);
}

void Delta::addTransition(State source, Symbol symbol, const StateSet& targets) {
    if (source >= states.size()) {
        states.resize(source + 1);
    }
    states[source].addTransition(symbol, targets);
}
