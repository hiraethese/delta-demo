#ifndef NFA_HH
#define NFA_HH

#include <string>

#include "delta.hh"
#include "../utils/sparse-set.hh"

namespace mata::nfa {

// TODO: Add description.
struct Nfa {
    Delta delta;
    utils::SparseSet<State> initial{};
    utils::SparseSet<State> final{};
    CounterSet counters{}; // Added CounterSet (CounterRegisterSet) member for NFA counters.

    Nfa() = default;
    Nfa(const Delta& delta,
        const utils::SparseSet<State>& initial,
        const utils::SparseSet<State>& final,
        const CounterSet& counters)
        : delta(delta), initial(initial), final(final), counters(counters) {}

    void addInitialState(State state);
    void addFinalState(State state);
    bool simulate(const std::string& input) const;

private:
    bool simulateRecursive(State currentState, const std::string& input, size_t index) const;
};

} // namespace mata::nfa.

#endif // NFA_HH
