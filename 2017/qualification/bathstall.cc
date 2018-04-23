// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <fstream>
#include <string>
#include <set>
// #include <map>
// #include <vector>
#include <utility>

#include <cstdlib>
// #include <gmpxx.h>

using namespace std;

// typedef mpz_class mpzc_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
// typedef vector<ul_t> vul_t;
typedef set<ull_t> set_ull_t;

class Space
{
 public:
    Space(ull_t vlength=0, ull_t vb=0) : length(vlength), b(vb) {}
    ull_t length;
    ull_t b;
};

bool operator<(const Space &s0, const Space &s1)
{
    bool lt = (s0.length > s1.length) ||
        ((s0.length == s1.length) && (s0.b < s1.b));
    return lt;
}

typedef set<Space> set_space_t;

static unsigned dbg_flags;

class BathStall
{
 public:
    BathStall(istream& fi);
    void solve_naive();
    void solve();
    void solve_geo();
    void print_solution(ostream&) const;
 private:
    void be_slr(ull_t b, ull_t e, ull_t &s, ull_t &l, ull_t &r);
    void slr_imrove(ull_t &s, ull_t &l, ull_t &r,
                    ull_t sc, ull_t &lc, ull_t &rc);
    ull_t n;
    ull_t k;

    ull_t solution_max, solution_min;
};

BathStall::BathStall(istream& fi) : solution_max(0), solution_min(0)
{
    fi >> n >> k;
}

void BathStall::be_slr(ull_t b, ull_t e, ull_t &s, ull_t &l, ull_t &r)
{
    ull_t space = e - b;
    if (space == 0)
    {
        s = e; // undef
        l = 0;
        r = 0;
    }
    else
    {
        ull_t half = (space - 1) / 2;
        s = b + half;
        l = s - b;
        r = e - s - 1;
    }
}

void BathStall::slr_imrove(
    ull_t &s, ull_t &l, ull_t &r,
    ull_t sc, ull_t &lc, ull_t &rc)
{
    if ((s == n) || (l < lc) || ((l == lc) && (r < rc)))
    {
        s = sc;
        l = lc;
        r = rc;
    }
}

void BathStall::solve_naive()
{
    set_ull_t taken;
    ull_t l = 0, r = 0; // undef?

    for (unsigned p = 0; p < k; ++p)
    {
        ull_t s = n; // undef
        if (taken.empty())
        {
            be_slr(ull_t(0), ull_t(n), s, l, r);
        }
        else
        {
            ull_t scandid, s_l, s_r;
            set_ull_t::iterator tb = taken.begin();
            set_ull_t::iterator te = taken.end();
            ull_t tfirst = *tb;
            ull_t tlast = *taken.rbegin();
            
            if (tfirst > 0)
            {
                be_slr(ull_t(0), tfirst, s, l, r);
            }

            set_ull_t::iterator ti(tb);
            set_ull_t::iterator tnext(ti);
            for (++tnext; tnext != te; ti = tnext, ++tnext)
            {
                be_slr(*ti + 1, *tnext, scandid, s_l, s_r);
                slr_imrove(s, l, r, scandid, s_l, s_r);
            }

            if (tlast < n)
            {
                be_slr(tlast + 1, n, scandid, s_l, s_r);
                slr_imrove(s, l, r, scandid, s_l, s_r);
            }
        }
        
        taken.insert(taken.end(), s);
    }
    solution_max = max(l, r);
    solution_min = min(l, r);
}

void BathStall::solve()
{
    ull_t s, l, r;
    set_space_t spaces;
    spaces.insert(Space(n, 0));
    for (unsigned p = 0; p < k; ++p)
    {
        set_space_t::iterator w = spaces.begin();
        const Space &space = *w;
        ull_t b = space.b;
        ull_t e = b + space.length;
        be_slr(b, e, s, l, r);
        spaces.erase(w);

        ull_t l_len = s - b;
        if (l_len > 0)
        {
            spaces.insert(Space(l_len, b));
        }

        ull_t r_len = e - (s + 1);
        if (r_len > 0)
        {
            spaces.insert(Space(r_len, s + 1));
        }
    }
    solution_max = max(l, r);
    solution_min = min(l, r);
}

void BathStall::solve_geo()
{
    ull_t g = 0, gadd = 1;
    while (g + gadd <= k - 1)
    {
        g += gadd;
        gadd *= 2;
    }
    ull_t n_spaces = g + 1;
    ull_t residue = (n - g) % n_spaces;
    ull_t space_size = (n - g) / n_spaces;
    if (k - g <= residue) 
    {
        ++space_size;
    }
    if (space_size > 0)
    {
        --space_size;
    }
    ull_t r = space_size/2;
    ull_t l = space_size - r;
    solution_max = max(l, r);
    solution_min = min(l, r);
    
}

void BathStall::print_solution(ostream &fo) const
{
    fo << ' ' << solution_max << ' ' << solution_min;
}

int main(int argc, char ** argv)
{
    const string dash("-");

    unsigned n_opts = 0;
    bool naive = false;
    bool geo = false;

    if ((argc > 1) && (string(argv[1]) == "-naive"))
    {
        naive = true;
        n_opts = 1;
    }
    if ((argc > 1) && (string(argv[1]) == "-geo"))
    {
        geo = true;
        n_opts = 1;
    }
    int ai_in = n_opts + 1;
    int ai_out = ai_in + 1;
    int ai_dbg = ai_out + 1;
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

    if (ai_dbg < argc) { dbg_flags = strtoul(argv[ai_dbg], 0, 0); }

    unsigned n_cases;
    *pfi >> n_cases;

    void (BathStall::*psolve)() =
        (naive ? &BathStall::solve_naive : &BathStall::solve);
    if (geo)
    {
        psolve = &BathStall::solve_geo;
    }

    ostream &fout = *pfo;
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        if (getenv("TELLG"))
        {
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg="<<pfi->tellg() << "\n";
        }
        BathStall problem(*pfi);
        (problem.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        problem.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
