#include <iostream>

#include "../include/mata/nfa/delta.hh"
#include "../include/mata/nfa//nfa.hh"

using namespace mata::nfa;
using namespace mata::utils;

// Use this as an example.
int main() {
    // Create Delta.
    Delta delta(4);
    delta.add(0, 'a', {0, 1});
    delta.add(1, 'b', 2);
    delta.add(2, 0, 3);
    delta.add(3, 'c', 3);

    // Create initial states.
    SparseSet<State> initial = {0};

    // Create final states.
    SparseSet<State> final = {3};

    // Create counters.
    CounterSet counters;
    counters.addCounter(0);
    counters.addCounter(2);
    counters.addCounter(4);
    counters.print();

    // TODO: Create Theta.

    // Create NFA.
    Nfa nfa(delta, initial, final, counters);

    // Test inputs for NFA.
    std::string testInputs[] = {"ab", "abc", "abccc", "a", "ac"};

    // Simulate the NFA.
    for (const auto& input : testInputs) {
        std::cout << "Testing input: \"" << input << "\"\n";
        if ( nfa.simulate(input) ) {
            std::cout << "Result: Accepted!\n";
        } else {
            std::cout << "Result: Rejected.\n";
        }
    }

    // End of simulation.
    return 0;
}
