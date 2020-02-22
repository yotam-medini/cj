#!/usr/bin/env python3

import os
from random import randint
import string
import sys

progname = 'scrambled'

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

def generate_text(c1, c2, N, A, B, C, D):
    text = c1 + c2
    x0 = ord(c1)
    x1 = ord(c2)
    while len(text) < N:
        sz = len(text)
        if False and ((sz & (sz - 1)) == 0):
            ew('generate_text sz: %d/%d' % (sz, N))
        xi = (A*x0 + B*x1 + C) % D;
        text += chr(ord('a') + (xi % 26))
        x0 = x1
        x1 = xi
    return text

if False:
    sys.stderr.write('Tiny text:\n%s\n' %
        generate_text('a', 'a', 50, 1, 1, 1, 30))
    sys.exit(7)

def get_lang(text, L, nsol = None):
    lang = set()
    N = len(text)
    if nsol is None:
        nsol = randint(0, min(L, 10**4))
    loops = 0
    prime = 29
    lang_size = 0
    while len(lang) < nsol:
        if False and ((loops & (loops - 1)) == 0):
            ew('scrambled loops=%d, #(lang): %d/%d' % (loops, len(lang), nsol))
        short = (randint(0, 6) > 0)
        sz = None
        while (sz is None) or ((sz % prime) == 2):
            szmax = min(N, 10) if short else (N//2)
            szmax = min(szmax, 10**5, 5*10**6 - lang_size)
            # ew('szmax=%d' % szmax)
            sz = randint(1, szmax)
        # ew('sz=%d' % sz)
        b = randint(0, N - sz)
        word = text[b: b + sz]
        scrambled = word
        if sz > 2:
            scrambled = word[0]
            if False:
                cis = list(range(1, sz - 1))
                while len(cis) > 0:
                    # ew('cis=%s' % str(cis))
                    i = randint(0, len(cis) - 1)
                    cisi = cis[i]
                    scrambled += word[cisi]
                    cis[i] = cis[-1]
                    cis.pop()
            for i in range(sz - 1):
                scrambled += word[1 + ((prime*i) % (sz - 2))]
            scrambled += word[-1]
        if (scrambled in lang):
            ew('scrambled loops=%d, already in lang: %s' % (loops, scrambled))
        else:
            lang.add(scrambled)
            lang_size += len(scrambled)
        loops += 1
    loops = 0
    while len(lang) < L:
        # if ((loops & (loops - 1)) == 0):
        #     ew('loops=%d, #(lang): %d/%d' % (loops, len(lang), L))
        szmax = min(N, 10)
        szmax = min(szmax, 10**5, 5*10**6 - lang_size)
        sz = randint(1, szmax)
        word = ''
        while len(word) < sz:
            word += string.ascii_lowercase[randint(0, 26-1)]
        if word in lang:
            # ew('loops=%d, already in lang: %s' % (loops, word))
            pass
        else:
            lang.add(word)
            lang_size += len(word)
        loops += 1
    return list(lang)

if __name__ == '__main__':
    large = False
    fn_in = '%s-auto.in' % progname
    ai = 1
    T = int(sys.argv[ai]); ai += 1
    Lmax = int(sys.argv[ai]); ai += 1
    Nmin = int(sys.argv[ai]); ai += 1
    Nmax = int(sys.argv[ai]); ai += 1
    nsolMax = int(sys.argv[ai]); ai += 1
    for t in range(T):
        ew('Tested %d/%d' % (t, T))
        L = randint(1, Lmax)
        N = randint(Nmin, Nmax)
        A = randint(0, 10**9)
        B = randint(0, 10**9)
        C = randint(0, 10**9)
        D = randint(1, 10**9)
        nsol = randint(0, min(nsolMax, L))
        ew('L=%d, N=%d, nsol=%d' % (L, N, nsol))
        s1 = chr(ord('a') + randint(0, 25))
        s2 = chr(ord('a') + randint(0, 25))
        text = generate_text(s1, s2, N, A, B, C, D)
        lang = get_lang(text, L, nsol)
        f = open(fn_in, 'w')
        f.write('1\n%d\n' % L)
        f.write('%s\n' % (' '.join(lang)))
        f.write('%s %s %d %d %d %d %d\n' % (s1, s2, N, A, B, C, D))
        f.close()
        large = (L > 1000) or (N > 1000)
        if large:
            fn_out = '%s-auto.out' % progname
            rc = syscmd('./bin/%s %s %s' % (progname, fn_in, fn_out))
            check_rc(rc)
        else:
            rundiff(fn_in)
    sys.exit(0)
