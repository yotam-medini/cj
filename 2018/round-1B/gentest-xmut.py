#!/usr/bin/env python3

import os
from random import randint
import sys
import time

progname = 'xmut'

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
    
def rundiff(fn_in, naive):
    fn_out_naive = '%s-auto-naive.out' % progname
    fn_out = '%s-auto.out' % progname
    rc1 = syscmd('./bin/%s -%s %s %s' % (progname, naive, fn_in, fn_out_naive))
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
    M = int(sys.argv[ai]); ai += 1
    G = int(sys.argv[ai]); ai += 1
    NC = 1
    if ai < len(sys.argv):
        NC = int(sys.argv[ai]); ai += 1
    dt_max = 0
    for t in range(T):
        ew('Tested %d/%d' % (t, T))
        f = open(fn_in, 'w')
        f.write('%d\n' % NC)
        for tcase in range(NC):
            f.write('%d\n' % M)
            for mi in range(M):
                r1 = r2 = 0
                while r1 >= r2:
                    r1 = randint(1, M)
                    r2 = randint(1, M)
                f.write('%d %d\n' % (r1, r2))
            sep = ''
            for mi in range(M):
                f.write('%s%d' % (sep, randint(0, G)))
                sep = ' '
            f.write('\n')
        f.close()
        large = (M > 100) or (G > 100)
        t0 = time.time()
        if large:
            fn_out = '%s-auto.out' % progname
            rc = syscmd('./bin/%s %s %s' % (progname, fn_in, fn_out))
            check_rc(rc)
        else:
            naive = 'naive1' if (M > 8) or (G > 8) else 'naive'
            rundiff(fn_in, naive)
        t1 = time.time()
        dt = t1 - t0
        if dt_max < dt:
            dt_max = dt
            syscmd('cp %s %s-auto-slow.in' % (fn_in, progname))
            ew('dt_max=%g' % dt_max)
    ew('dt_max=%g' % dt_max)
    sys.exit(0)
