//
// Created by nathan on 7/17/20.
//

#ifndef CASM_GRAMMAR_H
#define CASM_GRAMMAR_H

#include <cassert>
#include "token.h"

// A label of an arc
struct label {
    int         lb_type;
    const char *lb_str;
};

#define EMPTY 0 // Label number 0 is by definition the empty label

// A list of labels
struct labellist {
    int          ll_nlabels;
    const label *ll_label;
};

// An arc from one state to another
struct arc {
    short  a_lbl;   // label of this arc
    short  a_arrow; // State where this arc goes to
};

// A state in the DFA
struct state {
    int        s_narcs;
    const arc *s_arc;
    int        s_lower;
    int        s_upper;
    int       *s_accel;
    int        s_accept;
};

struct dfa {
    int         d_type;
    const char *name;
    int         d_nstates;
    state      *d_state;
    const char *d_first;
    [[nodiscard]] bool testBit(int ibit) const {
        return (static_cast<unsigned>(d_first[ibit / (sizeof(char) * 8)]) &
            1u << ibit % (sizeof(char) * 8)) != 0;
    }

};

struct grammar {
    int              g_ndfas;
    const dfa       *g_dfa;
    const labellist  g_ll;
    int              g_start;
    bool             g_accel;

    void addAccelerators();
    [[nodiscard]] const dfa * findDFA(int type) const {
        const dfa *d = &g_dfa[type - NT_OFFSET];
        assert(d->d_type == type);
        return d;
    }

private:
    void fixdfa(const dfa *d);
    void fixstate(state *s) const;
};

extern grammar casmGrammar;

#endif //CASM_GRAMMAR_H
