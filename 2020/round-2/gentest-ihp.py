#!/usr/bin/env python3

import os
from random import randint
import sys

progname = 'ihp'

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
    Rmin = int(sys.argv[ai]); ai += 1
    Rmax = int(sys.argv[ai]); ai += 1
    T = (Lmax + 1 - Lmin)*(Rmax + 1 - Rmin)
    t = 0
    for L in range(Lmin, Lmax + 1):
        for R in range(Rmin, Rmax + 1):
            ew('Tested %d/%d' % (t, T))
            f = open(fn_in, 'w')
            f.write('1\n%d %d\n' % (L, R))
            f.close()
            large = min(L, R) > 1000
            if large:
                fn_out = '%s-auto.out' % progname
                rc = syscmd('./bin/%s %s %s' % (progname, fn_in, fn_out))
                check_rc(rc)
            else:
                rundiff(fn_in)
            t += 1
    sys.exit(0)
