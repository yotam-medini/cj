#!/usr/bin/env python3

import os
from random import randint
import sys

progname = 'beautree'

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
    text = open(fn_out_naive).read()
    rc2 = syscmd('./bin/%s %s %s' % (progname, fn_in, fn_out))
    check_rc(rc1 | rc2)
    rcdiff = syscmd('numdiff -r 0.000001 %s %s' % (fn_out_naive, fn_out))
    if rcdiff != 0:
        ew('Inconsistent')
        sys.exit(1)

        
def advance(t, N, done, ntests):
    ew('N=%d, t=%s' % (N, str(t)))
    tlen = len(t)
    if tlen == N - 1:
        for A in range(1, N + 1):
            for B in range(1, N + 1):
                ew('Tested {N=%d] %d/%d' % (N, done, ntests))
                fn_in = '%s-auto.in' % progname
                f = open(fn_in, 'w')
                f.write('1\n%d %d %d\n' % (N, A, B))
                sep = ''
                for p in t:
                    f.write('%s%d' % (sep, p))
                    sep = ' '
                f.write('\n')
                f.close()
                rundiff(fn_in)
                done += 1
    else:
        ew('advance...')
        for p in range(1, tlen + 2):
            subt = t[:]
            subt.append(p)
            ew('subt=%s' % str(subt))
            done = advance(subt, N, done, ntests)
    return done

    
def test_all_small():
    for N in range(1, 7 + 1):
        ntests = N*N
        for k in range(2, N + 1):
            ntests = ntests * k
        t = []
        advance(t, N, 0, ntests)
    

def test_random():
    large = False
    fn_in = '%s-auto.in' % progname
    ai = 1
    T = int(sys.argv[ai]); ai += 1
    Nmin = int(sys.argv[ai]); ai += 1
    Nmax = int(sys.argv[ai]); ai += 1
    Amin = int(sys.argv[ai]); ai += 1
    Amax = int(sys.argv[ai]); ai += 1
    Bmin = int(sys.argv[ai]); ai += 1
    Bmax = int(sys.argv[ai]); ai += 1
    fall = open('in.d/c-auto-all.in', 'w')
    fall.write('%d\n' % min(100, T))
    for t in range(T):
        ew('Tested %d/%d' % (t, T))
        N = randint(Nmin, Nmax)
        A = min(randint(Amin, Amax), N)
        B = min(randint(Bmin, Bmax), N)
        f = open(fn_in, 'w')
        f.write('1\n%d %d %d\n' % (N, A, B))
        if t < 100:
            fall.write('%d %d %d\n' % (N, A, B))
        sep = ''
        for node in range(2, N + 1):
            p = randint(1, node - 1)
            f.write('%s%d' % (sep, p))
            if t < 100:
                fall.write('%s%d' % (sep, p))
            sep = ' '
        f.write('\n')
        if t < 100:
            fall.write('\n')
        f.close()
        large = N > 109
        if large:
            fn_out = '%s-auto.out' % progname
            rc = syscmd('./bin/%s %s %s' % (progname, fn_in, fn_out))
            check_rc(rc)
        else:
            rundiff(fn_in)
    fall.close()


if __name__ == '__main__':
    if len(sys.argv) < 6:
        test_all_small()
    else:
        test_random()
    sys.exit(0)
