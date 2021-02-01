#!/usr/bin/env python3

import copy
import os
from random import randint
import sys
import time
import pprint

progname = 'sherbit'

BMA_MAX = 15
# BMA_MAX = 3

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
            ab_found = ((b - a) <= BMA_MAX) and ((a, b) not in ab_set)
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

def test_random():
    large = False
    fn_in = '%s-auto.in' % progname
    ai = 2
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

class Constraint:
    def __init__(self, a, b, c):
        self.a = a
        self.b = b
        self.c = c
    def cmax(self):
        return (self.b - self.a) + 1
    def __str__(self):
        return '%d %d %d' % (self.a, self.b, self.c)
    def __repr__(self):
        return self.__str__()

class TestSet:
    def __init__(self, n, p, contraints):
        self.n = n
        self.p = p
        self.contraints = copy.deepcopy(contraints)
        
    def write(self, f):
        f.write('%d %d %d\n' % (self.n, len(self.contraints), self.p))
        for c in self.contraints:
            f.write('%s\n' % c)

def ab_comb_next(ab_comb, n, k): # assuming k < n
    next = False
    constraints = ab_comb['constraints']
    if constraints is None:
        constraints = []
        for i in range(1, k + 1):
            constraints.append(Constraint(1, i, 0))
        ab_comb['constraints'] = constraints
        next = True
        # ew('ab_comb_next: k=%d, constraints: %s\n' % (k, str(constraints)))
    else:
        j = k - 1
        while ((j >= 0) and
               (constraints[j].a == n - (k - j - 1)) and
               (constraints[j].b == n)):
            j -= 1
        # ew('n=%d, constraints=%s\n' % (n, str(constraints)))
        # ew('ab_comb_next: j = %d' % j)
        if j >= 0:
            constraint = constraints[j]
            if constraint.b < n:
                constraint.b += 1
            elif constraint.a < n:
                constraint.a += 1
                constraint.b = constraint.a
                for j in range(j + 1, k):
                    constraints[j].a = constraints[j - 1].a
                    constraints[j].b = constraints[j - 1].b + 1
            next = constraints[k - 1].b <= n
    # ew('ab_comb_next: ab_comb=%s\n' % pprint.pformat(ab_comb))
    return next

def abc_comb_next(constraints, first_c):
    if first_c:
        next = True
    else:
        k = len(constraints)
        j = k - 1
        while (j >= 0) and constraints[j].c == constraints[j].cmax():
            j -= 1
        next = (j >= 0)
        if next:
            constraints[j].c += 1
            for j in range(j + 1, k):
                constraints[j].c = 0
    return next

def agree(sol, constraints, n):
    ret = True
    for constraint in constraints:
        if ret:
            n_bits = 0
            for bi in range(constraint.a, constraint.b + 1):
                rbi = n - bi
                if (sol & (1 << rbi)) != 0:
                    n_bits += 1
            ret = (n_bits == constraint.c)
    if False and not ret:
        ew('agree: NOT!! sol=%s, constraints=%s' % (sol, str(constraints)))
    return ret

def run_tests(tests):
    fn_in = '%s-auto-all.in' % progname
    f = open(fn_in, 'w')
    f.write('%d\n' % len(tests))
    for test in tests:
        test.write(f)
    fn_out_naive = '%s-auto-all-naive.out' % progname
    fn_out = '%s-auto-all.out' % progname
    rc1 = syscmd('./bin/%s -naive %s %s' % (progname, fn_in, fn_out_naive))
    rc2 = syscmd('./bin/%s %s %s' % (progname, fn_in, fn_out))
    check_rc(rc1 | rc2)
    rcdiff = syscmd('diff %s %s' % (fn_out_naive, fn_out))
    if rcdiff != 0:
        ew('Inconsistent')
        sys.exit(1)
    
    
def test_all_nk(n, k):
    n_total_tests = 0
    tests = []
    ab_comb = {'constraints': None}
    while ab_comb_next(ab_comb, n, k):
        constraints = ab_comb['constraints']
        for constraint in constraints:
            constraint.c = 0
        first_c = True
        while abc_comb_next(constraints, first_c):
            # ew('constraints=%s' % str(constraints))
            first_c = False
            p = 0
            for sol in range(0, 2**n):
                if agree(sol, constraints, n):
                    p += 1
                    testset = TestSet(n, p, constraints)
                    tests.append(testset)
                    if len(tests) == 100:
                        run_tests(tests)
                        n_total_tests += len(tests)
                        tests = []
    if len(tests) > 0:
        run_tests(tests)
        n_total_tests += len(tests)
    ew('Ran %d tests\n' % n_total_tests)
                    
    
def test_all():
    ai = 2
    Nmin = int(sys.argv[ai]); ai += 1
    Nmax = int(sys.argv[ai]); ai += 1
    Kmin = int(sys.argv[ai]); ai += 1
    Kmax = int(sys.argv[ai]); ai += 1
    for n in range(Nmin, Nmax + 1):
        for k in range(Kmin, Kmax + 1):
            test_all_nk(n, k)
    sys.exit(0)

def traverse_constraints(n, k):
    ew('traverse_constraints(%d. %d)\n' % (n, k))
    nc = 0
    ab_comb = {'constraints': None}
    while ab_comb_next(ab_comb, n, k):
        constraints = ab_comb['constraints']
        for constraint in constraints:
            constraint.c = 0
        first_c = True
        while abc_comb_next(constraints, first_c):
            sys.stdout.write('%3d : constraints\n' % nc)
            for constraint in constraints:
                sys.stdout.write('    %s\n' % constraint)
            first_c = False
            nc += 1


if __name__ == '__main__':
    if BMA_MAX != 15:
        ew('!!! Note: BMA_MAX=%d != 15 !!1\n' % BMA_MAX)
        time.sleep(.1)

    test_type = sys.argv[1]
    if test_type == 'random':
        test_random()
    elif test_type == 'all':
        test_all()
    elif test_type == 'traverse_constraints':
        traverse_constraints(int(sys.argv[2]), int(sys.argv[3]))
    else:
        ew('Bad test_type: %s\n' % test_type)
        sys.exit(1)
    sys.exit(0)
    
