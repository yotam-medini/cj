#!/usr/bin/env python3

import os
from random import randint
import sys

progname = 'cart'

def ew(msg):
    sys.stderr.write('%s\n' % msg)

def syscmd(cmd):
    ew(cmd)
    rc = os.system(cmd)
    return rc

def rundiff(fn_in):
    fn_out_naive = '%s-auto-naive.out' % progname
    fn_out = '%s-auto.out' % progname
    rc1 = syscmd('./bin/%s -naive %s %s' % (progname, fn_in, fn_out_naive))
    rc2 = syscmd('./bin/%s %s %s' % (progname, fn_in, fn_out))
    if (rc1 | rc2) != 0:
        ew('Failed running %s' % progname)
        sys.exit(rc1 | rc2)
    rcdiff = syscmd('diff %s %s' % (fn_out_naive, fn_out))
    if rcdiff != 0:
        ew('Inconsistent')
        sys.exit(1)

if __name__ == '__main__':
    fn_in = '%s-auto.in' % progname
    N = int(sys.argv[1])
    p = int(sys.argv[2])
    q = int(sys.argv[3])
    for t in range(N):
        ew('Tested %d/%d' % (t, N))
        xy_used = set()
        f = open(fn_in, 'w')
        f.write('1\n%d %d\n' % (p, q))
        for i in range(p):
            alloc_xy = True
            while alloc_xy:
                x = randint(0, q - 1)
                y = randint(0, q - 1)
                alloc_xy = (x, y) in xy_used
            alloc_d = True
            while alloc_d:
                d = 'EWSN'[randint(0, 3)]
                alloc_d = (
                    ((x == 0) and (d == 'W')) or
                    ((x == q - 1) and (d == 'E')) or
                    ((y == 0) and (d == 'S')) or
                    ((y == q - 1) and (d == 'N')))
            xy_used.add((x, y))
            f.write('%d %d %s\n' % (x, y, d))
        f.close()
        if q <= 10:
            rundiff(fn_in)
        else:
            fn_out = '%s-auto.out' % progname
            rc = syscmd('./bin/%s %s %s' % (progname, fn_in, fn_out))
            if rc != 0:
                ew('Failed')
                sys.exit(rc)
    sys.exit(0)
