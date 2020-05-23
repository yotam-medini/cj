#!/usr/bin/env python3

import os
from random import randint
import sys
import pprint

progname = 'pascalwalk'

def ew(msg):
    sys.stderr.write('%s\n' % msg)

def fatal(msg):
    ew(msg)
    sys.exit(1)

triangle = []
triangle.append([1])
for r in range(1, 500):
    prev = triangle[-1]
    row = [1]
    for c in range(1, r):
        v = prev[c - 1] + prev[c]
        row.append(v)
    row.append(1)
    triangle.append(row)

# ew('Triangle:\n%s\n' % pprint.pformat(triangle[:5]))
# sys.exit(7)

def syscmd(cmd):
    ew(cmd)
    rc = os.system(cmd)
    return rc

def check(rc, N, fn_out):
    if rc != 0:
        ew('Failed running %s' % progname)
        sys.exit(rc | (rc >> 8))
    f = open(fn_out, 'r')
    line = f.readline()
    if line != 'Case #1:\n':
        ew('Unexpected first output line: %s' % line)
    pv = 0
    path_size = 0
    for line in f.readlines():
        ss = line.split()
        r = int(ss[0]) - 1
        c = int(ss[1]) - 1
        v = triangle[r][c]
        pv += v
        path_size += 1
    if path_size > 500:
        fatal('path_size=%d' % path_size)
    if pv != N:
        fatal('%s: N=%d != pv=%d' % (fn_out, N, pv))

def run(N):
    fn_in = '%s-auto.in' % progname
    fn_out_naive = '%s-auto-naive.out' % progname
    fn_out = '%s-auto.out' % progname
    open(fn_in, 'w').write('1\n%d\n' % N)
    if N <= 20:
        rc = syscmd('./bin/%s -naive %s %s' % (progname, fn_in, fn_out_naive))
        check(rc, N, fn_out_naive)
    rc = syscmd('./bin/%s %s %s' % (progname, fn_in, fn_out))
    check(rc, N, fn_out)

if __name__ == '__main__':
    large = False
    fn_in = '%s-auto.in' % progname
    ai = 1
    Nmin = int(sys.argv[ai]); ai += 1
    Nmax = int(sys.argv[ai]); ai += 1
    for n in range(Nmin, Nmax + 1):
        ew('Testing: n=%d in [%d %d]' % (n, Nmin, Nmax))
        run(n)
    sys.exit(0)
