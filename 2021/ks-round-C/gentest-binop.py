#!/usr/bin/env python3

import os
from random import randint
import sys

progname = 'binop'

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

def gen_expression(n_op, N_low_max, n_big):
    ret = ''
    if n_op > 0:
        op = '+*#'[randint(0, 2)]
        n_op -= 1
        l_op = randint(0, n_op)
        r_op = n_op - l_op
        n_big_l = randint(0, n_big)
        n_big_r = n_big - n_big_l
        lexp = gen_expression(l_op, N_low_max, n_big_l)
        rexp = gen_expression(r_op, N_low_max, n_big_r)
        ret = '(%s%s%s)' % (lexp, op, rexp)
    else:
        if n_big > 0:
            ret = '123456789'[randint(0, 8)]
            while len(ret) < 20:
                ret += '0123456789'[randint(0, 9)]
        else:
            ret = '%d' % randint(0, N_low_max)
    return ret

if __name__ == '__main__':
    large = False
    fn_in = '%s-auto.in' % progname
    ai = 1
    T = int(sys.argv[ai]); ai += 1
    exp_min = int(sys.argv[ai]); ai += 1
    exp_max = int(sys.argv[ai]); ai += 1
    OP_min = int(sys.argv[ai]); ai += 1
    OP_max = int(sys.argv[ai]); ai += 1
    N_low_max = int(sys.argv[ai]); ai += 1
    N_big_max = int(sys.argv[ai]); ai += 1
    for t in range(T):
        ew('Tested %d/%d' % (t, T))
        n_exp = randint(exp_min, exp_max)
        f = open(fn_in, 'w')
        f.write('1\n%d\n' % n_exp)
        for i in range(n_exp):
            n_op = randint(OP_min, OP_max)
            n_big = randint(0, min(N_big_max, n_op + 1));
            f.write('%s\n' % gen_expression(n_op, N_low_max, n_big))
        f.close()
        if large:
            fn_out = '%s-auto.out' % progname
            rc = syscmd('./bin/%s %s %s' % (progname, fn_in, fn_out))
            check_rc(rc)
        else:
            rundiff(fn_in)
    sys.exit(0)
