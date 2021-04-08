#!/usr/bin/env python3

import os
from random import randint
import sys

progname = 'fairies'

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

def get_L(N, Lmin, Lmax):
    L = []
    for i in range(N):
        L.append(N * [0])
    for i in range(N):
        for j in range(i + 1, N):
            len = randint(Lmin, Lmax)
            L[i][j] = L[j][i] = len
    return L;
    
if __name__ == '__main__':
    large = False
    fn_in = '%s-auto.in' % progname
    ai = 1
    T = int(sys.argv[ai]); ai += 1
    Nmin = int(sys.argv[ai]); ai += 1
    Nmax = int(sys.argv[ai]); ai += 1
    Lmin = int(sys.argv[ai]); ai += 1
    Lmax = int(sys.argv[ai]); ai += 1
    for t in range(T):
        ew('Tested %d/%d' % (t, T))
        N = randint(Nmin, Nmax)
        L = get_L(N, Lmin, Lmax)
        f = open(fn_in, 'w')
        f.write('1\n%d\n' % N)
        for i in range(N):
            sep = ''
            for len in L[i]:
                f.write('%s%d' % (sep, len))
                sep = ' '
            f.write('\n')
        f.close()
        large = N > 8
        if large:
            fn_out = '%s-auto.out' % progname
            rc = syscmd('./bin/%s %s %s' % (progname, fn_in, fn_out))
            check_rc(rc)
        else:
            rundiff(fn_in)
    sys.exit(0)
