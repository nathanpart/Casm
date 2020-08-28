//
// Created by nathan on 7/17/20.
//
#include <iostream>

#include "grammar.h"
#include "token.h"

using namespace std;

void grammar::fixdfa(const dfa *d) {
    //cout << "Processing " << d->name << ".\n";

    state *s;
    int j;
    s = d->d_state;
    for (j = 0; j < d->d_nstates; j++, s++) {
        //cout << "   state " << j + 1 << " out of " << d->d_nstates << ".\n";
        fixstate(s);
    }
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-narrowing-conversions"
#pragma ide diagnostic ignored "hicpp-signed-bitwise"
void grammar::fixstate(state *s) const {
    const arc *a;
    int k;
    int *accel;
    int nl = g_ll.ll_nlabels;
    s->s_accept = 0;
    accel = new int[nl];
    for (k = 0; k < nl; k++)
        accel[k] = -1;
    a = s->s_arc;
    for (k = s->s_narcs; --k >= 0; a++) {
        int lbl = a->a_lbl;
        const label *l = &g_ll.ll_label[lbl];
        int type = l->lb_type;
        if (a->a_arrow >= 128) {
            std::cout << "XXX to many states\n";
            continue;
        }
        if (ISNONTERMINAL(type)) {
            const dfa *d1 = findDFA(type);
            int ibit;
            if (type - NT_OFFSET >= 128) {
                std::cout << "XXX too high nonterminal number!\n";
                continue;
            }
            for (ibit = 0; ibit < g_ll.ll_nlabels; ibit++) {
                if (d1->testBit(ibit)) {
                    if (accel[ibit] != -1)
                        std::cout << "XXX ambiguity! - " << d1->name << " " << ibit << ".\n";
                    accel[ibit] = a->a_arrow | (1 << 7) | ((type - NT_OFFSET) << 8);
                }
            }
        }
        else if (lbl == EMPTY)
            s->s_accept = 1;
        else if (lbl >= 0 && lbl < nl)
            accel[lbl] = a->a_arrow;
    }
    while (nl > 0 && accel[nl-1] == -1)
        nl--;
    for (k = 0; k < nl && accel[k] == -1;)
        k++;
    if (k < nl) {
        int i;
        s->s_accel = new int[nl - k];
        s->s_lower = k;
        s->s_upper = nl;
        for (i = 0; k < nl; i++, k++)
            s->s_accel[i] = accel[k];
    }
    delete [] accel;
}
#pragma clang diagnostic pop

void grammar::addAccelerators() {
    int i;
    const dfa *d = g_dfa;
    for (i = g_ndfas; --i >= 0; d++)
        fixdfa(d);
    g_accel = true;
}