#!/usr/bin/env python3

import os
from random import randint
import sys

progname = 'fstall'

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
    k = int(sys.argv[ai]); ai += 1
    n = int(sys.argv[ai]); ai += 1
    xmax = 10*n
    cmax = 10*n
    for t in range(T):
        ew('Tested %d/%d' % (t, T))
        f = open(fn_in, 'w')
        f.write('1\n%d %d\n' % (k, n))
        xset = set()
        sep = ''
        for i in range(n):
            x = None
            while (x is None) or x in xset:
                x = randint(1, xmax)
            xset.add(x)
            f.write('%s%d' % (sep, x))
            sep = ' '
        f.write('\n')
        sep = ''
        for i in range(n):
            c = randint(1, cmax)
            f.write('%s%d' % (sep, c))
            sep = ' '
        f.write('\n')
        f.close()
        large = n > 20
        if large:
            fn_out = '%s-auto.out' % progname
            rc = syscmd('./bin/%s %s %s' % (progname, fn_in, fn_out))
            check_rc(rc)
        else:
            rundiff(fn_in)
    sys.exit(0)