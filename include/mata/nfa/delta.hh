// Based on
/**
    delta.hh
	Core Mata Structure.
    @author Mata Group
    https://github.com/VeriFIT/mata/blob/devel/include/mata/nfa/delta.hh
*/

#ifndef DELTA_HH
#define DELTA_HH

#include <vector>

#include "mata/utils/ord-vector.hh"
#include "types.hh"

namespace mata::nfa {

// TODO: Add description.
class SymbolPost {
public:
    Symbol symbol{};
    StateSet targets{};

    SymbolPost() = default;
    explicit SymbolPost(Symbol symbol) : symbol{ symbol }, targets{} {}
    SymbolPost(Symbol symbol, State target) : symbol{ symbol }, targets{ target } {}
    SymbolPost(Symbol symbol, StateSet targets) : symbol{ symbol }, targets{ std::move(targets) } {}

    SymbolPost(SymbolPost&& rhs) noexcept : symbol{ rhs.symbol }, targets{ std::move(rhs.targets) } {}
    SymbolPost(const SymbolPost& rhs) = default;
    SymbolPost& operator=(SymbolPost&& rhs) noexcept;
    SymbolPost& operator=(const SymbolPost& rhs) = default;

    std::weak_ordering operator<=>(const SymbolPost& other) const { return symbol <=> other.symbol; }
    bool operator==(const SymbolPost& other) const { return symbol == other.symbol; }

    void insert(State state);
    void insert(const StateSet& states);
};

// TODO: Add description.
class StatePost : private utils::OrdVector<SymbolPost> {
private:
    using super = utils::OrdVector<SymbolPost>;

public:
    using super::OrdVector;
    using super::operator=;
    using super::operator==;

    StatePost(const StatePost&) = default;
    StatePost(StatePost&&) = default;
    StatePost& operator=(const StatePost&) = default;
    StatePost& operator=(StatePost&&) = default;
    bool operator==(const StatePost&) const = default;

    using super::begin;
    using super::end;
    using super::insert;
    using super::empty;
    using super::back;
    using super::find;

    iterator find(const Symbol symbol) { return super::find({ symbol, {} }); }
    const_iterator find(const Symbol symbol) const { return super::find({ symbol, {} }); }
};

// TODO: Add description.
class Delta {
private:
    std::vector<StatePost> state_posts_;

public:
    Delta(): state_posts_{} {}
    Delta(const Delta& other) = default;
    Delta(Delta&& other) = default;
    explicit Delta(size_t n): state_posts_{ n } {}

    Delta& operator=(const Delta& other) = default;
    Delta& operator=(Delta&& other) = default;

    bool operator==(const Delta& other) const;

    void add(State source, Symbol symbol, State target);
    void add(const State source, const Symbol symbol, const StateSet& targets);

    // bool contains(State source, Symbol symbol, State target) const;
    const StatePost& getStatePost(State state) const { return state_posts_[state]; }
};

} // namespace mata::nfa.

#endif // DELTA_HH
