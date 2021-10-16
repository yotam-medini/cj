#!/usr/bin/env python3

import os
from random import randint
import sys

progname = 'hydrated'

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
    Kmin = int(sys.argv[ai]); ai += 1
    Kmax = int(sys.argv[ai]); ai += 1
    Xmin = int(sys.argv[ai]); ai += 1
    Xmax = int(sys.argv[ai]); ai += 1
    Ymin = int(sys.argv[ai]); ai += 1
    Ymax = int(sys.argv[ai]); ai += 1
    for t in range(T):
        ew('Tested %d/%d' % (t, T))
        f = open(fn_in, 'w')
        K = randint(Kmin, Kmax)
        f.write('1\n%d\n' % K)
        for ki in range(K):
            found = False
            while not found:
                x1 = randint(Xmin, Xmax - 1)
                x2 = randint(Xmin + 1, Xmax)
                if x1 > x2:
                    t = x1; x1 = x2; x2 = x1;
                y1 = randint(Ymin, Ymax - 1)
                y2 = randint(Ymin + 1, Ymax)
                if y1 > y2:
                    t = y1; y1 = y2; y2 = t;
                found = (x1 < x2) and (y1 < y2)
            f.write(f'{x1} {y1} {x2} {y2}\n')
        f.close()
        large = K > 20
        if large:
            fn_out = '%s-auto.out' % progname
            rc = syscmd('./bin/%s %s %s' % (progname, fn_in, fn_out))
            check_rc(rc)
        else:
            rundiff(fn_in)
    sys.exit(0)
