#!/usr/bin/env python3

import os
from random import randint
import sys

progname = 'cherries'

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
    n = int(sys.argv[ai]); ai += 1
    mdef = int(sys.argv[ai]) if (ai < len(sys.argv)) else None
    if n <= 7:
        all_edges = []
        for c0 in range(1, n):
            for c1 in range(c0 + 1, n + 1):
                all_edges.append((c0, c1))
    for t in range(T):
        ew('Tested %d/%d' % (t, T))
        f = open(fn_in, 'w')
        f.write('1\n')
        mmax = min((n*(n-1))/2, 100000)
        m = randint(0, mmax) if mdef is None else mdef
        f.write('%d %d\n' % (n, m))
        large = n > 7
        if large:
            pass
            vedges = set()
            for ei in range(m):
                if (ei & (ei - 1)) == 0:
                    ew('edges: %d/%d' % (ei, m))
                added = False
                while not added:
                    c1 = randint(1, n)
                    c2 = randint(1, n)
                    c1c2 = (c1, c2)
                    if (c1 < c2) and c1c2 not in vedges:
                        f.write('%d %d\n' % c1c2)
                        vedges.add(c1c2)
                        added = True
        else:
            vedges = all_edges[:]
            for i in range(m):
                ei = randint(0, len(vedges) - 1)
                e = vedges[ei]
                f.write('%d %d\n' % (e[0], e[1]))
                vedges[ei] = vedges[-1]
                vedges.pop()
        f.close()
        if large:
            fn_out = '%s-auto.out' % progname
            rc = syscmd('timeout 1 ./bin/%s %s %s' % (progname, fn_in, fn_out))
            check_rc(rc)
        else:
            rundiff(fn_in)
    sys.exit(0)
