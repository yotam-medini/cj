#!/usr/bin/env python3

import os
from random import randint
import re
import sys

progname = 'pattern'

def ew(msg):
    sys.stderr.write('%s\n' % msg)

def syscmd(cmd):
    ew(cmd)
    rc = os.system(cmd)
    return rc

def check_rc(rc):
    if rc != 0:
        ew('Failed running %s' % progname)
        sys.exit(rc | (rc >> 8))
    
def check(who, orig, patterns):
    for pat in patterns:
        dotaster = '^' + pat.replace('*', '.*') + '$'
        if re.match(dotaster, orig) is None:
            ew('%s=%s, Unmatched by %s' % (who, orig, pat))
            sys.exit(1)

def run(orig, patterns):
    check('orig', orig, patterns)
    fn_in = '%s-auto.in' % progname
    fn_out = '%s-auto.out' % progname
    f = open(fn_in, 'w')
    f.write('1\n%s\n' % len(patterns))
    for pat in patterns:
        f.write('%s\n' % pat)
    f.close()
    rc = syscmd('./bin/%s %s %s' % (progname, fn_in, fn_out))
    if rc != 0:
        ew('%s failed' % progname)
        sys.exit(1)
    line = open(fn_out).readline()
    solution = line.split()[-1]
    check('solution', solution, patterns)

if __name__ == '__main__':
    large = False
    fn_in = '%s-auto.in' % progname
    ai = 1
    T = int(sys.argv[ai]); ai += 1
    Nmin = int(sys.argv[ai]); ai += 1
    Nmax = int(sys.argv[ai]); ai += 1
    Amin = int(sys.argv[ai]); ai += 1
    Amax = int(sys.argv[ai]); ai += 1
    orig = 'AABBCCDDEEFFGGHH'
    # orig = 'ABCD'
    for t in range(T):
        ew('Tested %d/%d' % (t, T))
        n = randint(Nmin, Nmax)
        patterns = []
        for pi in range(n):
            pat = orig
            na = randint(Amin, Amax)
            for a in range(na):
                apos = randint(0, len(pat) + 1)
                ndel = randint(0, 2)
                pat = pat[:apos] + '*' + pat[apos + ndel:]
            patterns.append(pat)
        ew('patterns: %s' % str(patterns))
        run(orig, patterns)
    sys.exit(0)
