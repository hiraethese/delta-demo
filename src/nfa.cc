#include "../include/nfa.hh"

void Nfa::addInitialState(State state) {
    initial.insert(state);
}

void Nfa::addFinalState(State state) {
    final.insert(state);
}

// Simulate the NFA
bool Nfa::simulate(const std::string& input) const {
    for (State state : initial) {
        if ( simulateRecursive(state, input, 0) ) {
            return true;
        }
    }
    return false;
}

// Recursive helper function to simulate the NFA
bool Nfa::simulateRecursive(State currentState, const std::string& input, size_t index) const {
    if (index == input.size()) {
        // Note: If we reach the end of the input, check if the current state is accepting.
        return final.count(currentState) > 0;
    }

    Symbol symbol = static_cast<Symbol>(input[index]);
    const auto& transitions = delta.states[currentState].transitions;

    for (const auto& transition : transitions) {
        if (transition.symbol == symbol || transition.symbol == 0) { // 0 for epsilon transitions. TODO: Add EPSILON.
            for (State target : transition.targets) {
                if ( simulateRecursive(target, input, (transition.symbol == 0) ? index : index + 1) ) {
                    return true;
                }
            }
        }
    }

    return false;
}
