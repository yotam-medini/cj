#!/usr/bin/env python3

import os
from random import randint
import sys

progname = 'pylon'

def ew(msg):
    sys.stderr.write('%s\n' % msg)

def fatal(msg, rc=1):
    ew('fatal: ' + msg)
    exit(1)

def syscmd(cmd):
    ew(cmd)
    rc = os.system(cmd)
    return rc

def good_jump(cell0, cell1):
    good = True
    good = good and (cell0[0] != cell1[0])
    good = good and (cell0[1] != cell1[1])
    good = good and (cell0[0] + cell0[1] != cell1[0] + cell1[1])
    good = good and (cell0[0] - cell0[1] != cell1[0] - cell1[1])
    good = cell0[0] != cell1[0]
    return good

def run(fn_in, r, c):
    fn_out = '%s-auto.out' % progname
    rc = syscmd('./bin/%s %s %s' % (progname, fn_in, fn_out))
    if rc != 0:
        fatal('Failed running %s' % progname, rc)
    f = open(fn_out, 'r')
    lines = f.readlines()
    f.close()
    if len(lines) != 1 + r*c:
        fatal('#lines=%d != 1 + %d*%d' % (len(lines), r, c))
    last_cell = None
    cells = set()
    for line in lines[1:]:
        cell = tuple(map(int, line.split()))
        good = (last_cell is None) or good_jump(last_cell, cell)
        if not good:
            fatal('bad jump: last_cell=%s, cell=%s' % (last_cell, cell))
        cells.add(cell)
        last_cell = cell
    if len(cells) != r*c:
        fatal('#(cells)=%d != %d*%d' % (len(cells), r, c))

if __name__ == '__main__':
    fn_in = '%s-auto.in' % progname
    nt = 0
    for m in range(2, 21):
        nbegin = 5 if m == 2 else 4
        for n in range(nbegin, 21):
            ew('Tested %d  m:%d/%d, n=%d/%d' % (nt, m, 20, n, 20))
            f = open(fn_in, 'w')
            f.write('1\n%d %d\n' % (m, n))
            f.close()
            run(fn_in, m, n)
            nt += 1
    sys.exit(0)
