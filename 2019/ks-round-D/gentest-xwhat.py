#!/usr/bin/env python3

import os
from random import randint
import sys

progname = 'xwhat'

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

if __name__ == '__main__':
    large = False
    fn_in = '%s-auto.in' % progname
    T = int(sys.argv[1])
    n = int(sys.argv[2])
    q = int(sys.argv[3])
    B = 1024
    B = 10
    for t in range(T):
        ew('Tested %d/%d' % (t, T))
        f = open(fn_in, 'w')
        f.write('1\n%d %d\n' % (n, q))
        sep = ''
        for i in range(n):
            v = randint(0, B - 1)
            f.write('%s%d' % (sep, v))
            sep = ' '
        f.write('\n')
        for i in range(q):
            p = randint(0, n - 1)
            v = randint(0, B - 1)
            f.write('%d %d\n' % (p, v))
        f.close()
        large = n > 20
        if large:
            fn_out = '%s-auto.out' % progname
            rc = syscmd('./bin/%s %s %s' % (progname, fn_in, fn_out))
            check_rc(rc)
        else:
            rundiff(fn_in)
    sys.exit(0)
