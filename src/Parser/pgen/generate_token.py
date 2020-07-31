#! /usr/bin/env python3
# This script generates token related files from Grammar/Tokens:
#
#   Doc/library/token-list.inc
#   Include/token.h
#   Parser/token.c
#   Lib/token.py


NT_OFFSET = 256


def load_tokens(path):
    tok_names = []
    string_to_tok = {}
    ERRORTOKEN = None
    with open(path) as fp:
        for line in fp:
            line = line.strip()
            # strip comments
            i = line.find('#')
            if i >= 0:
                if i == 0 or line[i - 1] != "'":
                    line = line[:i].strip()
            if not line:
                continue
            fields = line.split()
            name = fields[0]
            value = len(tok_names)
            if name == 'ERRORTOKEN':
                ERRORTOKEN = value
            string = fields[1] if len(fields) > 1 else None
            if string:
                string = eval(string)
                string_to_tok[string] = value
            tok_names.append(name)
    return tok_names, ERRORTOKEN, string_to_tok


def update_file(file, content):
    try:
        with open(file, 'r') as fobj:
            if fobj.read() == content:
                return False
    except (OSError, ValueError):
        pass
    with open(file, 'w') as fobj:
        fobj.write(content)
    return True


token_h_template = """\
/* Auto-generated by Parser/pgen */

/* Token types */
#ifndef TOKEN_H
#define TOKEN_H

#undef TILDE   /* Prevent clash of our definition with system macro. Ex AIX, ioctl.h */

%s\
const int N_TOKENS        = %d;
const int NT_OFFSET       = %d;

/* Special definitions for cooperation with parser */

inline bool ISTERMINAL(int x) { return x < NT_OFFSET; }
inline bool ISNONTERMINAL(int x) {return x >= NT_OFFSET; }
inline bool ISEOF(int x) { return x == ENDMARKER; }


extern const char * const casmTokenNames[]; /* Token names */
int oneChar(int);
int twoChars(int, int);
int threeChars(int, int, int);

#endif /* !TOKEN_H */
"""


def make_h(infile, outfile='Include/token.h'):
    tok_names, ERRORTOKEN, string_to_tok = load_tokens(infile)

    defines = []
    for value, name in enumerate(tok_names[:ERRORTOKEN + 1]):
        defines.append("const int %-15s = %d;\n" % (name, value))

    if update_file(outfile, token_h_template % (
            ''.join(defines),
            len(tok_names),
            NT_OFFSET
    )):
        print("%s regenerated from %s" % (outfile, infile))


token_c_template = """\
/* Auto-generated by Parser/pgen */

#include "token.h"

/* Token names */

const char * const casmTokenNames[] = {
%s\
};

/* Return the token corresponding to a single character */

int
oneChar(int c1)
{
%s\
    return OP;
}

int
twoChars(int c1, int c2)
{
%s\
    return OP;
}

int
threeChars(int c1, int c2, int c3)
{
%s\
    return OP;
}
"""


def generate_chars_to_token(mapping, n=1):
    if len(mapping) == 0:
        return ''
    result = []
    write = result.append
    indent = '    ' * n
    write(indent)
    write('switch (c%d) {\n' % (n,))
    for c in sorted(mapping):
        write(indent)
        value = mapping[c]
        if isinstance(value, dict):
            write("case '%s':\n" % (c,))
            write(generate_chars_to_token(value, n + 1))
            write(indent)
            write('    break;\n')
        else:
            write("case '%s': return %s;\n" % (c, value))
    write(indent)
    write('}\n')
    return ''.join(result)


def make_c(infile, outfile='Parser/token.c'):
    tok_names, ERRORTOKEN, string_to_tok = load_tokens(infile)
    string_to_tok['<>'] = string_to_tok['!=']
    chars_to_token = {}
    for string, value in string_to_tok.items():
        assert 1 <= len(string) <= 3
        name = tok_names[value]
        m = chars_to_token.setdefault(len(string), {})
        for c in string[:-1]:
            m = m.setdefault(c, {})
        m[string[-1]] = name

    names = []
    for value, name in enumerate(tok_names):
        if value >= ERRORTOKEN:
            name = '<%s>' % name
        names.append('    "%s",\n' % name)
    names.append('    "<N_TOKENS>",\n')

    if update_file(outfile, token_c_template % (
            ''.join(names),
            generate_chars_to_token(chars_to_token[1]),
            generate_chars_to_token(chars_to_token[2]),
            generate_chars_to_token(chars_to_token.setdefault(3, {}))
    )):
        print("%s regenerated from %s" % (outfile, infile))


token_inc_template = """\
.. Auto-generated by Tools/scripts/generate_token.py
%s
.. data:: N_TOKENS

.. data:: NT_OFFSET
"""


def make_rst(infile, outfile='Doc/library/token-list.inc'):
    tok_names, ERRORTOKEN, string_to_tok = load_tokens(infile)
    tok_to_string = {value: s for s, value in string_to_tok.items()}

    names = []
    for value, name in enumerate(tok_names[:ERRORTOKEN + 1]):
        names.append('.. data:: %s' % (name,))
        if value in tok_to_string:
            names.append('')
            names.append('   Token value for ``"%s"``.' % tok_to_string[value])
        names.append('')

    if update_file(outfile, token_inc_template % '\n'.join(names)):
        print("%s regenerated from %s" % (outfile, infile))


token_py_template = '''\
"""Token constants."""
# Auto-generated by Tools/scripts/generate_token.py

__all__ = ['tok_name', 'ISTERMINAL', 'ISNONTERMINAL', 'ISEOF']

%s
N_TOKENS = %d
# Special definitions for cooperation with parser
NT_OFFSET = %d

tok_name = {value: name
            for name, value in globals().items()
            if isinstance(value, int) and not name.startswith('_')}
__all__.extend(tok_name.values())

EXACT_TOKEN_TYPES = {
%s
}

def ISTERMINAL(x):
    return x < NT_OFFSET

def ISNONTERMINAL(x):
    return x >= NT_OFFSET

def ISEOF(x):
    return x == ENDMARKER
'''


def make_py(infile, outfile='Lib/token.py'):
    tok_names, ERRORTOKEN, string_to_tok = load_tokens(infile)

    constants = []
    for value, name in enumerate(tok_names):
        constants.append('%s = %d' % (name, value))
    constants.insert(ERRORTOKEN,
                     "# These aren't used by the C tokenizer but are needed for tokenize.py")

    token_types = []
    for s, value in sorted(string_to_tok.items()):
        token_types.append('    %r: %s,' % (s, tok_names[value]))

    if update_file(outfile, token_py_template % (
            '\n'.join(constants),
            len(tok_names),
            NT_OFFSET,
            '\n'.join(token_types),
    )):
        print("%s regenerated from %s" % (outfile, infile))


def main(op, infile='Grammar/Tokens', *args):
    make = globals()['make_' + op]
    make(infile, *args)


if __name__ == '__main__':
    import sys

    main(*sys.argv[1:])
