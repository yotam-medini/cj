#!/usr/bin/env python3

import os
from random import randint
import sys

progname = 'king'

def ew(msg):
    sys.stderr.write('%s\n' % msg)

def fatal(msg):
    ew('Fatal: %s' % msg)
    sys.exit(1)

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

class TestSet:
    def __init__(self, N, V1, H1, A, B, C, D, E, F, M):
        self.N = N
        self.V1 = V1
        self.H1 = H1
        self.A = A
        self.B = B
        self.C = C
        self.D = D
        self.E = E
        self.F = F
        self.M = M
    def __str__(self):
        return '%d %d %d %d %d %d %d %d %d %d' % (
            (self.N, self.V1, self.H1,
             self.A, self.B, self.C, self.D, self.E, self.F, self.M))

    def separated(self):
        vhs = set()
        last = (self.V1, self.H1)
        vhs.add(last)
        ok = True
        i = 1
        while ok and i < self.N:
            vi = (self.A * last[0] + self.B * last[1] + self.C) % self.M;
            hi = (self.D * last[0] + self.E * last[1] + self.F) % self.M;
            vh = (vi, hi)
            ok = vh not in vhs
            if not ok:
                # ew('#(vhs)=%d, vh=%s' % (len(vhs), str(vh)))
                # ew('#(vhs)=%d, vh=%s' % (len(vhs), str(vh)))
                pass
            vhs.add(vh)
            last = vh
            i += 1
        return ok

def rand_test_gen(Nmin, Nmax, Mmin, Mmax):
    found = False
    loops = 0
    ts = None
    while (not found) and (loops < 1000):
        if (loops > 0) and ((loops & (loops - 1)) == 0):
            ew('rand_test_gen: loops=%d' % loops)
        N = randint(Nmin, Nmax)
        M = randint(Mmin, Mmax)
        ts = TestSet(
            N,
            randint(0, M - 1),
            randint(0, M - 1),
            randint(0, M - 1),
            randint(0, M - 1),
            randint(0, M - 1),
            randint(0, M - 1),
            randint(0, M - 1),
            randint(0, M - 1),
            M)
        found = ts.separated()
        loops += 1
    return ts if found else None

if __name__ == '__main__':
    large = False
    fn_in = '%s-auto.in' % progname
    ai = 1
    T = int(sys.argv[ai]); ai += 1
    Nmin = int(sys.argv[ai]); ai += 1
    Nmax = int(sys.argv[ai]); ai += 1
    Mmin = int(sys.argv[ai]); ai += 1
    Mmax = int(sys.argv[ai]); ai += 1
    for t in range(T):
        ew('Tested %d/%d' % (t, T))
        ts = rand_test_gen(Nmin, Nmax, Mmin, Mmax)
        if ts is None:
            fatal('Failed to generate TestSet')
        f = open(fn_in, 'w')
        f.write('1\n%s\n' % ts)
        f.close()
        large = ts.N > 100
        if large:
            fn_out = '%s-auto.out' % progname
            rc = syscmd('./bin/%s %s %s' % (progname, fn_in, fn_out))
            check_rc(rc)
        else:
            rundiff(fn_in)
    sys.exit(0)
