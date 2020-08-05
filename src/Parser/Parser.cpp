#pragma clang diagnostic push
#pragma ide diagnostic ignored "hicpp-signed-bitwise"
//
// Created by nathan on 7/18/20.
//

#include <cassert>
#include <cstring>
#include <utility>

#include "Parser.h"
#include "errcode.h"

using namespace std;

// Additional stack operations
int ParserStack::shift(short type, const string& str, int newState, Location loc) {
    assert(!empty());
    int err = top().i_parent.addChild(type, str, std::move(loc));
    if (err)
        return err;
    top().i_state = newState;
    return 0;
}

int ParserStack::ppush(short type, const dfa *d, int newState, Location loc) {
    assert(!empty());
    node &n = top().i_parent;
    int err = n.addChild(type, "", std::move(loc));
    if (err) {
        return err;
    }
    top().i_state = newState;
    push(StackEntry(d, n.get_child(n.num_children - 1)));
    return 0;
}


// Parser proper
Parser::Parser() : i_grammar(casmGrammar), i_tree(std::make_shared<node>(i_grammar.g_start)) {
    if (!i_grammar.g_accel) {
        i_grammar.addAccelerators();
    }

    i_stack.push(StackEntry(i_grammar.findDFA(i_grammar.g_start), *i_tree));
}

int Parser::addToken(short type, const string& str, const Location& loc) {
    int iLabel;
    int err;

    iLabel = classify(type, str.c_str());
    if (iLabel < 0)
        return E_SYNTAX;

    // Loop until the token is shifted or an error occurred
    for (;;) {
        // Fetch the current dfa and state
        const dfa *d = i_stack.top().i_dfa;
        state *s = &d->d_state[i_stack.top().i_state];

        // check accelerator
        if (s->s_lower <= iLabel && iLabel < s->s_upper) {
            int x = s->s_accel[iLabel - s->s_lower];
            if (x != -1) {
                if (x & (1 << 7)) {
                    // Push nonterminal
                    auto nt = static_cast<short>((x >> 8) + NT_OFFSET);
                    int arrow = x & ((1 << 7) - 1);
                    const dfa *d1 = i_grammar.findDFA(nt);
                    if ((err = i_stack.ppush(nt, d1, arrow, loc)) > 0) {
                        return err;
                    }
                    continue;
                }

                // Shift the token
                if ((err = i_stack.shift(type, str, x, loc)) > 0) {
                    return err;
                }

                // Pop while we are in an accept-only state
                while (s = &d->d_state[i_stack.top().i_state], s->s_accept && s->s_narcs == 1) {
                     i_stack.pop();
                     if (i_stack.empty()) {
                         return E_DONE;
                     }
                     d = i_stack.top().i_dfa;
                }
                return E_OK;
            }
        }

        if (s->s_accept) {
            // Pop this dfa and try again
            i_stack.pop();
            if (i_stack.empty()) {
                return E_SYNTAX;
            }
            continue;
        }

        // Stuck, report syntax error
        return E_SYNTAX;
    }
}

int Parser::classify(short type, const char *str) const {
    int n = i_grammar.g_ll.ll_nlabels;
    if (type == NAME) {
        const label *l = i_grammar.g_ll.ll_label;
        int i;
        for (i = n; i > 0; i--, l++) {
            if (l->lb_type != NAME || l->lb_str == nullptr || l->lb_str[0] != str[0] || strcmp(l->lb_str, str) != 0)
                continue;
            return n - i;
        }
    }

    const label *l = i_grammar.g_ll.ll_label;
    for (int i = n; i > 0; i--, l++) {
        if (l->lb_type == type && l->lb_str == nullptr)
            return n - i;
    }

    return -1;
}

#pragma clang diagnostic pop