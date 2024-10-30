// TODO: Insert file header.

#ifndef MATA_TYPES_HH
#define MATA_TYPES_HH

#include "mata/alphabet.hh"
#include "mata/parser/parser.hh"

#include <limits>

namespace mata::nfa {

extern const std::string TYPE_NFA;

using State = unsigned long;
using StateSet = mata::utils::OrdVector<State>;

/// State with counter (State state and void* counter_ptr).
struct CounterState {
    State state; ///< Automaton state.
    void* counter_ptr; ///< Pointer to the counter table when transitioning to a state.

    // Constructor to allow implicit conversion from state.
    CounterState(State state, void* counter_ptr = nullptr)
        : state(state), counter_ptr(counter_ptr) {}

    // Conversion operator to allow implicit conversion to state.
    operator State() const { return state; }

    // Comparison operators to allow comparison with state.
    bool operator==(State other) const { return state == other; }
    bool operator!=(State other) const { return state != other; }
    bool operator<(State other) const { return state < other; }
    bool operator<=(State other) const { return state <= other; }
    bool operator>(State other) const { return state > other; }
    bool operator>=(State other) const { return state >= other; }

    bool operator==(const CounterState& other) const {
        return state == other.state && counter_ptr == other.counter_ptr;
    }

    bool operator!=(const CounterState& other) const {
        return state != other.state || counter_ptr != other.counter_ptr;
    }
};

// TODO: Conversion between CounterStateSet and StateSet.
using CounterStateSet = mata::utils::OrdVector<CounterState>;

struct Run {
    Word word{}; ///< A finite-length word.
    std::vector<State> path{}; ///< A finite-length path through automaton.
};

using StateRenaming = std::unordered_map<State, State>;

/**
 * @brief Map of additional parameter name and value pairs.
 *
 * Used by certain functions for specifying some additional parameters in the following format:
 * ```cpp
 * ParameterMap {
 *     { "algorithm", "classical" },
 *     { "minimize", "true" }
 * }
 * ```
 */
using ParameterMap = std::unordered_map<std::string, std::string>;

struct Limits {
public:
    static const State min_state = std::numeric_limits<State>::min();
    static const State max_state = std::numeric_limits<State>::max();
    static const Symbol min_symbol = std::numeric_limits<Symbol>::min();
    static const Symbol max_symbol = std::numeric_limits<Symbol>::max();
};

struct Nfa; ///< A non-deterministic finite automaton.

/// An epsilon symbol which is now defined as the maximal value of data type used for symbols.
constexpr Symbol EPSILON = Limits::max_symbol;

} // namespace mata::nfa.

#endif //MATA_TYPES_HH
