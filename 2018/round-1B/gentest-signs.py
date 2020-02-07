#!/usr/bin/env python3

import os
from random import randint
import sys

progname = 'signs'

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

def randint_not(minval, maxval, notval):
    ret = notval
    while ret == notval:
        ret = randint(minval, maxval)
    return ret
    
if __name__ == '__main__':
    large = False
    fn_in = '%s-auto.in' % progname
    ai = 1
    T = int(sys.argv[ai]); ai += 1
    S = int(sys.argv[ai]); ai += 1
    M = int(sys.argv[ai]); ai += 1
    for t in range(T):
        ew('Tested %d/%d' % (t, T))
        f = open(fn_in, 'w')
        f.write('1\n%d\n' % S)
        mvals = []
        nvals = []
        c = randint(2, max(S//4, 2))
        # nc = randint(1, 4)
        segs = []
        b0 = 0
        while b0 + c < S:
            segs.append(randint(b0, S - c - 1))
            m = randint(M + 1, 2*M)
            n = randint(1, M)
            mvals.append(m)
            nvals.append(n)
            b0 = segs[-1] + c + 1
        ew('c=%d, segs=%s, Ms=%s, Ns=%s' %
            (c, str(segs), str(mvals), str(nvals)))
        for si in range(S):
            di = (M * si)//S + randint(0,2)
            in_segs = False
            segi = 0
            while segi < len(segs) and not in_segs:
                in_segs = segs[segi] <= si < segs[segi] + c
                if in_segs:
                    m = mvals[segi]
                    n = nvals[segi]
                segi += 1
            mask = randint(1, 3) if in_segs else 0
            a = m - di
            b = n + di
            if ((mask & 1) == 0):
                a = randint_not(1, M, a)
            if ((mask & 2) == 0):
                b = randint_not(1, M, b)
            f.write('%d %d %d\n' % (di, a, b))
        f.close()
        large = (S > 12)
        if large:
            fn_out = '%s-auto.out' % progname
            rc = syscmd('./bin/%s %s %s' % (progname, fn_in, fn_out))
            check_rc(rc)
        else:
            rundiff(fn_in)
    sys.exit(0)
