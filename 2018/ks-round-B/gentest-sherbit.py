#!/usr/bin/env python3

import os
from random import randint
import sys

progname = 'sherbit'

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

def get_abcs(N, K):
    ab_set = set()
    abcs = []
    while len(abcs) < K:
        ab_found = False
        while not ab_found:
            a = randint(1, N)
            b = randint(1, N)
            if b < a:
                t = a; a = b; b = t;
            ab_found = ((b - a) <= 15) and ((a, b) not in ab_set)
        ab_set.add((a, b))
        c = randint(0, b - a + 1)
        abcs.append((a, b, c))
    return abcs

def get_pmax(N, ABCs):
    n_legals = 0
    for n in range(0, 2**N):
        legal = True
        for (a, b, c) in ABCs:
            if legal:
                n_bits = 0
                for bi in range(a, b + 1):
                    bpos = N - bi
                    if (n & (1 << bpos)) != 0:
                        n_bits += 1
                legal = (n_bits == c)
        if legal:
            n_legals += 1
    return n_legals

if __name__ == '__main__':
    large = False
    fn_in = '%s-auto.in' % progname
    ai = 1
    T = int(sys.argv[ai]); ai += 1
    Nmin = int(sys.argv[ai]); ai += 1
    Nmax = int(sys.argv[ai]); ai += 1
    Kmin = int(sys.argv[ai]); ai += 1
    Kmax = int(sys.argv[ai]); ai += 1
    t = 0
    while t < T:
        N = randint(Nmin, Nmax)
        Nchoose2 = (N*(N - 1))//2 + N
        kmax_n = min(Kmax, Nchoose2)
        kmin_n = min(Kmin, kmax_n)
        K = randint(kmin_n, kmax_n)
        ew('N=%d, K=%d, Tested %d/%d' % (N, K, t, T))
        ABCs = get_abcs(N, K)
        # ew('ABCs: %s' % str(ABCs))
        Pmax = get_pmax(N, ABCs)
        Ps = []
        if Pmax >= 1:
            Ps = list(set([1, Pmax, randint(1, Pmax)]))
        for P in Ps:
            ew('Tested %d/%d' % (t, T))
            f = open(fn_in, 'w')
            f.write('1\n%d %d %d\n' % (N, K, P))
            for abc in ABCs:
                f.write('%d %d %d\n' % abc)
            f.close()
            if large:
                fn_out = '%s-auto.out' % progname
                rc = syscmd('./bin/%s %s %s' % (progname, fn_in, fn_out))
                check_rc(rc)
            else:
                rundiff(fn_in)
            t += 1
    sys.exit(0)
