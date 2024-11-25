#ifndef TYPES_HH
#define TYPES_HH

#include "mata/utils/ord-vector.hh"

namespace mata::nfa {

using Symbol = unsigned;
using State = unsigned long;
using StateSet = mata::utils::OrdVector<State>;

} // namespace mata::nfa.

#endif // TYPES_HH
