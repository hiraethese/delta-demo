#include <iostream>
#include <unordered_set>
#include "../include/delta.hh"
#include "../include/nfa.hh"

int main() {
    Delta delta;

    delta.addTransition(0, 'a', {0, 1});
    delta.addTransition(1, 'b', {2});
    delta.addTransition(2, 0, {3});
    delta.addTransition(3, 'c', {3});

    std::unordered_set<State> initial = {0};
    std::unordered_set<State> final = {3};

    Nfa nfa(delta, initial, final);

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
