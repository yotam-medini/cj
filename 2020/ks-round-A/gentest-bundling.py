#!/usr/bin/env python3

import os
from random import randint
import string
import sys

progname = 'bundling'

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
    rc2 = syscmd('timeout 6 ./bin/%s %s %s' % (progname, fn_in, fn_out))
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
    Kmin = int(sys.argv[ai]); ai += 1
    Kmax = int(sys.argv[ai]); ai += 1
    Qmin = int(sys.argv[ai]); ai += 1
    Qmax = int(sys.argv[ai]); ai += 1
    Lmax = int(sys.argv[ai]); ai += 1
    AZ = string.ascii_uppercase[:Lmax]
    for t in range(T):
        ew('Tested %d/%d' % (t, T))
        k = randint(Kmin, Kmax)
        q = randint(Qmin, Qmax)
        n = q*k;
        f = open(fn_in, 'w')
        f.write('1\n%d %d\n' % (n, k))
        for wi in range(n):
            l = randint(1, Lmax)
            w = ''
            while len(w) < l:
                azi = randint(0, len(AZ)-1)
                w += AZ[azi]
            f.write('%s\n' % w)
        f.close()
        large = (Lmax > 5) or (n > 10)
        if large:
            fn_out = '%s-auto.out' % progname
            rc = syscmd('timeout 10 ./bin/%s %s %s' % (progname, fn_in, fn_out))
            check_rc(rc)
        else:
            rundiff(fn_in)
    sys.exit(0)
