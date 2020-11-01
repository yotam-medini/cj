#!/usr/bin/env python3

import os
from random import randint
import sys

progname = 'doors'

def ew(msg):
    sys.stderr.write('%s\n' % msg)

def factorial(n):
    ret = 1
    while n > 1:
        ret *= n
        n -= 1
    return ret
    
def next_permutation(a):
    n = len(a)
    # find the last 'in order'
    j = n - 1 - 1
    while j >= 0 and a[j] > a[j + 1]:
        j -= 1
    if j == -1:
        a = None
    else:
        # Find last > a[j]. Must find since a[j] < a[j+1]
        l = n - 1
        while a[j] >= a[l]:
            # sys.stdout.write("l=%d\n" % l)
            l -= 1
        t = a[j];  a[j] = a[l];  a[l] = t
        asis_head = a[:j + 1]
        rev_tail = a[j + 1:]
        rev_tail.reverse()
        a = asis_head + rev_tail
    return a

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

def rand_test():
    large = False
    fn_in = '%s-auto.in' % progname
    ai = 1
    T = int(sys.argv[ai]); ai += 1
    Nmin = int(sys.argv[ai]); ai += 1
    Nmax = int(sys.argv[ai]); ai += 1
    Qmin = int(sys.argv[ai]); ai += 1
    Qmax = int(sys.argv[ai]); ai += 1
    for t in range(T):
        ew('Tested %d/%d' % (t, T))
        f = open(fn_in, 'w')
        N = randint(Nmin, Nmax)
        Q = randint(Qmin, Qmax)
        f.write('1\n%d %d\n' % (N, Q))
        doors = []
        while len(doors) < N - 1:
            d = 10*randint(1, 2*N)
            while d in doors:
                d = 10*randint(1, 2*N)
            doors.append(d)
        f.write('%s\n' % (' '.join(list(map(lambda d: '%d' % d, doors)))))
        for qi in range(Q):
            f.write('%d %d\n' % (randint(1, N), randint(1, N)))
        f.close()
        large = N > 20
        if large:
            fn_out = '%s-auto.out' % progname
            rc = syscmd('./bin/%s %s %s' % (progname, fn_in, fn_out))
            check_rc(rc)
        else:
            rundiff(fn_in)

def permute_test():
    Nmax = 9
    fn_in = '%s-auto.in' % progname
    for N in range(2, Nmax + 1):
        factNm1 = factorial(N - 1)
        NT = factNm1 * N * N;
        ew('N=%d' % N)
        perm = list(range(N - 1))
        t = 0
        while perm is not None:
            for S in range(1, N + 1):
                for K in range(1, N + 1):
                    ew('N=%d: Tested: %d/%d' % (N, t, NT))
                    f = open(fn_in, 'w')
                    f.write('1\n%d 1\n' % N)
                    sep = ''
                    for p in perm:
                        d = 5 if p == 0 else 10*p
                        f.write('%s%d' % (sep, d))
                        sep = ' '
                    f.write('\n')
                    f.write('%d %d\n' % (S, K))
                    f.close()
                    rundiff(fn_in)
                    t += 1
            perm = next_permutation(perm)

if __name__ == '__main__':
    if len(sys.argv) > 1:
        rand_test()
    else:
        permute_test()
    sys.exit(0)
