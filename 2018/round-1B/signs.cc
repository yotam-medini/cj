// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <fstream>
#include <string>
#include <set>
// #include <map>
#include <vector>
#include <utility>

#include <cstdlib>
// #include <gmpxx.h>

using namespace std;

// typedef mpz_class mpzc_t;
typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
// typedef vector<ul_t> vul_t;

typedef set<int> seti_t;

static unsigned dbg_flags;

class Sign
{
 public:
    Sign(int vD=0, int vA=0, int vB=0) : D(vD), A(vA), B(vB) {}
    int m() const { return D + A; }
    int n() const { return D - B; }
    int D, A, B;
};

typedef vector<Sign> vsign_t;

class Road
{
 public:
    Road(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    ul_t s;
    vsign_t signs;
    ul_t sol_len;
    ul_t sol_n;
};

Road::Road(istream& fi) : sol_len(0), sol_n(0)
{
    fi >> s;
    signs.reserve(s);
    for (u_t i = 0; i < s; ++i)
    {
        int d, a, b;
        fi >> d >> a >> b;
        signs.push_back(Sign(d, a, b));
    }
}

void Road::solve_naive()
{
    for (u_t sz = s; (sz > 0) && (sol_n == 0); --sz)
    {
        if (dbg_flags & 0x1) { cerr << "sz="<<sz << "\n"; }
        for (u_t b = 0, e = b + sz; e <= s; ++b, ++e)
        {
            seti_t mset, nset;
            for (u_t i = b; i != e; ++i)
            {
                const Sign &sign = signs[i];
                mset.insert(mset.end(), sign.m());
                nset.insert(nset.end(), sign.n());
            }
            bool found = false;
            for (seti_t::const_iterator mi = mset.begin(), me = mset.end();
                (mi != me) && !found; ++mi)
            {
                int m = *mi;
                for (seti_t::const_iterator ni = nset.begin(), ne = nset.end();
                    (ni != ne) && !found; ++ni)
                {
                    int n = *ni;
                    bool agree = true;
                    for (u_t i = b; (i != e) && agree; ++i)
                    {
                        const Sign &sign = signs[i];
                        int sm = sign.m();
                        int sn = sign.n();
                        agree = (sm == m) || (sn == n);
                    }
                    found = agree;                    
                    if (found)
                    {
                        ++sol_n;
                        sol_len = sz;
                    }
                }
            }
        }
    }
}

void Road::solve()
{
    solve_naive();
}

void Road::print_solution(ostream &fo) const
{
    fo << ' ' << sol_len << ' ' << sol_n;
}

int main(int argc, char ** argv)
{
    const string dash("-");

    bool naive = false;
    bool tellg = false;
    int rc = 0, ai;

    for (ai = 1; (rc == 0) && (ai < argc) && (argv[ai][0] == '-') &&
        argv[ai][1] != '\0'; ++ai)
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
        else
        {
            cerr << "Bad option: " << opt << "\n";
            return 1;
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

    string ignore;
    unsigned n_cases;
    *pfi >> n_cases;
    getline(*pfi, ignore);

    void (Road::*psolve)() =
        (naive ? &Road::solve_naive : &Road::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Road road(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (road.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        road.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
