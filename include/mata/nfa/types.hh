// Based on
// https://github.com/hiraethese/mata/blob/counter-feature/include/mata/nfa/types.hh

#ifndef TYPES_HH
#define TYPES_HH

#include <cstddef>
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

    // Notes about (void* counter_ptr):
    // It seems that using void pointer is not very user friendly.
    // Ideally use a pointer to a vector of pointers to TransitionAnnotation objects.
    // This will most likely lead to a problem with casting from one type to another.
    // This is inconvenient for testing and will most likely slow down the library.

    // Example of using:
    // Create a vector of pointers to TransitionAnnotation objects.
    // Cast the pointer to this vector to the void pointer to use in (void* counter_ptr).
    // To reuse this vector, you will have to cast it back to the original type.

    // Conclusion:
    // This approach is not very useful.

    CounterState() : state(), counter_ptr(nullptr) {}

    CounterState(const CounterState&) = default;
    CounterState(CounterState&&) = default;
    CounterState& operator=(const CounterState&) = default;
    CounterState& operator=(CounterState&&) = default;

    CounterState(const State& state): state{ state }, counter_ptr(nullptr) {} // NOLINT(*-explicit-constructor)
    CounterState(State&& state): state{ state }, counter_ptr(nullptr) {} // NOLINT(*-explicit-constructor)
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
    size_t id; ///< Unique ID for the counter.
    // Note: ID is the index in the counters vector.
    // TODO: Is this a good idea? Think about better solutions.
    CounterValue value; ///< Current counter value.
    CounterValue initial_value; ///< Initial counter value.

    CounterRegister() = default;
    CounterRegister(size_t id, CounterValue value) : id(id), value(value), initial_value(value) {}

    CounterRegister(const CounterRegister&) = default;
    CounterRegister(CounterRegister&&) = default;
    CounterRegister& operator=(const CounterRegister&) = default;
    CounterRegister& operator=(CounterRegister&&) = default;

    CounterRegister& operator=(CounterValue other) { value = other; return *this; }

    auto operator<=>(const CounterValue& other) const { return value <=> other; }
    auto operator<=>(const CounterRegister&) const = default;

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

class CounterRegisterSet {
private:
    std::vector<CounterRegister> counters; ///< Stores counters.

public:
    // TODO: Add the necessary constructors later.
    CounterRegisterSet() = default;

    void addCounter(CounterValue value) {
        counters.emplace_back(counters.size(), value);
    }
    // TODO: Change this to operator.
    CounterRegister& getCounter(size_t id) {
        if (id >= counters.size()) {
            throw std::runtime_error("CounterRegisterSet: Counter with this ID does not exist.");
        }
        return counters[id];
    }
    const CounterRegister& getCounter(size_t id) const {
        if (id >= counters.size()) {
            throw std::runtime_error("CounterRegisterSet: Counter with this ID does not exist.");
        }
        return counters[id];
    }
    // This implementation of getCounter is probably better.
    CounterRegister& operator[](size_t id) {
        return counters[id];
    }
    const CounterRegister& operator[](size_t id) const {
        return counters[id];
    }
    size_t size() {
        return counters.size();
    }
    // Note: Custom debug output. This should be removed later.
    void print() const {
        for (const auto& counter : counters) {
            counter.print();
        }
    }
    // TODO: Add counter removal later.
    // TODO: Add iterators later.
};

// Added for better readability.
using Target = CounterState;
using TargetSet = CounterStateSet;
using Counter = CounterRegister;
using CounterSet = CounterRegisterSet;

} // namespace mata::nfa.

#endif // TYPES_HH
