#!/usr/bin/env python
import sys

fin = sys.stdin
fout = sys.stdout
ferr = sys.stderr

def ew(s):
    ferr.write("%s\n" % s)

def ew0(s):
    pass

def get_io(argv):
    fin = sys.stdin
    fout = sys.stdout
    ifn = ofn = "-"
    if len(argv) == 2:
        bfn = sys.argv[1]
        ifn = bfn + '.in'
        ofn = bfn + '.out'
    if len(argv) > 2:
        ifn = argv[1]
        ofn = argv[2]
    if ifn != '-':
        fin = open(ifn, "r")
    if ofn != '-':
        fout = open(ofn, "w")
    return (fin, fout)

def get_numbers():
    line = fin.readline()
    return map(int, line.split())

def get_number():
    return get_numbers()[0]

def get_line():
    line = fin.readline()
    if len(line) > 0 and line[-1] == '\n':
        line = line[:-1]
    return line

def get_string():
    line = fin.readline()
    return line.strip()

class Magicka:

    def __init__(self, fin):
        self.comb = {}
        self.cancel = set()

        line = get_line()
        ss = line.split()

        i = 0;
        C = int(ss[i]);  i += 1
        # ew("C=%d" % C)
        ssi = 1;
        for ci in range(C):
            s3 = ss[i];  i += 1;
            # ew("s3=%s" % s3)
            self.comb[(s3[0], s3[1])] = s3[2]
            self.comb[(s3[1], s3[0])] = s3[2]
        D = int(ss[i]);  i += 1
        # ew("D=%d" % D)
        for ci in range(D):
            s2 = ss[i];  i += 1
            self.cancel.add((s2[0], s2[1]))
            self.cancel.add((s2[1], s2[0]))
        N = int(ss[i]);  i += 1
        self.seqin = ss[i]
        # ew("N=%d, seqin=%s" % (N, self.seqin))

    def solve(self):
        seqout = []
        for c in self.seqin:
            cnew = ' '
            if len(seqout) > 0:
                cnew = self.comb.get((seqout[-1], c), ' ')
            if cnew == ' ':
                cancel = False
                for oc in seqout:
                    cancel = cancel or (c, oc) in self.cancel
                if cancel:
                    seqout = []
                else:
                    seqout.append(c)
            else:
                seqout[-1] = cnew
        self.seqout = seqout

    def get_solution(self ):
        ret = "["
        sep = ""
        for c in self.seqout:
            ret += ("%s%s" % (sep, c))
            sep = ", "
        ret += "]"
        return ret

if __name__ == "__main__":
    (fin, fout) = get_io(sys.argv)
    n_cases = get_number()
    for ci in range(n_cases):
        r = -1
        p = Magicka(fin)
        p.solve()
        r = p.get_solution()
        fout.write("Case #%d: %s\n" % (ci + 1, r))

    fin.close()
    fout.close()
    sys.exit(0)
