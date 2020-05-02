#!/usr/bin/env python3

import sys
from random import randint

def n2ab(n):
    s = '%d' % n
    ret = ''
    for c in s:
        ret += 'ABCDEFGHIJ'[int(c)]
    return ret

sys.stdout.write('1\n16\n')
for i in range(10000):
    M = randint(1, 10**16)
    n = randint(1, M)
    sys.stdout.write('%d %s\n' % (M, n2ab(n)))
sys.exit(0)

