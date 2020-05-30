#!/usr/bin/env python3

import os
from random import randint
import sys

progname = 'candies'

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
    Nmin = int(sys.argv[ai]); ai += 1
    Nmax = int(sys.argv[ai]); ai += 1
    Qmin = int(sys.argv[ai]); ai += 1
    Qmax = int(sys.argv[ai]); ai += 1
    Umin = int(sys.argv[ai]); ai += 1
    Umax = int(sys.argv[ai]); ai += 1
    Amax = int(sys.argv[ai]); ai += 1
    for t in range(T):
        ew('Tested %d/%d' % (t, T))
        f = open(fn_in, 'w')
        n = randint(Nmin, Nmax)
        q = randint(Qmin, Qmax)
        u = randint(Umin, Umax)
        f.write('1\n%d %d\n' % (n, q + u))
        sep = ''
        for i in range(n):
            f.write('%s%d' % (sep, randint(1, Amax)))
            sep = ' '
        f.write('\n')
        qu = q + u
        for i in range(qu):
            typeq = (randint(0, q + u - 1) < q)
            if typeq:
                q -= 1
                l = randint(1, n);
                r = randint(1, n);
                if r < l:
                    t = r; r = l; l = t;
                f.write('Q %d %d\n' % (l, r))
            else:
                f.write('U %d %d\n' % (randint(1, n), randint(1, Amax)))
        f.close()
        large = (n > 10) and (u > 6)
        if large:
            fn_out = '%s-auto.out' % progname
            rc = syscmd('./bin/%s %s %s' % (progname, fn_in, fn_out))
            check_rc(rc)
        else:
            rundiff(fn_in)
    sys.exit(0)
