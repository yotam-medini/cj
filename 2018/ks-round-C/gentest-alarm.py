#!/usr/bin/env python3

import os
from random import randint
import sys

progname = 'alarm'

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

if __name__ == '__main__':
    large = False
    fn_in = '%s-auto.in' % progname
    ai = 1
    T = int(sys.argv[ai]); ai += 1
    Ncases = int(sys.argv[ai]); ai += 1
    # N, K, x1, y1, C, D, E1, E2, F
    Nmin = int(sys.argv[ai]); ai += 1
    Nmax = int(sys.argv[ai]); ai += 1
    Kmin = int(sys.argv[ai]); ai += 1
    Kmax = int(sys.argv[ai]); ai += 1
    x1min = int(sys.argv[ai]); ai += 1
    x1max = int(sys.argv[ai]); ai += 1
    y1min = int(sys.argv[ai]); ai += 1
    y1max = int(sys.argv[ai]); ai += 1
    Cmin = int(sys.argv[ai]); ai += 1
    Cmax = int(sys.argv[ai]); ai += 1
    Dmin = int(sys.argv[ai]); ai += 1
    Dmax = int(sys.argv[ai]); ai += 1
    E1min = int(sys.argv[ai]); ai += 1
    E1max = int(sys.argv[ai]); ai += 1
    E2min = int(sys.argv[ai]); ai += 1
    E2max = int(sys.argv[ai]); ai += 1
    Fmin = int(sys.argv[ai]); ai += 1
    Fmax = int(sys.argv[ai]); ai += 1
    for t in range(T):
        ew('Tested %d/%d' % (t, T))
        f = open(fn_in, 'w')
        f.write('%d\n' % Ncases)
        for tc in range(Ncases):
            N = randint(Nmin, Nmax)
            K = randint(Kmin, Kmax)
            x1 = randint(x1min, x1max)
            y1 = randint(y1min, y1max)
            C = randint(Cmin, Cmax)
            D = randint(Dmin, Dmax)
            E1 = randint(E1min, E1max)
            E2 = randint(E2min, E2max)
            F = randint(Fmin, Fmax)
            f.write('%d %d %d %d %d %d %d %d %d\n' %
                    (N, K, x1, y1, C, D, E1, E2, F))
        f.close()
        large = Nmax > 20
        if large:
            fn_out = '%s-auto.out' % progname
            rc = syscmd('./bin/%s %s %s' % (progname, fn_in, fn_out))
            check_rc(rc)
        else:
            rundiff(fn_in)
    sys.exit(0)
