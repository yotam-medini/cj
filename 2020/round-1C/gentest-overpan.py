#!/usr/bin/env python3

import os
from random import randint
import sys

progname = 'overpan'

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
    nmin = int(sys.argv[ai]); ai += 1
    nmax = int(sys.argv[ai]); ai += 1
    dmin = int(sys.argv[ai]); ai += 1
    dmax = int(sys.argv[ai]); ai += 1
    amax = int(sys.argv[ai]); ai += 1
    for t in range(T):
        ew('Tested %d/%d' % (t, T))
        n = randint(nmin, nmax)
        d = randint(dmin, dmax)
        f = open(fn_in, 'w')
        f.write('1\n%d %d\n' % (n, d))
        sep = ''
        for i in range(0, n):
            f.write('%s%d' % (sep, randint(1, amax)))
            sep = ' '
        f.write('\n')
        f.close()
        large = (n > 15) or (d > 10)
        if large:
            fn_out = '%s-auto.out' % progname
            rc = syscmd('./bin/%s %s %s' % (progname, fn_in, fn_out))
            check_rc(rc)
        else:
            rundiff(fn_in)
    sys.exit(0)
