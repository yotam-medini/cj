#!/usr/bin/env python

import os
import random
import sys

progname = 'bitparty'

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
    if rc1 == 0:
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
    C = int(sys.argv[2])
    for n in range(N):
        f = open(fn_in, 'w')
        f.write('1\n')
        r = random.randint(1, C)
        b = random.randint(5, 9)
        f.write("%d %d %d\n" % (r, b, C))
        m_total = 0
        for i in range(C):
            m_min = 1 if i < C - 1 else max((b - m_total), 1)
            m = random.randint(m_min, b)
            m_total += m
            s = random.randint(1, 6)
            p = random.randint(1, 6)
            f.write("%d %d %d\n" % (m, s, p))
        f.close()
        rundiff(fn_in)
    sys.exit(0)
