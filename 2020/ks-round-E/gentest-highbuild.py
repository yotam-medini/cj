#!/usr/bin/env python3

import os
from random import randint
import sys

progname = 'highbuild'

def ew(msg):
    sys.stderr.write('%s\n' % msg)

def fatal(msg):
    ew(msg)
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
    rcdiff = syscmd(
        '/bin/bash -c "diff <(grep IMPOSSIBLE %s) <(grep IMPOSSIBLE %s)"' %
        (fn_out_naive, fn_out))
    if rcdiff != 0:
        ew('Inconsistent')
        sys.exit(1)

def check_result(n, a, b, c):
    fn_out = '%s-auto.out' % progname
    line = open(fn_out, 'r').readline()
    tail = line.split(':')[1].strip()
    if 'POSSIBLE' not in tail:
        solution = list(map(int, tail.split()))
        # ew('solution: %s' % str(solution))
        sol_n = len(solution)
        if n != sol_n:
            fatal('n=%d != sol_n=%d' % (n, sol_n))
        a_max = solution[0]
        a_set = set()
        for i in range(0, sol_n):
            if a_max <= solution[i]:
                a_max = solution[i]
                a_set.add(i)
        if n < a_max:
            fatal('a_max=%d > n=%d' % (a_max, n))
        if a != len(a_set):
            ew('a_set=%s' % str(a_set))
            fatal('a=%d != #(a_set)=%d' % (a, len(a_set)))
        b_max = solution[n - 1]
        b_set = set([sol_n - 1])
        for i in range(sol_n - 1, -1, -1):
            if b_max <= solution[i]:
                b_max = solution[i]
                b_set.add(i)
        if n < b_max:
            fatal('b_max=%d > n=%d' % (b_max, n))
        if b != len(b_set):
            ew('b_set=%s' % str(b_set))
            fatal('b=%d != #(b_set)=%d' % (b, len(b_set)))
        c_set = set.intersection(a_set, b_set)
        if c != len(c_set):
            ew('c_set=%s' % str(c_set))
            fatal('c=%d != #(c_set)=%d' % (c, len(c_set)))
    
def rand_test():
    large = False
    fn_in = '%s-auto.in' % progname
    ai = 1
    T = int(sys.argv[ai]); ai += 1
    Nmin = int(sys.argv[ai]); ai += 1
    Nmax = int(sys.argv[ai]); ai += 1
    for t in range(T):
        ew('Tested %d/%d' % (t, T))
        n = randint(Nmin, Nmax)
        a = randint(1, n)
        b = randint(1, n)
        c = randint(1, min(a, b))
        f = open(fn_in, 'w')
        f.write('1\n%d %d %d %d\n' % (n, a, b, c))
        f.close()
        large = n > 5
        if large:
            fn_out = '%s-auto.out' % progname
            rc = syscmd('./bin/%s %s %s' % (progname, fn_in, fn_out))
            check_rc(rc)
        else:
            rundiff(fn_in)
        check_result(n, a, b, c)

def all_small_tests():
    fn_in = '%s-auto.in' % progname
    ntests = 0
    for n in range(1, 5 + 1):
        n_ntests = 0
        for a in range(1, n + 1):
            for b in range(1, n + 1):
                for c in range(1, min(a, b) + 1):
                    f = open(fn_in, 'w')
                    f.write('1\n%d %d %d %d\n' % (n, a, b, c))
                    f.close()
                    rundiff(fn_in)
                    check_result(n, a, b, c)
                    ntests += 1
                    n_ntests += 1
        ew('#(N=%d tests)=%d' % (n, n_ntests))
    ew('#(tests)=%d' % ntests)
                               
if __name__ == '__main__':
    if len(sys.argv) > 2:
        rand_test()
    else:
        all_small_tests()
    sys.exit(0)
        
