#!/usr/bin/env python3

import os
from random import randint
import sys

progname = 'rps'

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

def rps_index(c):
    i = 0
    RPS = 'RPS'
    while i < 3 and RPS[i] != c:
        i += 1
    if RPS[i] != c:
        ew('Bad c: %s' % c)
        sys.exit(1)
    return i
        
def run(fn_in, wes, X):
    fn_out = '%s-auto.out' % progname
    rc = syscmd('./bin/%s -debug 0x9 %s %s' % (progname, fn_in, fn_out))
    check_rc(rc)
    if rc != 0:
        ew('Fail')
        sys.exit(rc)
    out_lines = open(fn_out, 'r').readlines()
    days = len(out_lines)
    ew('#(out_lines)=%d' % days)
    if days != len(wes):
        ew('Inconsistent: #(wes)=%d != days=%s' % (len(wes), days))
        sys.exit(1)
    average = 0
    for day, line in enumerate(out_lines):
        count = [0, 0, 0]
        expectation = 0.
        W, E = wes[day]
        we_debug = (W, E) == (50, 0)
        seq = line.split()[-1]
        for r, c in enumerate(seq):
            i = rps_index(c)
            i1 = (i + 1) % 3
            i2 = (i + 2) % 3
            if r == 0:
                add = (W + E) / 3.
            else:
                pwin = float(count[i1]) / float(r)
                pequ = float(count[i2]) / float(r)
                add = pwin * W + pequ * E
            expectation += add
            if we_debug:
                ew('we_debug: r=%d, i=%d, count=%s, add=%g' %
                   (r, i, str(count), add))
            count[i] += 1
        ew('day: %s, expectation=%8.2f : W=%3d, E=%3d : %s' %
           (day, expectation, W, E, seq))
        average += expectation
    average = average / float(days)
    ew('Average: %g' % average)
    if average < X:
        ew('Average expectation to low: %g < X=%g' % (average, X))
        

def selectG():
    G = set()
    while len(G) < 50:
        g = randint(5, 95)
        G.add(g)
    return list(G)

if __name__ == '__main__':
    large = False
    X = 16400
    fn_in = '%s-auto.in' % progname
    ai = 1
    T = int(sys.argv[ai]); ai += 1
    for t in range(T):
        ew('Tested %d/%d' % (t, T))
        G = selectG()
        f = open(fn_in, 'w')
        f.write('200\n%d\n' % X)
        wes = []
        for g in G:
            W = 10*g
            for E in (W, W//2, W//10, 0):
                we = (W, E)
                f.write('%d %d\n' % we)
                wes.append(we)
        f.close()
        run(fn_in, wes, X)
    sys.exit(0)
