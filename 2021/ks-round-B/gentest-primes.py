#!/usr/bin/env python3

import os
from random import randint
import sys

progname = 'primes'

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
    rc2 = syscmd('./bin/%s-opt %s %s' % (progname, fn_in, fn_out))
    check_rc(rc1 | rc2)
    rcdiff = syscmd('diff %s %s' % (fn_out_naive, fn_out))
    if rcdiff != 0:
        ew('Inconsistent')
        sys.exit(1)

if __name__ == '__main__':
    large = False
    fn_in = '%s-auto.in' % progname
    ai = 1
    # T = int(sys.argv[ai]); ai += 1
    T = 1
    for t in range(T):
        ew('Tested %d/%d' % (t, T))
        f = open(fn_in, 'w')
        f.write('100\n')
        pending = 100
        for n in [6, 7, 13, 14, 15, 16, 20, 21, 22, 34, 35, 36]:
            f.write('%d\n' % n)
            pending -= 1;
        pwr = 1
        while pending > 0:
            pwr += 1
            b = 10**pwr;
            k = 0
            while (pending > 0) and k < 0x10:
                f.write('%d\n' % (b + k))
                pending -= 1
                k += 1
        f.close()
        if large:
            fn_out = '%s-auto.out' % progname
            rc = syscmd('./bin/%s %s %s' % (progname, fn_in, fn_out))
            check_rc(rc)
        else:
            rundiff(fn_in)
    sys.exit(0)
