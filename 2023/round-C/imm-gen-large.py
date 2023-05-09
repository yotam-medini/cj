#!/usr/bin/env python

import sys

V = int(sys.argv[1])
M = 2*V + 2
ow = sys.stdout.write
ow(f"1\n{V} {M}\n")

sep = ''
for i in range(V):
    ow(f"{sep}{2*V}")
    sep = ' '
ow('\n')

sep = ''
for d in range(1, V + 1):
    ow(f"{sep}{d}")
    sep = ' '
ow('\n')

sep = ''
for m in range(V):
    ow(f"{sep}{V + 1}")
    sep = ' '
    ow(f"{sep}{-(V + 1)}")

ow(f"{sep}{3*V}")
ow(f"{sep}{-(3*V)}")
ow('\n')

sys.exit(0)

