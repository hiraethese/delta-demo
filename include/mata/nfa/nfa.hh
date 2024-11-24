#ifndef NFA_HH
#define NFA_HH

#include <unordered_set>
#include <string>
#include "delta.hh"

// TODO: Add description.
struct Nfa {
    Delta delta;
    std::unordered_set<State> initial;
    std::unordered_set<State> final;

    Nfa() = default;
    Nfa(const Delta& delta,
        const std::unordered_set<State>& initial,
        const std::unordered_set<State>& final)
        : delta(delta), initial(initial), final(final) {}

    void addInitialState(State state);
    void addFinalState(State state);
    bool simulate(const std::string& input) const;

private:
    bool simulateRecursive(State currentState, const std::string& input, size_t index) const;
};

#endif // NFA_HH
