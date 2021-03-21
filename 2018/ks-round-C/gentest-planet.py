#!/usr/bin/env python3

import os
from random import randint
import sys

progname = 'planet'

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
    Nmin = int(sys.argv[ai]); ai += 1
    Nmax = int(sys.argv[ai]); ai += 1
    for t in range(T):
        ew('Tested %d/%d' % (t, T))
        N = randint(Nmin, Nmax)
        cycle_size = randint(3, N)
        ew('N=%d, cycle_size=%d\n' % (N, cycle_size))
        planets = list(range(N))
        cycle = []
        while len(cycle) < cycle_size:
            i = randint(0, len(planets) - 1)
            cycle.append(planets[i])
            planets[i] = planets[-1]
            planets.pop()
        edges = []
        for i in range(len(cycle)):
            edges.append((cycle[i - 1], cycle[i]))
        boundary = cycle[:]
        boundary.sort()
        ew('boundary=%s\n' % str(boundary))
        while len(planets) > 0:
            ib = randint(0, len(boundary) - 1)
            ip = randint(0, len(planets) - 1)
            edges.append((boundary[ib], planets[ip]))
            boundary[ib] = planets[ip]
            planets[ip] = planets[-1]
            planets.pop()
        edges.sort()
        f = open(fn_in, 'w')
        f.write('1\n%d\n' % N)
        for edge in edges:
            f.write('%d %d\n' % (edge[0] + 1, edge[1] + 1))
        f.close()
        if large:
            fn_out = '%s-auto.out' % progname
            rc = syscmd('./bin/%s %s %s' % (progname, fn_in, fn_out))
            check_rc(rc)
        else:
            rundiff(fn_in)
    sys.exit(0)
