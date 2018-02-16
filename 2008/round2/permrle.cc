// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <fstream>
#include <string>
// #include <set>
// #include <map>
#include <algorithm>
#include <vector>
#include <utility>

#include <cstdlib>
#include <cstring>
// #include <gmpxx.h>

using namespace std;

// typedef mpz_class mpzc_t;
typedef unsigned long ul_t;
// typedef unsigned long long ull_t;
typedef vector<unsigned> vu_t;

static unsigned dbg_flags;

#if 0
static void ov(ostream &fo, const vu_t &v)
{
    fo << "[";
    const char *del = "";
    for (auto i = v.begin(), e = v.end(); i != e; ++i)
    {
        fo << del << *i;
        del = ", ";
    }
    fo << "]\n";
}
#endif

static void iota(vu_t& v, unsigned n, unsigned val=0)
{
    v.clear();
    v.reserve(n);
    for (unsigned k = 0; k < n; ++k, val++)
    {
        v.push_back(val);
    }
}

void permutation_first(vu_t &p, unsigned n)
{
    iota(p, n, 0);
}

bool permutation_next(vu_t &p)
{
    unsigned n = p.size();

    // find the last 'in order'
    int j = n - 1 - 1;
    while ((j >= 0) && (p[j] > p[j + 1]))
    {
        --j;
    }
    bool ret = j >= 0;
    if (ret)
    {
        // Find last > p[j]. Must find since p[j] < p[j+1]
        unsigned l;
        for (l = n - 1; p[j] >= p[l]; --l);
        swap(p[j], p[l]);
        reverse(p.begin() + j + 1, p.end());
    }
    return ret;
}

class PermRLE
{
 public:
    PermRLE(istream& fi);
    ~PermRLE()
    {
        delete [] cs;
        delete [] cs_alt;
    }
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    unsigned compress_size(const char *ccs, unsigned limit) const;
    void sperm(const vu_t &perm);
    void perm_sub_mirror(vu_t &perm, unsigned b, unsigned e) const;
    void perm_shift(vu_t &perm, unsigned b, unsigned e, unsigned t) const;
    unsigned k;
    string s;
    unsigned slen;;
    char *cs;
    char *cs_alt;
    unsigned solution;
};

PermRLE::PermRLE(istream& fi) : k(0), slen(0), cs(0), cs_alt(0), solution(0)
{
    fi >> k;
    fi >> s;
    slen = s.size();
    cs = new char[slen + 1];
    cs_alt = new char[slen + 1];
    strcpy(cs, s.c_str());
    strcpy(cs_alt, s.c_str());
}

void PermRLE::solve_naive()
{
    const unsigned slen = s.size();
    vu_t perm;
    permutation_first(perm, k);
    unsigned best = compress_size(cs_alt, slen);
    while (permutation_next(perm))
    {
        for (unsigned ci = 0; ci < slen; )
        {
            unsigned ci0 = ci;
            for (unsigned pi = 0; pi < k; ++pi, ++ci)
            {
                cs_alt[ci0 + perm[pi]] = cs[ci];
            }
        }
        unsigned csize = compress_size(cs_alt, best);
        if (best > csize)
        {
            best = csize;
        }
    }
    solution = best;
}

void PermRLE::solve()
{
    vu_t perm0, perm;
    permutation_first(perm0, k);
    unsigned best = compress_size(cs_alt, slen);
    if (dbg_flags & 0x1) { cerr << "Initial best=" << best << "\n"; }
    bool reducing = true;
    while (reducing)
    {
        reducing = false;
        // sub-mirrors
        for (unsigned b = 0; b < k; ++b)
        {
            unsigned b1 = (b + 1) % k;
            for (unsigned e = (b + 2) % k; e != b1; e = (e + 1) % k)
            {
                perm = perm0;
                perm_sub_mirror(perm, b, e);
                sperm(perm);
                unsigned csize = compress_size(cs_alt, best);
                if (best > csize)
                {
                    reducing = true;
                    best = csize;
                    swap(cs, cs_alt);
                    if (dbg_flags & 0x1) {
                        // ov(cerr, perm);
                        cerr << "mirror-reducing: best=" << best << "\n"; }
                }
            }
        }

        // 3-cycles
        for (unsigned i = 0; i < k; ++i)
        {
            for (unsigned j = 0; j < k; ++j)
            {
                for (unsigned y = (i != j ? 0 : k); y < k; ++y)
                {
                    if ((i != y) && (j != y))
                    {
                        perm = perm0;
                        perm[i] = j;
                        perm[j] = y;
                        perm[y] = i;
                        unsigned csize = compress_size(cs_alt, best);
                        if (best > csize)
                        {
                            reducing = true;
                            best = csize;
                            swap(cs, cs_alt);
                            if (dbg_flags & 0x1) {
                                // ov(cerr, perm);
                                cerr << "3cycle-reducing: best=" << best <<"\n";
                            }
                        }
                    }
                }
            }
        }
        
        // swap segments
        for (unsigned b = 0; b < k; ++b)
        {
            for (unsigned e = (b + 1) % k; e != b; e = (e + 1) % (k + 1))
            {
                for (unsigned t = (b + 1) % k; t != b; t = (t + 1) % k)
                {
                    for (unsigned sm = 0; sm != 4; ++sm)
                    {
                        perm = perm0;
                        if (sm & 0x1)
                        {
                            perm_sub_mirror(perm, b, e);
                        }
                        perm_shift(perm, b, e, t);
                        if (sm & 0x2)
                        {
                            perm_sub_mirror(perm, t, (t + e + (k - b)) % k);
                        }
                        sperm(perm);
                        unsigned csize = compress_size(cs_alt, best);
                        if (best > csize)
                        {
                            reducing = true;
                            best = csize;
                            swap(cs, cs_alt);
                            if (dbg_flags & 0x1) {
                                // ov(cerr, perm);
                                cerr << "shift-reducing: best=" << best << "\n";
                            }
                        }
                    }
                }
            }
        }
    }
    solution = best;
}

void PermRLE::print_solution(ostream &fo) const
{
    fo << " " << solution;
}

unsigned PermRLE::compress_size(const char *ccs, unsigned limit) const
{
    char clast = ' ';
    unsigned ret = 0;
    char c;
    for (const char *pc = ccs; (c = *pc) && (ret < limit); ++pc)
    {
        if (clast != c)
        {
            ++ret;
            clast = c;
        }
    }
    return ret;
}

void PermRLE::sperm(const vu_t &perm)
{
    for (unsigned ci = 0; ci < slen; )
    {
        unsigned ci0 = ci;
        for (unsigned pi = 0; pi < k; ++pi, ++ci)
        {
            cs_alt[ci0 + perm[pi]] = cs[ci];
        }
    }
}

void PermRLE::perm_sub_mirror(vu_t &perm, unsigned b, unsigned e) const
{
    unsigned sz = perm.size();
    unsigned i = b, j = (e + sz - 1) % sz;
    bool mirror = true;
    while (mirror)
    {
        swap(perm[i], perm[j]);
        mirror = (i + 1 != j) && (i != j + 1);
        i = (i + 1) % sz;
        j = (j + (sz - 1)) % sz;
        mirror = mirror && (i != j);
    }
}

void PermRLE:: perm_shift(vu_t &perm, unsigned b, unsigned e, unsigned t) const
{
    unsigned sz = perm.size();
    e = e % sz;
    for (unsigned i = b, j = t; i != e; i = (i + 1) % sz, j = (j + 1) % sz)
    {
        swap(perm[i], perm[j]);
    }
}

int main(int argc, char ** argv)
{
    const string dash("-");

    bool naive = false;
    bool tellg = false;
    int rc = 0, ai;

    for (ai = 1; (rc == 0) && (argv[ai][0] == '-') && argv[ai][1] != '\0'; ++ai)
    {
        const string opt(argv[ai]);
        if (opt == string("-naive"))
        {
            naive = true;
        }
        else if (opt == string("-debug"))
        {
            dbg_flags = strtoul(argv[++ai], 0, 0);
        }
        else if (opt == string("-tellg"))
        {
            tellg = true;
        }
    }
    
    int ai_in = ai;
    int ai_out = ai + 1;
    istream *pfi = (argc <= ai_in || (string(argv[ai_in]) == dash))
         ? &cin
         : new ifstream(argv[ai_in]);
    ostream *pfo = (argc <= ai_out || (string(argv[ai_out]) == dash))
         ? &cout
         : new ofstream(argv[ai_out]);

    if ((!pfi) || (!pfo) || pfi->fail() || pfo->fail())
    {
        cerr << "Open file error\n";
        exit(1);
    }

    unsigned n_cases;
    *pfi >> n_cases;

    void (PermRLE::*psolve)() =
        (naive ? &PermRLE::solve_naive : &PermRLE::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        PermRLE permrle(*pfi);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << ", " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (permrle.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        permrle.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
