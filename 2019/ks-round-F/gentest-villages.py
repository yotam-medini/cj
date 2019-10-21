#!/usr/bin/env python3

import os
from random import randint
import sys

progname = 'villages'

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
    bmax = int(sys.argv[ai]); ai += 1
    for t in range(T):
        ew('Tested %d/%d' % (t, T))
        f = open(fn_in, 'w')
        f.write('1\n%d\n' % n)
        sep = ''
        for bi in range(n):
            f.write('%s%d' % (sep, randint(-bmax, bmax)))
            sep = ' '
        f.write('\n')
        vs = list(range(1, n + 1))
        vi = randint(0, len(vs) - 1)
        v1 = vs[vi]
        vs[vi] = vs[-1]
        vs.pop()
        vi = randint(0, len(vs) - 1)
        v2 = vs[vi]
        vs[vi] = vs[-1]
        vs.pop()
        f.write('%d %d\n' % (v1, v2))
        added = [v1, v2]
        for e in range(n - 2):
            vi = randint(0, len(vs) - 1)
            v1 = vs[vi]
            vs[vi] = vs[-1]
            vs.pop()
            v2 = added[randint(0, len(added) - 1)]
            f.write('%d %d\n' % (v1, v2))
            added.append(v1)
        f.close()
        large = n > 15
        if large:
            fn_out = '%s-auto.out' % progname
            rc = syscmd('timeout 30 ./bin/%s %s %s' % (progname, fn_in, fn_out))
            check_rc(rc)
        else:
            rundiff(fn_in)
    sys.exit(0)
