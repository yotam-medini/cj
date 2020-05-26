#!/usr/bin/env python3

import os
from random import randint
import sys

progname = 'sqdance'

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
    Rmin = int(sys.argv[ai]); ai += 1
    Rmax = int(sys.argv[ai]); ai += 1
    Cmin = int(sys.argv[ai]); ai += 1
    Cmax = int(sys.argv[ai]); ai += 1
    Smax = int(sys.argv[ai]); ai += 1
    for t in range(T):
        ew('Tested %d/%d' % (t, T))
        r = randint(Rmin, Rmax)
        c = randint(Cmin, Cmax)
        f = open(fn_in, 'w')
        f.write('1\n%d %d\n' % (r, c))
        for ri in range(r):
            sep = ''
            for ci in range(r):
                f.write('%s%d' % (sep, randint(1, Smax)))
                sep = ' '
            f.write('\n');
        f.close()
        large = r*c > 199
        if large:
            fn_out = '%s-auto.out' % progname
            rc = syscmd('./bin/%s %s %s' % (progname, fn_in, fn_out))
            check_rc(rc)
        else:
            rundiff(fn_in)
    sys.exit(0)
