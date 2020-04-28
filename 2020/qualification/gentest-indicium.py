#!/usr/bin/env python3

import os
from random import randint
import sys

progname = 'indicium'

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
    
def check_output(fn_out, N, K):
    f = open(fn_out)
    line1 = f.readline() # 1
    impossible = 'IMPOSSIBLE' in line1
    if not impossible:
        nums_expected = list(range(1, N + 1))
        trace = 0
        for r in range(N):
            line = f.readline()
            nums = list(map(int, line.split()))
            trace += nums[r]
            nums.sort()
            if nums != nums_expected:
                ew('Bad nums in %s' % line)
                sys.exit(1)
        if trace != K:
            ew('trace=%d != k=%d' % (trace, K))
            sys,exit(0)
    f.close()

def rundiff(fn_in, N, K):
    fn_out_naive = '%s-auto-naive.out' % progname
    fn_out = '%s-auto.out' % progname
    rc1 = syscmd('./bin/%s -naive %s %s' % (progname, fn_in, fn_out_naive))
    rc2 = syscmd('./bin/%s %s %s' % (progname, fn_in, fn_out))
    check_rc(rc1 | rc2)
    impossible_naive = 'IMPOSSIBLE' in open(fn_out_naive).readline()
    impossible = 'IMPOSSIBLE' in open(fn_out).readline()
    ew('impossible: naive?=%s, normal=%s' % (impossible_naive, impossible))
    if impossible_naive != impossible:
        ew('Inconsistent')
        sys.exit(1)
    if not impossible:
        check_output(fn_out_naive, N, K)
        check_output(fn_out, N, K)
   
if __name__ == '__main__':
    large = False
    fn_in = '%s-auto.in' % progname
    ai = 1
    Nmin = int(sys.argv[ai]); ai += 1
    Nmax = int(sys.argv[ai]); ai += 1
    t = 0
    for N in range(Nmin, Nmax + 1):
        ew('N=%d [%d, %d]' % (N, Nmin, Nmax))
        large = N > 5
        for K in range(N, N*N + 1):
            ew('Tested %d, K=%d [N=%d, N^2=%d]' % (t, K, N, N*N))
            f = open(fn_in, 'w')
            f.write('1\n%d %d\n' % (N, K))
            f.close()
            if large:
                fn_out = '%s-auto.out' % progname
                rc = syscmd('./bin/%s %s %s' % (progname, fn_in, fn_out))
                check_rc(rc)
                check_output(fn_out, N, K)
            else:
                rundiff(fn_in, N, K)
            t += 1
    sys.exit(0)
