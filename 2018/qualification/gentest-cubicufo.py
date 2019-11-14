#!/usr/bin/env python3

import os
import sys
import pprint

progname = 'cubicufo'

def strv(a):
    ret = ''
    sep = '('
    for x in a:
        ret += ('%s%g' % (sep, x))
        sep = ', '
    ret += ')'
    return ret

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

def sq_abs(p):
    return p[0]*p[0] + p[1]*p[1]

def dv2(p0, p1):
    # ew('p0=%s' % str(p0))
    # ew('p1=%s' % str(p1))
    return (p1[0] - p0[0], p1[1] - p0[1])

def dv3(p0, p1):
    # ew('p0=%s' % str(p0))
    # ew('p1=%s' % str(p1))
    return (p1[0] - p0[0], p1[1] - p0[1], p1[2] - p0[2])

def sq_norm3(p):
    return p[0]*p[0] + p[1]*p[1] + p[2]*p[2]
    
def trapeze(p0, p1):
    dx = p1[0] - p0[0]
    ymid = (p0[1] + p1[1]) / 2.
    area = dx*ymid
    ew('trapeze(%s, %s) = %g' % (strv(p0), strv(p1), area))
    return area

def calc_area(v8):
    area = 0.
    v82d = list(map(lambda p3: (p3[0], p3[2]), v8)) # project on XZ plane
    ew('v82d=%s' % pprint.pformat(v82d))
    v82d.sort()
    ew('sorted v82d=%s' % pprint.pformat(v82d))

    used = set()
    candidates = set(range(8))
    p0 = v82d[0]
    candidates.remove(0)

    ew('go_right')
    go_right = True
    while go_right: 
        p1ci = p1 = dxy = None
        for p1c in candidates: # max dy/dx
            d1xy = dv2(p0, v82d[p1c])
            if (d1xy[0] >= 0) and (
                (p1ci is None) or (dxy[1] * d1xy[0] < dxy[0] * d1xy[1]) or
                    ((dxy[1] * d1xy[0] == dxy[0] * d1xy[1]) and
                     sq_abs(d1xy) < sq_abs(dxy))):
                p1ci = p1c
                dxy = d1xy
        go_right = p1ci is not None
        # ew('p1ci=%s, candidates=%s' % (p1ci, str(candidates)))
        if go_right:
            candidates.remove(p1ci)
            p1 = v82d[p1ci]
            area += trapeze(p0, p1)
            p0 = p1

    ew('go left')
    go_left = True
    candidates.add(0)
    while go_left: 
        p1ci = p1 = dxy = None
        for p1c in candidates: # max dy/dx
            d1xy = dv2(p0, v82d[p1c])
            if (d1xy[0] <= 0) and (
                (p1ci is None) or (dxy[1] * d1xy[0] < dxy[0] * d1xy[1]) or
                    ((dxy[1] * d1xy[0] == dxy[0] * d1xy[1]) and
                     sq_abs(d1xy) < sq_abs(dxy))):
                    
                p1ci = p1c
                dxy = d1xy
        go_left = p1ci is not None
        # ew('p1ci=%s, candidates=%s' % (p1ci, str(candidates)))
        if go_left:
            candidates.remove(p1ci)
            p1 = v82d[p1ci]
            area += trapeze(p0, p1)
            p0 = p1
    #
    return area

def check_mid_points(p):
    for i in range(3):
        i1 = (i + 1) % 3
        d2 = sq_norm3(dv3(p[i], p[i1]))
        if abs(d2 - 1) > 1e6:
            ew('d2(%s, %s) = %g  !~= 1' % (str(p[i], p[i + 1]), d2))
            sys.exit(1)
    for i in range(3):
        i1 = (i + 1) % 3
        i2 = (i + 2) % 3
        edge1 = dv3(p[i1], p[i])
        edge2 = dv3(p[i2], p[i])
        ew('i=%d, edge1=%s, edge2=%s' % (i, strv(edge1), strv(edge2)))
        sprod2 = edge1[0]*edge1[0] + edge1[1]*edge1[1] + edge1[2]*edge1[2]
        cosine2 = (sprod2 * sprod2) / (sq_norm3(edge1) * sq_norm3(edge2))
        if abs(cosine2) > 1e6:
            ew('Cosine2(%s) = %g >> 0' % (str(p[i]), cosine2))
            sys.exit(1)
    ew('check_mid_points passed')
            
def run_check(fn_in, a):
    fn_out = '%s-auto.out' % progname
    rc = syscmd('./bin/%s %s %s' % (progname, fn_in, fn_out))
    check_rc(rc)
    f = open(fn_out)
    p = []
    caseline = f.readline()
    for i in range(3):
        line = f.readline()
        pt = tuple(map(float, line.split()))
        p.append(pt)
    f.close()
    ew('mid-points=%s' % str(list(map(strv, p))))
    check_mid_points(p)
    v8 = []
    for m in range(8):
        v = [0., 0., 0.]
        for pti in range(3):
            pt = p[pti]
            sign = -1 if (m & (1 << pti)) == 0 else 1
            for d in range(3):
                 v[d] += sign * pt[d]
        v8.append(tuple(v))
    ew('v8=%s' % pprint.pformat(v8))
    area = calc_area(v8)
    err = abs(area - a)/a
    if err > 1e-6:
        ew('err=%g, a=%g, area=%g' % (err, a, area))
        sys.exit(1)

    
if __name__ == '__main__':
    large = False
    fn_in = '%s-auto.in' % progname
    ai = 1
    T = int(sys.argv[ai]); ai += 1
    ab = float(sys.argv[ai]); ai += 1
    ae = float(sys.argv[ai]); ai += 1
    step = 0 if T == 1 else (ae - ab) / (T - 1)
    for t in range(T):
        ew('Tested %d/%d' % (t, T))
        a = ab + t*step
        f = open(fn_in, 'w')
        f.write('1\n%.20f' % a)
        f.close()
        run_check(fn_in, a)
    sys.exit(0)
