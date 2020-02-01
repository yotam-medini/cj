#!/usr/bin/env python

import math
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
    rcdiff = syscmd('numdiff -r 0.00001 %s %s' % (fn_out_naive, fn_out))
    if rcdiff != 0:
        ew('Inconsistent')
        sys.exit(1)


        random.seed(42)
randint = random.randint

if __name__ == '__main__':
    fn_in = '%s-auto.in' % progname
    ai = 1
    N = int(sys.argv[ai]); ai += 1
    T = int(sys.argv[ai]); ai += 1
    wh_max = int(sys.argv[ai]); ai += 1
    scan = False
    if len(sys.argv) > ai:
        scan = sys.argv[ai] == 'scan'; ai += 1
    for t in range(T):
        ew('Tested %d/%d' % (t, T))
        simple = randint(0, 9) == 0
        if simple:
            w = randint(1, wh_max)
            h = randint(1, wh_max)
            perimeters = N * 2 * (w + h)
            diag = math.sqrt(w*w + h*h)
            diags = N * 2 * diag
        else:
            ws = []
            hs = []
            perimeters = 0
            diags = 0.
            for i in range(N):
                w = randint(1, wh_max)
                h = randint(1, wh_max)
                ws.append(w)
                hs.append(h)
                diag = math.sqrt(w*w + h*h)
                diags += 2 * diag
            perimeters += 2*(sum(ws) + sum(hs))
        diags_up = int(diags) + 1
        if scan:
            pb = perimeters; pe = perimeters + diags_up + 2;
        else:
            pb = randint(perimeters, min(perimeters + diags_up, 10**8))
            pe = pb + 1
        for p in range(pb, pe):
            ew('Tested %d/%d, p=%d/[%d,%d)' % (t, T, p, pb, pe))
            f = open(fn_in, 'w')
            f.write('1\n%d %d\n' % (N, p))
            for i in range(N):
                if simple:
                   f.write('%d %d\n' % (w, h))
                else:
                   f.write('%d %d\n' % (ws[i], hs[i]))
            f.close()
            rundiff(fn_in)
    sys.exit(0)
