#!/usr/bin/env python3

import os
from random import randint
import sys

progname = 'friends'

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
    
def rundiff(fn_in):
    fn_out_naive = '%s-auto-naive.out' % progname
    fn_out = '%s-auto.out' % progname
    rc1 = syscmd('./bin/%s -naive %s %s' % (progname, fn_in, fn_out_naive))
    rc2 = syscmd('./bin/%s %s %s' % (progname, fn_in, fn_out))
    check_rc(rc1 | rc2)
    rcdiff = syscmd('diff %s %s' % (fn_out_naive, fn_out))
    if rcdiff != 0:
        ew('Inconsistent')
        sys.exit(1)

def get_names(N, Smin, Smax):
    names = set()
    while len(names) < N:
        found = False
        while not found:
            S = randint(Smin, Smax)
            name = ''
            while len(name) < S:
                name += chr(ord('A') + randint(0, 25))
            found = name not in names
        names.add(name)
    return names

if __name__ == '__main__':
    large = False
    fn_in = '%s-auto.in' % progname
    ai = 1
    T = int(sys.argv[ai]); ai += 1
    Nmin = int(sys.argv[ai]); ai += 1
    Nmax = int(sys.argv[ai]); ai += 1
    Q = int(sys.argv[ai]); ai += 1
    Smin = int(sys.argv[ai]); ai += 1
    Smax = int(sys.argv[ai]); ai += 1
    for t in range(T):
        ew('Tested %d/%d' % (t, T))
        N = randint(Nmin, Nmax)
        names = get_names(N, Smin, Smax)
        f = open(fn_in, 'w')
        f.write('1\n%d %d\n' % (N, Q))
        sep = ''
        for name in names:
            f.write('%s%s' % (sep, name))
            sep = ' '
        f.write('\n')
        for qi in range(Q):
            x = randint(1, N)
            y = randint(1, N)
            if x > y:
                t = x; x = y; y = t;
            f.write('%d %d\n' % (x, y))
        f.close()
        large = N > 20
        if large:
            fn_out = '%s-auto.out' % progname
            rc = syscmd('./bin/%s %s %s' % (progname, fn_in, fn_out))
            check_rc(rc)
        else:
            rundiff(fn_in)
    sys.exit(0)
