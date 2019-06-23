#!/usr/bin/env python

import os
import random
import sys

progname = 'parcel'

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
    rmax = int(sys.argv[2])
    cmax = int(sys.argv[3])
    zto1 = int(sys.argv[4])
    zero_ones = (zto1 * '0') + '1'
    for t in range(N):
        ew('Tested %d/%d' % (t, N))
        r = random.randint(1, rmax) if rmax <= 20 else rmax
        c = random.randint(1, cmax) if cmax <= 20 else cmax
        f = open(fn_in, 'w')
        f.write('1\n%d %d\n' % (r, c))
        for y in range(r):
            line = ''
            for x in range(c):
                zo = zero_ones[random.randint(0, len(zero_ones) - 1)]
                line += zo
            f.write('%s\n' % line)
        f.close()
        if max(r, c) <= 20:
            rundiff(fn_in)
        else:
            fn_out = '%s-auto.out' % progname
            rc = syscmd('./bin/%s %s %s' % (progname, fn_in, fn_out))
            if rc != 0:
                sys.exit(rc | (rc >> 8))
    sys.exit(0)
