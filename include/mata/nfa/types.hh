// Based on
// https://github.com/hiraethese/mata/blob/counter-feature/include/mata/nfa/types.hh

#ifndef TYPES_HH
#define TYPES_HH

#include "mata/utils/ord-vector.hh"

namespace mata::nfa {

using Symbol = unsigned;

using State = unsigned long;
using StateSet = mata::utils::OrdVector<State>;

/// State with a counter (@c State @c state and @c void* @c counter_ptr).
struct CounterState {
    State state; ///< Automaton state.
    void* counter_ptr; ///< Pointer to the counter table when transitioning to a state.

    CounterState() : state(), counter_ptr(nullptr) {}

    CounterState(const CounterState&) = default;
    CounterState(CounterState&&) = default;
    CounterState& operator=(const CounterState&) = default;
    CounterState& operator=(CounterState&&) = default;

    CounterState(const State& state): state{ state }, counter_ptr(nullptr) {} // NOLINT(*-explicit-constructor)
    CounterState(State&& state): state{ state }, counter_ptr(nullptr) {} // NOLINT(*-explicit-constructor)
    CounterState& operator=(const State& other) { state = other; return *this; }
    CounterState& operator=(State&& other) { state = other; return *this; }

    CounterState(const State state, void* counter_ptr) : state(state), counter_ptr(counter_ptr) {} // NOLINT(*-explicit-constructor)

    auto operator<=>(const State& other) const { return state <=> other; }
    bool operator==(const State other) const { return state == other; }
    auto operator<=>(const CounterState&) const = default;
    bool operator==(const CounterState& other) const { return state == other; }


    operator State() const { return state; } // NOLINT(*-explicit-constructor)
};

class CounterStateSet : public mata::utils::OrdVector<CounterState> {
public:
    CounterStateSet() = default;

    CounterStateSet(State state) { // NOLINT(*-explicit-constructor)
        this->push_back(CounterState(state));
    }
    CounterStateSet(StateSet& state_set) { // NOLINT(*-explicit-constructor)
        for (const State& state: state_set) {
            this->push_back(state);
        }
    }
    CounterStateSet(StateSet&& state_set) { // NOLINT(*-explicit-constructor)
        for (const State& state: state_set) {
            this->push_back(state);
        }
    }
    CounterStateSet& operator=(const StateSet& state_set) {
        for (const State& state: state_set) {
            this->push_back(state);
        }
        return *this;
    }
    CounterStateSet& operator=(StateSet&& state_set) {
        for (const State& state: state_set) {
            this->push_back(state);
        }
        return *this;
    }
    CounterStateSet(const CounterStateSet& counter_state_set) = default;
    CounterStateSet(CounterStateSet&& counter_state_set) noexcept = default;
    CounterStateSet& operator=(const CounterStateSet& counter_state_set) = default;
    CounterStateSet& operator=(CounterStateSet&& counter_state_set) noexcept = default;

    // FIXME: This is severely limiting. Basically, this cannot ever be used in production unless explicitly requested.
    //  Should this be explicit? Probably no, but some iteration over CounterStateSet which would behave as a StateSet would be good.
    //  But that is already happening, no?
    operator StateSet() const { // NOLINT(*-explicit-constructor)
        StateSet state_set;
        for (const auto& target : *this) {
            state_set.push_back(target.state);
        }
        return state_set;
    }
};

// Added for better readability.
using Target = CounterState;
using TargetSet = CounterStateSet;

} // namespace mata::nfa.

#endif // TYPES_HH
