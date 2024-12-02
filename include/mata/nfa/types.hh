// Based on
// https://github.com/hiraethese/mata/blob/counter-feature/include/mata/nfa/types.hh

#ifndef TYPES_HH
#define TYPES_HH

#include <cstddef>
#include <iostream>
#include <limits>

#include "mata/utils/ord-vector.hh"

// Use this for undefined ID or index.
#define MAX_SIZE_T (std::numeric_limits<size_t>::max())
#define UNDEFINED_ID MAX_SIZE_T

namespace mata::nfa {

using Symbol = unsigned;
using State = unsigned long;
using StateSet = mata::utils::OrdVector<State>;

// State with an annotation (@c State @c state and @c size_t @c annotation_id).
// TODO: Move this to the annotation header file.
struct AnnotationState {
    State state; ///< Automaton state.
    size_t annotation_id; ///< Unique ID for the position in the vector of transition annotations.

    AnnotationState() : state(), annotation_id(UNDEFINED_ID) {}
    AnnotationState(const State state, size_t annotation_id) : state(state), annotation_id(annotation_id) {} // NOLINT(*-explicit-constructor)

    AnnotationState(const AnnotationState&) = default;
    AnnotationState(AnnotationState&&) = default;
    AnnotationState& operator=(const AnnotationState&) = default;
    AnnotationState& operator=(AnnotationState&&) = default;

    AnnotationState(const State& state): state{ state }, annotation_id(UNDEFINED_ID) {} // NOLINT(*-explicit-constructor)
    AnnotationState(State&& state): state{ state }, annotation_id(UNDEFINED_ID) {} // NOLINT(*-explicit-constructor)

    auto operator<=>(const State& other) const { return state <=> other; }
    bool operator==(const State other) const { return state == other; }
    auto operator<=>(const AnnotationState&) const = default;
    bool operator==(const AnnotationState& other) const { return state == other; }

    operator State() const { return state; } // NOLINT(*-explicit-constructor)
};

// Set of states with annotation.
// TODO: Move this to the annotation header file.
class AnnotationStateSet : public mata::utils::OrdVector<AnnotationState> {
public:
    AnnotationStateSet() = default;

    AnnotationStateSet(State state) { // NOLINT(*-explicit-constructor)
        this->push_back(AnnotationState(state));
    }
    AnnotationStateSet(StateSet& state_set) { // NOLINT(*-explicit-constructor)
        for (const State& state: state_set) {
            this->push_back(state);
        }
    }
    AnnotationStateSet(StateSet&& state_set) { // NOLINT(*-explicit-constructor)
        for (const State& state: state_set) {
            this->push_back(state);
        }
    }
    AnnotationStateSet& operator=(const StateSet& state_set) {
        for (const State& state: state_set) {
            this->push_back(state);
        }
        return *this;
    }
    AnnotationStateSet& operator=(StateSet&& state_set) {
        for (const State& state: state_set) {
            this->push_back(state);
        }
        return *this;
    }

    AnnotationStateSet(const AnnotationStateSet& counter_state_set) = default;
    AnnotationStateSet(AnnotationStateSet&& counter_state_set) noexcept = default;
    AnnotationStateSet& operator=(const AnnotationStateSet& counter_state_set) = default;
    AnnotationStateSet& operator=(AnnotationStateSet&& counter_state_set) noexcept = default;

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

// Register for counters.
// TODO: Create separate header file for counter types.
struct CounterRegister {
    size_t id; ///< Unique ID for the counter.
    // Note: ID is the index in the counters vector.
    // TODO: Is this a good idea? Think about better solutions.
    CounterValue value; ///< Current counter value.
    CounterValue initial_value; ///< Initial counter value.

    CounterRegister() : id(UNDEFINED_ID), value(0), initial_value(0) {}
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

// Set of counter registers.
// TODO: Create separate header file for counter types.
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
using Target = AnnotationState;
using TargetSet = AnnotationStateSet;
using Counter = CounterRegister;
using CounterSet = CounterRegisterSet;

} // namespace mata::nfa.

#endif // TYPES_HH
