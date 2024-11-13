// TODO: Insert file header.

#ifndef MATA_TYPES_HH
#define MATA_TYPES_HH

// #include "mata/alphabet.hh"
// #include "mata/parser/parser.hh"

// Note: Temporary local includes for convenience.
#include "../alphabet.hh"
#include "../parser/parser.hh"

#include <limits>

namespace mata::nfa {

extern const std::string TYPE_NFA;

using State = unsigned long;
using StateSet = mata::utils::OrdVector<State>;

/// State with counter (State state and void* counter_ptr).
struct CounterState {
    State state; ///< Automaton state.
    void* counter_ptr; ///< Pointer to the counter table when transitioning to a state.

    CounterState() : state(), counter_ptr(nullptr) {}
    CounterState(const CounterState&) = default;
    CounterState(CounterState&&) = default;
    CounterState &operator=(const CounterState&) = default;
    CounterState &operator=(CounterState&&) = default;
    CounterState(State state, void* counter_ptr = nullptr) : state(state), counter_ptr(counter_ptr) {}

    auto operator<=>(const CounterState&) const = default;

    bool operator==(State other) const { return state == other; }
    bool operator!=(State other) const { return state != other; }
    bool operator<(State other) const { return state < other; }
    bool operator<=(State other) const { return state <= other; }
    bool operator>(State other) const { return state > other; }
    bool operator>=(State other) const { return state >= other; }

    operator State() const { return state; }
};

/// TODO: Can be very slow! Change this later.
class CounterStateSet : public mata::utils::OrdVector<CounterState> {
public:
    CounterStateSet() = default;

    explicit CounterStateSet(State state) {
        this->push_back(CounterState(state));
    }

    explicit CounterStateSet(const StateSet& state_set) {
        for (const State& state : state_set) {
            this->push_back(state);
        }
    }

    operator StateSet() const {
        StateSet state_set;
        for (const auto& target : *this) {
            state_set.push_back(target.state);
        }
        return state_set;
    }
};

// Note: Added for better readability.
using Target = CounterState;
// Note: Added for better readability.
using TargetSet = CounterStateSet;

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

// Hash specialization for CounterState.
namespace std {
    template<>
    struct hash<mata::nfa::CounterState> {
        size_t operator()(const mata::nfa::CounterState& cs) const noexcept {
            // Note: Hash the State (unsigned long).
            return std::hash<mata::nfa::State>{}(cs.state);
        }
    };
} // namespace std.

#endif //MATA_TYPES_HH
