#!/usr/bin/env python

import os
import sys

def safe_unlink(fn):
    try:
        os.unlink(fn)
    except:
        pass

def syscmd(cmd):
    sys.stderr.write("%s\n" % cmd)
    rc = os.system(cmd)
    return rc
    
def test(n):
    fn_in = "tidynum-at.in"
    fn_xnout = "tidynum-at.xnout"
    fn_xout = "tidynum-at.xout"
    safe_unlink(fn_in)
    fin = open(fn_in, "w")
    fin.write("1\n%d\n" % n)
    fin.close()
    rc = 0
    safe_unlink(fn_xnout)
    safe_unlink(fn_xout)
    if rc == 0:
        rc = syscmd("./bin/tidynum -naive %s %s" % (fn_in, fn_xnout))
    if rc == 0:
        rc = syscmd("./bin/tidynum %s %s" % (fn_in, fn_xout))
    if rc == 0:
        rc = syscmd("diff %s %s" % (fn_xnout, fn_xout))
    return rc
                 
rc = 0
N = int(sys.argv[1]) if len(sys.argv) > 1 else 1
n = 1
while n <= N and rc == 0:
    rc = test(n)
    n += 1
sys.exit(rc)


