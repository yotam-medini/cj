#!/usr/bin/env python3

import os
from random import randint
import sys
import textwrap

progname = 'blind'

def ew(msg):
    sys.stderr.write('%s\n' % msg)

def syscmd(cmd):
    ew(cmd)
    rc = os.system(cmd)
    return rc

def run():
    rc = syscmd("timeout 4 "
        "python interactive_runner.py ./my-testing_tool.py 2 -- ./bin/blind")
    if rc != 0:
        ew('Failed')
        sys.exit(1)

if __name__ == '__main__':
    large = False
    fn_in = '%s-auto.in' % progname
    ai = 1
    TENP9 = 10**9
    T = int(sys.argv[ai]); ai += 1
    for t in range(T):
        ew('Tested %d/%d' % (t, T))
        a = TENP9/2
        b = TENP9
        xyr_list = []
        for k in range(2):
            r = randint(a, b)
            cmax = TENP9 - r
            cx = randint(-cmax, cmax)
            cy = randint(-cmax, cmax)
            xyr_list.append((cx, cy, r))
        f = open('extcases.py', 'w')
        f.write('MINR = [None, None, %d]\n' % a)
        f.write('MAXR = [None, None, %d]\n' % b)
        f.write('\n')
        sxyr = ', '.join(map(lambda xyr: '((%d, %d), %d)' % xyr, xyr_list))
        case2 = '[ %s ]' % sxyr
        f.write('CASES = [None, None, %s]\n' % case2)
        f.close()
        run()
    sys.exit(0)
