cimport mata
from libcpp.vector cimport vector
from libcpp.list cimport list as clist
from libcpp.set cimport set as cset
from cython.operator import dereference, postincrement as postinc, preincrement as preinc
from libcpp.unordered_map cimport unordered_map as umap

import shlex
import subprocess
import tabulate

cdef class Trans:
    """
    Wrapper over the transitions in NFA
    """
    cdef mata.CTrans *thisptr

    @property
    def src(self):
        """
        :return: source state of the transition
        """
        return self.thisptr.src

    @property
    def symb(self):
        """
        :return: symbol for the transition
        """
        return self.thisptr.symb

    @property
    def tgt(self):
        """
        :return: target state of the transition
        """
        return self.thisptr.tgt

    def __cinit__(self, State src=0, Symbol s=0, State tgt=0):
        """Constructor of the transition

        :param State src: source state
        :param Symbol s: symbol
        :param State tgt: target state
        """
        self.thisptr = new mata.CTrans(src, s, tgt)

    def __dealloc__(self):
        """
        Destructor
        """
        if self.thisptr != NULL:
            del self.thisptr

    cdef copy_from(self, CTrans trans):
        """Copies the internals of trans into the wrapped pointer

        :param CTrans trans: copied transition
        """
        self.thisptr.src = trans.src
        self.thisptr.symb = trans.symb
        self.thisptr.tgt = trans.tgt

    def __eq__(self, Trans other):
        return dereference(self.thisptr) == dereference(other.thisptr)

    def __neq__(self, Trans other):
        return dereference(self.thisptr) != dereference(other.thisptr)

    def __str__(self):
        return f"{self.thisptr.src}-[{self.thisptr.symb}]\u2192{self.thisptr.tgt}"

    def __repr__(self):
        return str(self)


cdef class TransSymbolStates:
    """
    Wrapper over pair of symbol and states for transitions
    """
    cdef mata.CTransSymbolStates *thisptr

    @property
    def symbol(self):
        return self.thisptr.symbol

    @symbol.setter
    def symbol(self, value):
        self.thisptr.symbol = value

    @property
    def states_to(self):
        states = []
        cdef vector[State] states_as_vector = self.thisptr.states_to.ToVector()
        return [s for s in states_as_vector]

    @states_to.setter
    def states_to(self, value):
        cdef StateSet states_to = StateSet(value)
        self.thisptr.states_to = states_to

    def __cinit__(self, Symbol symbol, vector[State] states):
        cdef StateSet states_to = StateSet(states)
        self.thisptr = new mata.CTransSymbolStates(symbol, states_to)

    def __dealloc__(self):
        if self.thisptr != NULL:
            del self.thisptr

    def __lt__(self, TransSymbolStates other):
        return dereference(self.thisptr) < dereference(other.thisptr)

    def __gt__(self, TransSymbolStates other):
        return dereference(self.thisptr) > dereference(other.thisptr)

    def __le__(self, TransSymbolStates other):
        return dereference(self.thisptr) <= dereference(other.thisptr)

    def __ge__(self, TransSymbolStates other):
        return dereference(self.thisptr) >= dereference(other.thisptr)

    def __eq__(self, TransSymbolStates other):
        return self.symbol == other.symbol and self.states_to == other.states_to

    def __neq__(self, TransSymbolStates other):
        return self.symbol != other.symbol or self.states_to != other.states_to

    def __str__(self):
        trans = "{" + ",".join(map(str, self.states_to)) + "}"
        return f"[{self.symbol}]\u2192{trans}"

    def __repr__(self):
        return str(self)

cdef class Nfa:
    """
    Wrapper over NFA
    """
    cdef mata.CNfa *thisptr
    cdef alphabet

    def __cinit__(self, state_number = 0, alphabet=None):
        """Constructor of the NFA

        :param int state_number: number of states in automaton
        :param Alpabet alphabet: alphabet corresponding to the automaton
        """
        self.thisptr = new mata.CNfa(state_number)
        self.alphabet = alphabet

    def __dealloc__(self):
        del self.thisptr

    def is_state(self, state):
        """Tests if state is in the automaton

        :param int state: tested state
        :return: true if state is in the automaton
        """
        return self.thisptr.is_state(state)

    def add_new_state(self):
        """Adds new state to automaton

        :return: number of the state
        """
        return self.thisptr.add_new_state()

    def add_initial_state(self, State st):
        """Adds initial state to automaton

        :param State st: added initial state
        """
        self.thisptr.make_initial(st)

    def add_initial_states(self, vector[State] states):
        """Adds list of initial state to automaton

        :param list states: list of initial states
        """
        self.thisptr.make_initial(states)

    def has_initial_state(self, State st):
        """Tests if automaton contains given state

        :param State st: tested state
        :return: true if automaton contains given state
        """
        return self.thisptr.has_initial(st)

    def add_final_state(self, State st):
        """Adds final state to automaton

        :param State st: added final state
        """
        self.thisptr.make_final(st)

    def has_final_state(self, State st):
        """Tests if automaton contains given state

        :param State st: tested state
        :return: true if automaton contains given state
        """
        return self.thisptr.has_final(st)

    def state_size(self):
        """Returns number of states in automaton

        :return: number of states in automaton
        """
        return self.thisptr.get_num_of_states()

    def add_trans(self, Trans tr):
        """Adds transition to automaton

        :param Trans tr: added transition
        """
        self.thisptr.add_trans(dereference(tr.thisptr))

    def add_trans_raw(self, State src, Symbol symb, State tgt):
        """Constructs transition and adds it to automaton

        :param State src: source state
        :param Symbol symb: symbol
        :param State tgt: target state
        """
        self.thisptr.add_trans(src, symb, tgt)

    def has_trans(self, Trans tr):
        """Tests if automaton contains transition

        :param Trans tr: tested transition
        :return: true if automaton contains transition
        """
        return self.thisptr.has_trans(dereference(tr.thisptr))

    def has_trans_raw(self, State src, Symbol symb, State tgt):
        """Tests if automaton contains transition

        :param State src: source state
        :param Symbol symb: symbol
        :param State tgt: target state
        :return: true if automaton contains transition
        """
        return self.thisptr.has_trans(src, symb, tgt)

    def trans_empty(self):
        """Tests if there are no transitions

        :return: true if there are no transitions in automaton
        """
        return self.thisptr.nothing_in_trans()

    def get_num_of_trans(self):
        """Returns number of transitions in automaton

        :return: number of transitions in automaton
        """
        return self.thisptr.get_num_of_trans()

    def resize(self, size):
        """Increases the size of the automaton to size

        :param int size: new size of the
        """
        self.thisptr.increase_size(size)

    def iterate(self):
        """Iterates over all transitions

        :return: stream of transitions
        """
        iterator = self.thisptr.begin()
        while iterator != self.thisptr.end():
            trans = Trans()
            lhs = dereference(iterator)
            trans.copy_from(lhs)
            preinc(iterator)
            yield trans

    def get_transitions_from_state(self, State state):
        """Returns list of TransSymbolStates for the given state

        :param State state: state for which we are getting the transitions
        :return: TransSymbolStates
        """
        cdef TransitionList transitions = self.thisptr.get_transitions_from_state(state)
        cdef vector[mata.CTransSymbolStates] transitions_list = transitions.ToVector()

        cdef vector[mata.CTransSymbolStates].iterator it = transitions_list.begin()
        cdef vector[mata.CTransSymbolStates].iterator end = transitions_list.end()
        transsymbols = []
        while it != end:
            t = TransSymbolStates(
                dereference(it).symbol,
                dereference(it).states_to.ToVector()
            )
            postinc(it)
            transsymbols.append(t)
        return transsymbols



    def __str__(self):
        """String representation of the automaton displays states, and transitions

        :return: string representation of the automaton
        """
        cdef vector[State] initial_states = self.thisptr.initialstates.ToVector()
        cdef vector[State] final_states = self.thisptr.finalstates.ToVector()
        result = "initial_states: {}\n".format([s for s in initial_states])
        result += "final_states: {}\n".format([s for s in final_states])
        result += "transitions:\n"
        for trans in self.iterate():
            symbol = trans.symb if self.alphabet is None \
                else self.alphabet.reverse_translate_symbol(trans.symb)
            result += f"{trans.src}-[{symbol}]\u2192{trans.tgt}\n"
        return result

    def to_dot_file(self, output_file='aut.dot', output_format='pdf'):
        """Transforms the automaton to dot format.

        By default the result is saved to `aut.dot`, and further to `aut.dot.pdf`.

        One can choose other format that is supported by graphviz format (e.g. pdf or png).

        :param str output_file: name of the output file where the automaton will be stored
        :param str output_format: format of the output file (pdf/png/etc)
        """
        cdef mata.ofstream* output
        output = new mata.ofstream(output_file.encode('utf-8'))
        try:
            self.thisptr.print_to_DOT(dereference(output))
        finally:
            del output

        graphviz_command = f"dot -O -T{output_format} {output_file}"
        _, err = run_safely_external_command(graphviz_command)
        if err:
            print(f"error while dot file: {err}")

    def to_dot_str(self, encoding='utf-8'):
        """Transforms the automaton to dot format string

        :param str encoding: encoding of the dot string
        :return: string with dot representation of the automaton
        """
        cdef mata.stringstream* output_stream
        output_stream = new mata.stringstream("")
        cdef string result
        try:
            self.thisptr.print_to_DOT(dereference(output_stream))
            result = output_stream.str()
        finally:
            del output_stream
        return result.decode(encoding)

    def post_map_of(self, State st, Alphabet alphabet):
        """Returns mapping of symbols to set of states.

        :param State st: source state
        :param Alphabet alphabet: alphabet of the post
        :return: dictionary mapping symbols to set of reachable states from the symbol
        """
        symbol_map = {}
        for symbol in alphabet.get_symbols():
            symbol_map[symbol] = self.post_of({st}, symbol)
        return symbol_map

    def post_of(self, vector[State] states, Symbol symbol):
        """Returns sets of reachable states from set of states through a symbol

        :param StateSet states: set of states
        :param Symbol symbol: source symbol
        :return: set of reachable states
        """
        cdef vector[State] return_value
        cdef StateSet input_states = StateSet(states)
        return_value = self.thisptr.post(input_states, symbol).ToVector()
        return {v for v in return_value}

    def get_shortest_words(self):
        """Returns set of shortest words accepted by automaton

        :return: set of shortest words accepted by automaton
        """
        cdef WordSet shortest
        shortest = self.thisptr.get_shortest_words()
        result = []
        cdef cset[vector[Symbol]].iterator it = shortest.begin()
        cdef cset[vector[Symbol]].iterator end = shortest.end()
        while it != end:
            short = dereference(it)
            result.append(short)
            postinc(it)
        return result

    # External Constructors
    @classmethod
    def from_regex(cls, regex, encoding='utf-8'):
        """Creates automaton from the regular expression

        The format of the regex conforms to google RE2 regular expression library.

        :param str regex: regular expression
        :param str encoding: encoding of the string
        :return: Nfa automaton
        """
        result = Nfa()
        mata.create_nfa(result.thisptr, regex.encode(encoding))
        return result

    # Operations
    @classmethod
    def determinize(cls, Nfa lhs):
        """Determinize the lhs automaton

        :param Nfa lhs: non-deterministic finite automaton
        :return: deterministic finite automaton, subset map
        """
        result = Nfa()
        cdef SubsetMap subset_map
        mata.determinize(result.thisptr, dereference(lhs.thisptr), &subset_map)
        return result, subset_map_to_dictionary(subset_map)

    @classmethod
    def union(cls, Nfa lhs, Nfa rhs):
        """Performs union of lhs and rhs

        :param Nfa lhs: first automaton
        :param Nfa rhs: second automaton
        :return: union of lhs and rhs
        """
        result = Nfa()
        mata.uni(
            result.thisptr, dereference(lhs.thisptr), dereference(rhs.thisptr)
        )
        return result

    @classmethod
    def intersection(cls, Nfa lhs, Nfa rhs):
        """Performs intersection of lhs and rhs

        :param Nfa lhs: first automaton
        :param Nfa rhs: second automaton
        :return: intersection of lhs and rhs, product map of the results
        """
        result = Nfa()
        cdef ProductMap product_map
        mata.intersection(
            result.thisptr, dereference(lhs.thisptr), dereference(rhs.thisptr), &product_map
        )
        return result, {tuple(sorted(k)): v for k, v in product_map}

    @classmethod
    def complement(cls, Nfa lhs, Alphabet alphabet, params = None):
        """Performs complement of lhs

        :param Nfa lhs: complemented automaton
        :param OnTheFlyAlphabet alphabet: alphabet of the lhs
        :param dict params: additional params
        :return: complemented automaton, map of subsets to states
        """
        result = Nfa()
        params = params or {'algo': 'classical'}
        cdef SubsetMap subset_map
        mata.complement(
            result.thisptr,
            dereference(lhs.thisptr),
            <CAlphabet&>dereference(alphabet.as_base()),
            {
                k.encode('utf-8'): v.encode('utf-8') if isinstance(v, str) else v
                for k, v in params.items()
            },
            &subset_map
        )
        return result, subset_map_to_dictionary(subset_map)

    @classmethod
    def make_complete(cls, Nfa lhs, State sink_state, Alphabet alphabet):
        """Makes lhs complete

        :param Nfa lhs: automaton that will be made complete
        :param Symbol sink_state: sink state of the automaton
        :param OnTheFlyAlphabet alphabet: alphabet of the
        """
        if not lhs.thisptr.is_state(sink_state):
            lhs.thisptr.increase_size(lhs.state_size() + 1)
        mata.make_complete(lhs.thisptr, <CAlphabet&>dereference(alphabet.as_base()), sink_state)

    @classmethod
    def revert(cls, Nfa lhs):
        """Reverses transitions in the lhs

        :param Nfa lhs: source automaton
        :return: automaton with reversed transitions
        """
        result = Nfa()
        mata.revert(result.thisptr, dereference(lhs.thisptr))
        return result

    @classmethod
    def remove_epsilon(cls, Nfa lhs, Symbol epsilon):
        """Removes transitions that contains epsilon symbol

        TODO: Possibly there may be issue with setting the size of the automaton beforehand?

        :param Nfa lhs: automaton, where epsilon transitions will be removed
        :param Symbol epsilon: symbol representing the epsilon
        :return: automaton, with epsilon transitions removed
        """
        result = Nfa(lhs.state_size())
        mata.remove_epsilon(
            result.thisptr, dereference(lhs.thisptr), epsilon
        )
        return result

    @classmethod
    def minimize(cls, Nfa lhs):
        """Minimies the automaton lhs

        :param Nfa lhs: automaton to be minimized
        :return: minimized automaton
        """
        result = Nfa()
        mata.minimize(result.thisptr, dereference(lhs.thisptr))
        return result


    @classmethod
    def compute_relation(cls, Nfa lhs, params = None):
        """Computes the relation for the automaton

        :param Nfa lhs: automaton
        :param Dict params: parameters of the computed relation
        :return: computd relation
        """
        params = params or {'relation': 'simulation', 'direction': 'forward'}
        cdef mata.CBinaryRelation relation = mata.compute_relation(
            dereference(lhs.thisptr),
            {
                k.encode('utf-8'): v.encode('utf-8') if isinstance(v, str) else v
                for k, v in params.items()
            }
        )
        result = BinaryRelation()
        cdef size_t relation_size = relation.size()
        result.resize(relation_size)
        for i in range(0, relation_size):
            for j in range(0, relation_size):
                val = relation.get(i, j)
                result.set(i, j, val)
        return result

    # Tests
    @classmethod
    def is_deterministic(cls, Nfa lhs):
        """Tests if the lhs is determinstic

        :param Nfa lhs: non-determinstic finite automaton
        :return: true if the lhs is deterministic
        """
        return mata.is_deterministic(dereference(lhs.thisptr))

    @classmethod
    def is_lang_empty_path_counterexample(cls, Nfa lhs):
        """Checks if language of automaton lhs is empty, if not, returns path of states as counter
        example.

        :param Nfa lhs:
        :return: true if the lhs is empty, counter example if lhs is not empty
        """
        cdef Path path
        result = mata.is_lang_empty(dereference(lhs.thisptr), &path)
        return result, path


    @classmethod
    def is_lang_empty_word_counterexample(cls, Nfa lhs):
        """Checks if language of automaton lhs is empty, if not, returns word as counter example.

        :param Nfa lhs:
        :return: true if the lhs is empty, counter example if lhs is not empty
        """
        cdef Word word
        result = mata.is_lang_empty_cex(dereference(lhs.thisptr), &word)
        return result, word

    @classmethod
    def is_universal(cls, Nfa lhs, Alphabet alphabet, params = None):
        """Tests if lhs is universal wrt given alphabet

        :param Nfa lhs: automaton tested for universality
        :param OnTheFlyAlphabet alphabet: on the fly alphabet
        :param dict params: additional params to the function, currently supports key 'algo',
            which determines used universality test
        :return: true if lhs is universal
        """
        params = params or {'algo': 'antichains'}
        return mata.is_universal(
            dereference(lhs.thisptr),
            <CAlphabet&>dereference(alphabet.as_base()),
            {
                k.encode('utf-8'): v.encode('utf-8') if isinstance(v, str) else v
                for k, v in params.items()
            }
        )

    @classmethod
    def is_included(
            cls, Nfa lhs, Nfa rhs, Alphabet alphabet, params = None
    ):
        """Test inclusion between two automata

        :param Nfa lhs: smaller automaton
        :param Nfa rhs: bigger automaton
        :param Alphabet alphabet: alpabet shared by two automata
        :param dict params: adtitional params
        :return: true if lhs is included by rhs, counter example word if not
        """
        cdef Word word
        params = params or {'algo': 'antichains'}
        result = mata.is_incl(
            dereference(lhs.thisptr),
            dereference(rhs.thisptr),
            <CAlphabet&>dereference(alphabet.as_base()),
            &word,
            {
                k.encode('utf-8'): v.encode('utf-8') if isinstance(v, str) else v
                for k, v in params.items()
            }
        )
        return result, word

    @classmethod
    def is_complete(cls, Nfa lhs, Alphabet alphabet):
        """Test if automaton is complete

        :param Nf lhs: tested automaton
        :param OnTheFlyAlphabet alphabet: alphabet of the automaton
        :return: true if the automaton is complete
        """
        return mata.is_complete(
            dereference(lhs.thisptr),
            <CAlphabet&>dereference(alphabet.as_base())
        )

    @classmethod
    def is_in_lang(cls, Nfa lhs, vector[Symbol] word):
        """Tests if word is in language

        :param Nfa lhs: tested automaton
        :param vector[Symbol] word: tested word
        :return: true if word is in language of automaton lhs
        """
        return mata.is_in_lang(dereference(lhs.thisptr), <Word> word)

    @classmethod
    def is_prefix_in_lang(cls, Nfa lhs, vector[Symbol] word):
        """Test if any prefix of the word is in the language

        :param Nfa lhs: tested automaton
        :param vector[Symbol] word: tested word
        :return: true if any prefix of word is in language of automaton lhs
        """
        return mata.is_prfx_in_lang(dereference(lhs.thisptr), <Word> word)

    @classmethod
    def accepts_epsilon(cls, Nfa lhs):
        """Tests if automaton accepts epsilon

        :param Nfa lhs: tested automaton
        :return: true if automaton accepts epsilon
        """
        cdef vector[State] initial_states = lhs.thisptr.initialstates.ToVector()
        for state in initial_states:
            if lhs.has_final_state(state):
                return True
        return False

    # Helper functions
    @classmethod
    def get_forward_reachable_states(cls, Nfa lhs, Alphabet alphabet):
        """Returns list of reachable states from initial states

        WARNING: This is quite inefficient operation, that could be implemented better

        >>> mata.Nfa.get_forward_reachable_states(lhs)
        {0, 1, 2}

        :param Nfa lhs: source automaton
        :return: set of reachable states
        """
        cdef vector[State] initial_states = lhs.thisptr.initialstates.ToVector()
        reachable = {state for state in initial_states}
        worklist = reachable
        while worklist:
            state = worklist.pop()
            for post in lhs.post_map_of(state, alphabet).values():
                worklist.update({s for s in post if s not in reachable})
            reachable = reachable.union(worklist)
        return reachable


    @classmethod
    def get_word_for_path(cls, Nfa lhs, path):
        """For a given path (set of states) returns a corresponding word

        >>> mata.Nfa.get_word_for_path(lhs, [0, 1, 2])
        ([1, 1], True)

        :param Nfa lhs: source automaton
        :param list path: list of states
        :return: pair of word (list of symbols) and true or false, whether the search was successful
        """
        return mata.get_word_for_path(dereference(lhs.thisptr), path)

    @classmethod
    def encode_word(cls, string_to_symbol, word):
        """Encodes word based on a string to symbol map

        >>> mata.Nfa.encode_word({'a': 1, 'b': 2, "c": 0}, "abca")
        [1, 2, 0, 1]

        :param dict string_to_symbol: dictionary of strings to integers
        :param word: list of strings representing a encoded word
        :return:
        """
        return mata.encode_word(
            {k.encode('utf-8'): v for (k, v) in string_to_symbol.items()},
            [s.encode('utf-8') for s in word]
        )

cdef class Alphabet:
    """
    Base class for alphabets
    """
    def __cinit__(self):
        pass

    def __dealloc__(self):
        pass

    def translate_symbol(self, str symbol):
        pass

    def reverse_translate_symbol(self, Symbol symbol):
        pass

    cdef get_symbols(self):
        pass

    cdef mata.CAlphabet* as_base(self):
        pass

cdef class CharAlphabet(Alphabet):
    """
    CharAlphabet translates characters in quotes, such as 'a' or "b" to their ordinal values.
    """
    cdef mata.CCharAlphabet *thisptr

    def __cinit__(self):
        self.thisptr = new mata.CCharAlphabet()

    def __dealloc__(self):
        del self.thisptr

    def translate_symbol(self, str symbol):
        """Translates character to its ordinal value. If the character is not in quotes,
        it is interpreted as 0 byte

        :param str symbol: translated symbol
        :return: ordinal value of the symbol
        """
        return self.thisptr.translate_symb(symbol.encode('utf-8'))

    def reverse_translate_symbol(self, Symbol symbol):
        """Translates the ordinal value back to the character

        :param Symbol symbol: integer symbol
        :return: symbol as a character
        """
        return "'" + chr(symbol) + "'"

    cpdef get_symbols(self):
        """Returns list of supported symbols
        
        :return: list of symbols
        """
        cdef clist[Symbol] symbols = self.thisptr.get_symbols()
        return [s for s in symbols]

    cdef mata.CAlphabet* as_base(self):
        """Retypes the alphabet to its base class
        
        :return: alphabet as CAlphabet*
        """
        return <mata.CAlphabet*> self.thisptr


cdef class EnumAlphabet(Alphabet):
    """
    EnumAlphabet represents alphabet that has fixed number of possible values
    """

    cdef mata.CEnumAlphabet *thisptr
    cdef vector[string] enums_as_strings

    def __cinit__(self, enums):
        self.enums_as_strings = [e.encode('utf-8') for e in enums]
        self.thisptr = new mata.CEnumAlphabet(
            self.enums_as_strings.begin(), self.enums_as_strings.end()
        )

    def __dealloc__(self):
        del self.thisptr

    def translate_symbol(self, str symbol):
        """Translates the symbol ot its position in the enumeration

        :param str symbol: translated symbol
        :return: symbol as an position in the enumeration
        """
        return self.thisptr.translate_symb(symbol.encode('utf-8'))

    def reverse_translate_symbol(self, Symbol symbol):
        """Translates the symbol back to its string representation

        :param Symbol symbol: integer symbol (position in enumeration)
        :return: symbol as the original string
        """
        if symbol < len(self.enums_as_strings):
            raise IndexError(f"{symbol} is out of range of enumeration")
        return self.enums_as_strings[symbol]

    cpdef get_symbols(self):
        """Returns list of supported symbols
        
        :return: list of supported symbols
        """
        cdef clist[Symbol] symbols = self.thisptr.get_symbols()
        return [s for s in symbols]

    cdef mata.CAlphabet* as_base(self):
        """Retypes the alphabet to its base class
        
        :return: alphabet as CAlphabet*
        """
        return <mata.CAlphabet*> self.thisptr


cdef class OnTheFlyAlphabet(Alphabet):
    """
    OnTheFlyAlphabet represents alphabet that is not known before hand and is constructed on-the-fly
    """
    cdef mata.COnTheFlyAlphabet *thisptr
    cdef StringToSymbolMap string_to_symbol_map

    def __cinit__(self, State initial_symbol = 0):
        self.thisptr = new mata.COnTheFlyAlphabet(&self.string_to_symbol_map, initial_symbol)

    def __dealloc__(self):
        del self.thisptr

    def translate_symbol(self, str symbol):
        """Translates symbol to the position of the seen values

        :param str symbol: translated symbol
        :return: order of the symbol as was seen during the construction
        """
        return self.thisptr.translate_symb(symbol.encode('utf-8'))

    def reverse_translate_symbol(self, Symbol symbol):
        """Translates symbol back to its string representation

        :param Symbol symbol: integer symbol
        :return: original string
        """
        cdef umap[string, Symbol].iterator it = self.string_to_symbol_map.begin()
        cdef umap[string, Symbol].iterator end = self.string_to_symbol_map.end()
        while it != end:
            key = dereference(it).first
            value = dereference(it).second
            if value == symbol:
                return key
            postinc(it)
        raise IndexError(f"{symbol} is out of range of enumeration")


    cpdef get_symbols(self):
        """Returns list of supported symbols
        
        :return: list of supported symbols
        """
        cdef clist[Symbol] symbols = self.thisptr.get_symbols()
        return [s for s in symbols]

    cdef mata.CAlphabet* as_base(self):
        """Retypes the alphabet to its base class
        
        :return: alphabet as CAlphabet*
        """
        return <mata.CAlphabet*> self.thisptr

cdef class BinaryRelation:
    """
    Wrapper for binary relation
    """
    cdef mata.CBinaryRelation *thisptr

    def __cinit__(self, size_t size=0, bool defVal=False, size_t rowSize=16):
        self.thisptr = new mata.CBinaryRelation(size, defVal, rowSize)

    def __dealloc__(self):
        if self.thisptr != NULL:
            del self.thisptr

    def size(self):
        """Returns the size of the relation

        :return: size of the relation
        """
        return self.thisptr.size()

    def resize(self, size_t size, bool defValue=False):
        """Resizes the binary relation to size

        :param size_t size: new size of the binary relation
        :param bool defValue: default value that is set after resize
        """
        self.thisptr.resize(size, defValue)

    def get(self, size_t row, size_t col):
        """Gets the value of the relation at [row, col]

        :param size_t row: row of the relation
        :param size_t col: col of the relation
        :return: value of the binary relation at [row, col]
        """
        return self.thisptr.get(row, col)

    def set(self, size_t row, size_t col, bool value):
        """Sets the value of the relation at [row, col]

        :param size_t row: row of the relation
        :param size_t col: col of the relation
        :param bool value: value that is set
        """
        self.thisptr.set(row, col, value)

    def to_matrix(self):
        """Converts the relation to list of lists of booleans

        :return: relation of list of lists to booleans
        """
        size = self.size()
        result = []
        for i in range(0, size):
            sub_result = []
            for j in range(0, size):
                sub_result.append(self.get(i, j))
            result.append(sub_result)
        return result

    def reset(self, bool defValue = False):
        """Resets the relation to defValue

        :param bool defValue: value to which the relation will be reset
        """
        self.thisptr.reset(defValue)

    def split(self, size_t at, bool reflexive=True):
        """Creates new row corresponding to the row/col at given index (i think)

        :param size_t at: where the splitting will commence
        :param bool reflexive: whether the relation should stay reflexive
        """
        self.thisptr.split(at, reflexive)

    def alloc(self):
        """Increases the size of the relation by one

        :return: previsous size of the relation
        """
        return self.thisptr.alloc()

    def is_symmetric_at(self, size_t row, size_t col):
        """Checks if the relation is symmetric at [row, col] and [col, row]

        :param size_t row: checked row
        :param size_t col: checked col
        :return: true if [row, col] and [col, row] are symmetric
        """
        return self.thisptr.sym(row, col)

    def restrict_to_symmetric(self):
        """Restricts the relation to its symmetric fragment"""
        self.thisptr.restrict_to_symmetric()

    def build_equivalence_classes(self):
        """Builds equivalence classes w.r.t relation

        :return: mapping of state to its equivalence class,
            first states corresponding to a equivalence class?
        """
        cdef vector[size_t] index
        cdef vector[size_t] head
        self.thisptr.build_equivalence_classes(index, head)
        return index, head

    def build_index(self):
        """Builds index mapping states to their images

        :return: index mapping states to their images, i.e. x -> {y | xRy}
        """
        cdef vector[vector[size_t]] index
        self.thisptr.build_index(index)
        return [[v for v in i] for i in index]

    def build_inverse_index(self):
        """Builds index mapping states to their co-images

        :return: index mapping states to their co-images, i.e. x -> {y | yRx}
        """
        cdef vector[vector[size_t]] index
        self.thisptr.build_inv_index(index)
        return [[v for v in i] for i in index]

    def build_indexes(self):
        """Builds index mapping states to their images/co-images

        :return: index mapping states to their images/co-images, i.e. x -> {y | yRx}
        """
        cdef vector[vector[size_t]] index
        cdef vector[vector[size_t]] inv_index
        self.thisptr.build_index(index, inv_index)
        return [[v for v in i] for i in index], [[v for v in i] for i in inv_index]

    def transpose(self):
        """Transpose the relation

        :return: transposed relation
        """
        result = BinaryRelation()
        self.thisptr.transposed(dereference(result.thisptr))
        return result

    def get_quotient_projection(self):
        """Gets quotient projection of the relation

        :return: quotient projection
        """
        cdef vector[size_t] projection
        self.thisptr.get_quotient_projection(projection)
        return projection

    def __str__(self):
        return str(tabulate.tabulate(self.to_matrix()))


cdef subset_map_to_dictionary(SubsetMap subset_map):
    """Helper function that translates the unordered map to dictionary

    :param SubsetMap subset_map: map of state sets to states
    :return: subset_map as dictionary
    """
    result = {}
    cdef umap[StateSet, State].iterator it = subset_map.begin()
    while it != subset_map.end():
        key = dereference(it).first.ToVector()
        value = dereference(it).second
        result[tuple(sorted(key))] = value
        postinc(it)
    return result


# Temporary for testing
def divisible_by(k: int):
    """
    Constructs automaton accepting strings containing ones divisible by "k"
    """
    assert k > 1
    lhs = Nfa(k+1)
    lhs.add_initial_state(0)
    lhs.add_trans_raw(0, 0, 0)
    for i in range(1, k + 1):
        lhs.add_trans_raw(i - 1, 1, i)
        lhs.add_trans_raw(i, 0, i)
    lhs.add_trans_raw(k, 1, 1)
    lhs.add_final_state(k)
    return lhs


def run_safely_external_command(cmd: str, check_results=True, quiet=True, timeout=None, **kwargs):
    """Safely runs the piped command, without executing of the shell

    Courtesy of: https://blog.avinetworks.com/tech/python-best-practices

    :param str cmd: string with command that we are executing
    :param bool check_results: check correct command exit code and raise exception in case of fail
    :param bool quiet: if set to False, then it will print the output of the command
    :param int timeout: timeout of the command
    :param dict kwargs: additional args to subprocess call
    :return: returned standard output and error
    :raises subprocess.CalledProcessError: when any of the piped commands fails
    """
    # Split
    unpiped_commands = list(map(str.strip, cmd.split(" | ")))
    cmd_no = len(unpiped_commands)

    # Run the command through pipes
    objects: List[subprocess.Popen] = []
    for i in range(cmd_no):
        executed_command = shlex.split(unpiped_commands[i])

        # set streams
        stdin = None if i == 0 else objects[i-1].stdout
        stderr = subprocess.STDOUT if i < (cmd_no - 1) else subprocess.PIPE

        # run the piped command and close the previous one
        piped_command = subprocess.Popen(
            executed_command,
            shell=False, stdin=stdin, stdout=subprocess.PIPE, stderr=stderr, **kwargs
        )
        if i != 0:
            objects[i-1].stdout.close()  # type: ignore
        objects.append(piped_command)

    try:
        # communicate with the last piped object
        cmdout, cmderr = objects[-1].communicate(timeout=timeout)

        for i in range(len(objects) - 1):
            objects[i].wait(timeout=timeout)

    except subprocess.TimeoutExpired:
        for process in objects:
            process.terminate()
        raise

    # collect the return codes
    if check_results:
        for i in range(cmd_no):
            if objects[i].returncode:
                if not quiet and (cmdout or cmderr):
                    print(f"captured stdout: {cmdout.decode('utf-8')}", 'red')
                    print(f"captured stderr: {cmderr.decode('utf-8')}", 'red')
                raise subprocess.CalledProcessError(
                    objects[i].returncode, unpiped_commands[i]
                )

    return cmdout, cmderr