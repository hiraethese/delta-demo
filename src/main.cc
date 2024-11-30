#include <iostream>

#include "../include/mata/nfa/delta.hh"
#include "../include/mata/nfa//nfa.hh"

using namespace mata::nfa;
using namespace mata::utils;

int main() {
    Delta delta(4);

    delta.add(0, 'a', {0, 1});
    delta.add(1, 'b', 2);
    delta.add(2, 0, 3);
    delta.add(3, 'c', 3);

    SparseSet<State> initial = {0};
    SparseSet<State> final = {3};

    CounterSet counters;
    counters.push_back(Counter(0, 0));

    Nfa nfa(delta, initial, final, counters);

    std::string testInputs[] = {"ab", "abc", "abccc", "a", "ac"};

    for (const auto& input : testInputs) {
        std::cout << "Testing input: \"" << input << "\"\n";
        if ( nfa.simulate(input) ) {
            std::cout << "Result: Accepted!\n";
        } else {
            std::cout << "Result: Rejected.\n";
        }
    }

    return 0;
}
