#!/usr/bin/env python

import sys
from random import randint

ow = sys.stdout.write
T = 100
T_large = 5
N = 1000; NBIG = 2*(10**5)
Q = 1000; QBIG = 2*(10**5)
PMax = 10**6

ow(f"{T}\n")
for t in range(T):
    large = t < T_large
    n = NBIG if large else N
    q = QBIG if large else Q
    ow(f"{n} {q}\n")
    K = randint(0, PMax)
    ow(f"{K}\n")
    for e in range(2, n + 1):
        P = e - 1 if t == 0 else randint(1, e - 1)
        A = randint(0, PMax)
        B = randint(0, PMax)
        ow(f"{P} {A} {B}\n")
    for _q in range(q):
        u = randint(1, n)
        v = u
        while v == u:
            v = randint(1, N)
        ow(f"{u} {v}\n")
sys.exit(0)


