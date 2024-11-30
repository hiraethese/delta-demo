// Based on
// https://github.com/hiraethese/mata/blob/counter-feature/include/mata/nfa/types.hh

#ifndef TYPES_HH
#define TYPES_HH

#include <iostream>
#include <limits>

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

// TODO: Choose the best value for counters.
using CounterValue = unsigned long;
using CounterValueSet = mata::utils::OrdVector<CounterValue>;

/// Register for counters.
struct CounterRegister {
    int id; ///< Unique ID for the counter.
    // Note: ID is temporary to show the properties of counters.
    // TODO: Is there a need for a hash table with counters?
    CounterValue value; ///< Current counter value.
    CounterValue initial_value; ///< Initial counter value.

    CounterRegister() : id(-1), value(0), initial_value(0) {}
    CounterRegister(int id, CounterValue value) : id(id), value(value), initial_value(value) {}

    CounterRegister(const CounterRegister&) = default;
    CounterRegister(CounterRegister&&) = default;
    CounterRegister& operator=(const CounterRegister&) = default;
    CounterRegister& operator=(CounterRegister&&) = default;

    CounterRegister& operator=(CounterValue other) { value = other; return *this; }

    auto operator<=>(const CounterValue& other) const { return value <=> other; }
    bool operator==(const CounterValue& other) const { return value == other; }
    auto operator<=>(const CounterRegister&) const = default;
    bool operator==(const CounterRegister& other) const = default;

    operator CounterValue() const { return value; }

    // Increment the counter by 1 (or specified amount).
    void increment(CounterValue amount = 1) {
        if (value > std::numeric_limits<CounterValue>::max() - amount) {
            throw std::overflow_error("CounterRegister: Increment operation would result in overflow.");
        }
        value += amount;
    }
    // Decrement the counter by 1 (or specified amount).
    void decrement(CounterValue amount = 1) {
        if (amount > value) {
            throw std::underflow_error("CounterRegister: Decrement operation would result in a negative value.");
        }
        value -= amount;
    }
    // Reset the counter to its initial value.
    void reset() { value = initial_value; }
    // Note: Custom debug output. This should be removed later.
    void print() const {
        std::cout << "ID: " << id << ", Value: " << value << ", Initial: " << initial_value << "\n";
    }
};

class CounterRegisterSet : public mata::utils::OrdVector<CounterRegister> {
public:
    CounterRegisterSet() = default;

    CounterRegisterSet(int id, CounterValue value) {
        this->push_back(CounterRegister(id, value));
    }
    CounterRegisterSet(int id_start, CounterValueSet& value_set) {
        int id = id_start; // TODO: Change this later to better counter ID. Use this approach to test counters.
        for (const CounterValue& value: value_set) {
            this->push_back(CounterRegister(id, value));
            ++id;
        }
    }
    CounterRegisterSet(int id_start, CounterValueSet&& value_set) {
        int id = id_start; // TODO: Change this later to better counter ID. Use this approach to test counters.
        for (const CounterValue& value: value_set) {
            this->push_back(CounterRegister(id, value));
            ++id;
        }
    }

    CounterRegisterSet(const CounterRegisterSet& counter_register_set) = default;
    CounterRegisterSet(CounterRegisterSet&& counter_register_set) noexcept = default;
    CounterRegisterSet& operator=(const CounterRegisterSet& counter_register_set) = default;
    CounterRegisterSet& operator=(CounterRegisterSet&& counter_register_set) noexcept = default;

    operator CounterValueSet() const {
        CounterValueSet counter_value_set;
        for (const auto& counter_register : *this) {
            counter_value_set.push_back(counter_register.value);
        }
        return counter_value_set;
    }
};

// Added for better readability.
using Target = CounterState;
using TargetSet = CounterStateSet;
using Counter = CounterRegister;
using CounterSet = CounterRegisterSet;

} // namespace mata::nfa.

#endif // TYPES_HH
