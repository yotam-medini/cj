#!/usr/bin/env python

import os
import random
import sys

MAX = 1000
# MAX = 10

def syscmd(cmd):
    sys.stderr.write("%s\n" % cmd)
    rc = os.system(cmd)
    return rc
    
def test():
    slen = random.randint(2, MAX)
    happy = slen*[True]
    k = random.randint(2, slen)
    fe = slen - k + 1
    f0 = 0
    solution = 0;
    while f0 < fe:
        f = random.randint(f0, fe - 1)
        sys.stderr.write("flip @ %d\n" % f)
        for i in range(f, f + k):
            happy[i] = not happy[i]
        solution += 1
        f0 = random.randint(f + 1, slen)
    impossible = random.randint(0, 4) == 0
    if impossible:
        f0 = slen - k
        for i in range(f0, slen):
            happy[i] = not happy[i]
        bad = random.randint(f0 + 1, slen - 1)
        sys.stderr.write("Last flip @ %d, impossible @ %d\n" % (f0, bad))
        happy[bad] = not happy[bad];
            
    s = ""
    for i in xrange(slen):
        s += '+' if happy[i] else '-'
    fn_in = "ospanflip-rand.in"
    fn_out = "ospanflip-rand.out"
    fn_xout = "ospanflip-rand.xout"
    fin = open(fn_in, "w")
    fin.write("1\n%s %d\n" % (s, k))
    fin.close()
    fout = open(fn_out, "w")
    fout.write("Case #1: ")
    if impossible:
        fout.write("IMPOSSIBLE\n")
    else:
        fout.write("%s\n" % solution)
    fout.close()
    syscmd("./bin/ospanflip %s %s" % (fn_in, fn_xout))
    rc = syscmd("diff %s %s" % (fn_out, fn_xout))
    return rc
                 
rc = 0
N = int(sys.argv[1]) if len(sys.argv) > 1 else 1
t = 0
while t < N and rc == 0:
    rc = test()
    t += 1
sys.exit(rc)


