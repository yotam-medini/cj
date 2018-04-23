#!/usr/bin/env python

import os
import random
import sys

progname = 'edgy'

def ew(msg):
    sys.stderr.write('%s\n' % msg)

def syscmd(cmd):
    ew(cmd)
    rc = os.system(cmd)
    return rc

def rundiff(fn_in):
    fn_out_naive = '%s-auto-naive.out' % progname
    fn_out = '%s-auto.out' % progname
    rc1 = syscmd('timeout 5 ./bin/%s -naive %s %s' %
                 (progname, fn_in, fn_out_naive))
    rc2 = syscmd('timeout 5 ./bin/%s %s %s' % (progname, fn_in, fn_out))
    ew('rc1=%d, rc=%d' % (rc1, rc2))
    if (rc1 | rc2) != 0:
        ew('Failed running %s' % progname)
        sys.exit(rc1 | rc2)
    rcdiff = syscmd('numdiff -r 0.0001 %s %s' % (fn_out_naive, fn_out))
    if rcdiff != 0:
        ew('Inconsistent')
        sys.exit(1)


        random.seed(42)
randint = random.randint

if __name__ == '__main__':
    fn_in = '%s-auto.in' % progname
    N = int(sys.argv[1])
    T = int(sys.argv[2])
    for t in range(T):
        f = open(fn_in, 'w')
        simple = randint(0, 1) == 0
        if simple:
            w = randint(1, 10**6)
            h = randint(1, 10**6)
            perimeters = N * 2 * (w + h)
        else:
            w = []
            h = []
            perimeters = 0
            for i in range(N):
                w.append(randint(1, 10**6))
                h.append(randint(1, 10**6))
            perimeters += 2*(sum(w) + sum(h))
        p = randint(perimeters, 10**9)
        f.write('1\n%d %d\n' % (N, p))
        for i in range(N):
            if simple:
               f.write('%d %d\n' % (w, h))
            else:
               f.write('%d %d\n' % (w[i], h[i]))
        f.close()
        rundiff(fn_in)
    sys.exit(0)
