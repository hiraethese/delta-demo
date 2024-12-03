// TODO: Create and implement the annotation.cc file using this header.

#ifndef ANNOTATION_HH
#define ANNOTATION_HH

#include <memory>

#include "types.hh"

namespace mata::nfa {

/// Class with a virtual interface for different types of annotations.
// Note: This is convenient to implement various operations during transitions.
// TODO: Is this a good idea? Think about better solutions.
class TransitionAnnotation {
public:
    virtual ~TransitionAnnotation() = default;

    virtual void execute(CounterSet& counters) const = 0;
    virtual bool test(const CounterSet& counters) { return true; }
};

/// Class for incrementing and decrementing a counter by its ID.
class CounterIncrement : public TransitionAnnotation {
private:
    size_t counter_id; ///< The ID of the counter to modify.
    int increment_value; ///< The value to increment (can be negative for decrement).

public:
    CounterIncrement() = default;
    CounterIncrement(size_t counter_id, int increment_value) : counter_id(counter_id), increment_value(increment_value) {}

    void execute(CounterSet& counters) const override {
        if (counter_id >= counters.size()) {
            throw std::runtime_error("CounterOperation: Invalid counter ID.");
        }

        CounterRegister& counter = counters[counter_id];

        // FIXME: Possibly unnecessary operations.
        if (increment_value > 0) {
            counter.increment(increment_value);
        } else if (increment_value < 0) {
            counter.decrement(-increment_value);
        }
    }
};
// TODO: Add CounterTest class.

using TransitionAnnotationPtr = std::unique_ptr<TransitionAnnotation>;
using TransitionAnnotations = std::vector<TransitionAnnotationPtr>;

// Note: Theta is used to simplify the name of the TransitionAnnotations collection.
// TODO: Try to recreate this like a class Theta (or any other Greek letter) to encapsulate the logic.
// Theta should be similar to Delta? Probably yes. Ask about this approach.
using Theta = std::vector<TransitionAnnotations>;

} // namespace mata::nfa.

#endif // ANNOTATION_HH
