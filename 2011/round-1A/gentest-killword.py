#!/usr/bin/env python3

import os
from random import randint
import sys

progname = 'killword'

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
    n = 4
    sz = 4
    az = 'abcdefghijklmnopqrstuvwxyz'
    for t in range(N):
        f = open(fn_in, 'w')
        f.write('1\n')
        f.write('%d 1\n' % n)
        for wi in range(n):
            w = ''
            for ci in range(sz):
                w += az[randint(0, 2)]
            f.write('%s\n' % w)
        f.write('%s\n' % az)
        f.close()
        rundiff(fn_in)
    sys.exit(0)
