#!/usr/bin/env python3

import os
from random import randint
import sys
import time

progname = 'ranks'

def ew(msg):
    sys.stderr.write('%s\n' % msg)

def syscmd(cmd):
    ew(cmd)
    rc = os.system(cmd)
    return rc

def deckstr(deck):
    s = ' '.join(map(lambda card: '%d%d' % card, deck))
    return s

def validate(fn_out, r, s):
    lines = open(fn_out, 'r').readlines()
    n = len(lines) - 1
    n_line0 = int(lines[0].split()[-1])
    valid = (n == n_line0)
    if not valid:
        ew('n=%s, n_line0=%s' % (n, n_line0))
    deck = []
    for si in range(s):
        for ri in range(r):
            deck.append((ri, si))
    ew('Orig Deck: r=%d, s=%d,   %s' % (r, s, deckstr(deck)))
    for line in lines[1:]: # skip  Case#x
        [a, b] = list(map(int, line.split()))
        a_pile = deck[:a]
        b_pile = deck[a: a + b]
        deck = b_pile + a_pile + deck[a + b:]
        ew('(After a=%d, b=%d, deck: %s' % (a, b, deckstr(deck)))
    icard = 0
    ri = 0
    while valid and (ri < r):
        si = 0
        while valid and (si < s):
            valid = valid and (deck[icard][0] == ri)
            si += 1
            icard += 1
        ri += 1
    return valid, n

def check_rc(rc):
    if rc != 0:
        ew('Failed running %s' % progname)
        sys.exit(rc | (rc >> 8))

n_max = 0
def rundiff(fn_in, r, s):
    global n_max
    fn_out_naive = '%s-auto-naive.out' % progname
    fn_out = '%s-auto.out' % progname
    rc = syscmd('./bin/%s -naive %s %s' % (progname, fn_in, fn_out_naive))
    check_rc(rc)
    valid_naive, n_naive = validate(fn_out_naive, r, s)
    if not valid_naive:
        ew('Invalid: naive')
        sys.exit(1)
    return # Temprary !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    rc = syscmd('./bin/%s %s %s' % (progname, fn_in, fn_out))
    check_rc(rc)
    if n_max < n_naive:
        n_max = n_naive
        ew('n_max=%d, r=%d, s=%s' % (n_max, r, s))
        time.sleep(1)
    valid, n = validate(fn_out_naive, r, s)
    if not valid:
        ew('Invalid')
        sys.exit(1)
    if n_naive != n:
        ew('n_naive=%d, n=%d' % (n_naive, n))
        sys.exit(1)

if __name__ == '__main__':
    large = False
    fn_in = '%s-auto.in' % progname
    for r in range(2, 5 + 1):
        for s in range(2, 7 + 1):
            if r*s <= 14:
                open(fn_in, 'w').write('1\n%d %d\n' % (r, s))
                rundiff(fn_in, r, s)
    ai = 1
    T = int(sys.argv[ai]); ai += 1
    rmin = int(sys.argv[ai]); ai += 1
    rmax = int(sys.argv[ai]); ai += 1
    smin = int(sys.argv[ai]); ai += 1
    smax = int(sys.argv[ai]); ai += 1
    for t in range(T):
        ew('Tested %d/%d' % (t, T))
        r = randint(rmin, rmax)
        s = randint(smin, smax)
        f = open(fn_in, 'w')
        f.write('1\n%d %d\n' % (r, s))
        f.close()
        large = (r > 5) or (s > 7) or (r*s > 14)
        if large:
            if False:
                fn_out = '%s-auto.out' % progname
                rc = syscmd('./bin/%s %s %s' % (progname, fn_in, fn_out))
                check_rc(rc)
                valid, n = validate(fn_out, r, s)
        else:
            rundiff(fn_in, r, s)
    sys.exit(0)
