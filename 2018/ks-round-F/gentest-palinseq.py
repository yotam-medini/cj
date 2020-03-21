#!/usr/bin/env python3

import os
from random import randint
import sys

progname = 'palinseq'

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
    Lmin = int(sys.argv[ai]); ai += 1
    Lmax = int(sys.argv[ai]); ai += 1
    Nmin = int(sys.argv[ai]); ai += 1
    Nmax = int(sys.argv[ai]); ai += 1
    t = 0
    for L in range(Lmin, Lmax + 1):
        for N in range(Nmin, Nmax + 1):
            GotZero = False
            k = 1
            while not GotZero:
                ew('Tested %d, L=%d, N=%d, K=%d' % (t, L, N, k))
                f = open(fn_in, 'w')
                f.write('1\n%d %d %d\n' % (L, N, k))
                f.close()
                large = (L > 10) or (N > 10)
                fn_out = '%s-auto.out' % progname
                if large:
                    rc = syscmd('./bin/%s %s %s' % (progname, fn_in, fn_out))
                    check_rc(rc)
                else:
                    rundiff(fn_in)
                line = open(fn_out).readline()
                GotZero = line.startswith('Case #1: 0')
                k += 1
                t += 1
    sys.exit(0)
