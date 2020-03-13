#!/usr/bin/env python3

import os
from random import randint
import sys

progname = 'specvill'

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
    ai = 1
    T = int(sys.argv[ai]); ai += 1
    Vmin = int(sys.argv[ai]); ai += 1
    Vmax = int(sys.argv[ai]); ai += 1
    for t in range(T):
        ew('Tested %d/%d' % (t, T))
        V = randint(Vmin, Vmax)
        edges = set()
        covered = set()
        while len(covered) < V:
            edge = None
            while (edge is None) or (edge in edges):
                s = randint(1, V - 1)
                t = randint(s + 1, V)
                edge = (s, t)
            edges.add(edge)
            covered.add(edge[0])
            covered.add(edge[1])
        E = len(edges)
        f = open(fn_in, 'w')
        f.write('1\n%d %d\n' % (V, E))
        lengths = set()
        lmax = 3*V
        for e in edges:
            l = None
            lmax -= 1
            while (l is None) or (l in lengths):
                lmax += 1
                l = randint(0, lmax)
            lengths.add(l)
            f.write('%d %d %d\n' % (e[0], e[1], l))
        f.close()
        large = (V > 10)
        if large:
            fn_out = '%s-auto.out' % progname
            rc = syscmd('./bin/%s %s %s' % (progname, fn_in, fn_out))
            check_rc(rc)
        else:
            rundiff(fn_in)
    sys.exit(0)
