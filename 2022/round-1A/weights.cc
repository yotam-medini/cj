// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <numeric>
#include <string>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
typedef vector<vu_t> vvu_t;
typedef vector<vvu_t> vvvu_t;

static unsigned dbg_flags;

class Weights
{
 public:
    Weights(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    void naive_next(vu_t& combs);
    ull_t naive_count(const vu_t& combs);
    void gen_combs(const vu_t& exer, vvu_t& combs) const;
    u_t E, W;
    vvu_t X;
    ull_t solution;
    vvvu_t exs_combs;
};

Weights::Weights(istream& fi) : solution(0)
{
    fi >> E >> W;
    X.reserve(E);
    for (u_t ei = 0; ei < E; ++ei)
    {
        vu_t ws;
        copy_n(istream_iterator<u_t>(fi), W, back_inserter(ws));
        X.push_back(ws);
    }
}

void Weights::solve_naive()
{
    solution = ull_t(-1);
    for (const vu_t& x: X)
    {
        vvu_t xcomb;
        gen_combs(x, xcomb);
        exs_combs.push_back(xcomb);
    }
    vu_t combs;
    naive_next(combs);
}

void Weights::naive_next(vu_t& icombs)
{
    const u_t sz = icombs.size();
    if (sz == E)
    {
        ull_t c = naive_count(icombs);
        if (solution > c)
        {
            solution = c;
        }
    }
    else
    {
        icombs.push_back(0);
        for (u_t i = 0, csz = exs_combs[sz].size(); i < csz; ++i)
        {
            icombs.back() = i;
            naive_next(icombs);
        }
        icombs.pop_back();
    }
}

void tuple_next(vu_t& t, const vu_t& bound)
{
    u_t i, sz = bound.size();
    bool done = false;
    for (i = 0; (i < sz) && !done; ++i)
    {
        ++t[i];
        done = (t[i] < bound[i]);
        if (!done)
        {
            fill_n(t.begin(), i + 1, 0);
        }
    }
    if (!done)
    {
        t.clear();
    }
}

void Weights::gen_combs(const vu_t& exer, vvu_t& combs) const
{
    combs.clear();
    const u_t n = accumulate(exer.begin(), exer.end(), 0u);
    vu_t bound(n, W);
    vu_t t(n, 0);
    for (; !t.empty(); tuple_next(t, bound))
    {
        vu_t counts(W, 0);
        for (u_t w: t)
        {
            ++counts[w];
        }
        if (counts == exer)
        {
            combs.push_back(t);
        }
    }
   
}

ull_t Weights::naive_count(const vu_t& combs)
{
    ull_t bcount = accumulate(X.front().begin(), X.front().end(), 0u);
    ull_t ecount = accumulate(X.back().begin(), X.back().end(), 0u);
    ull_t count = bcount;
    for (u_t ei = 0; ei + 1 < E; ++ei)
    {
        const u_t ia = combs[ei];
        const u_t ib = combs[ei + 1];
        const vu_t& a = exs_combs[ei][ia];
        const vu_t& b = exs_combs[ei + 1][ib];
        u_t sz_min = min(a.size(), b.size());
        u_t i_eq;
        for (i_eq = 0; (i_eq < sz_min) && (a[i_eq] == b[i_eq]); ++i_eq)
        {
            ;
        }
        ull_t rem_count = a.size() - i_eq;
        ull_t add_count = b.size() - i_eq;
        count += rem_count + add_count;
    }
    count += ecount;
    return count;
}

void Weights::solve()
{
    if ((E <= 10) && (W <= 3))
    {
        solve_naive();
    }
}

void Weights::print_solution(ostream &fo) const
{
    fo << ' ' << solution;
}

static int real_main(int argc, char ** argv)
{
    const string dash("-");

    bool naive = false;
    bool tellg = false;
    u_t solve_ver = 0;
    int rc = 0, ai;

    for (ai = 1; (rc == 0) && (ai < argc) && (argv[ai][0] == '-') &&
        argv[ai][1] != '\0'; ++ai)
    {
        const string opt(argv[ai]);
        if (opt == string("-naive"))
        {
            naive = true;
        }
        else if (opt == string("-solve1"))
        {
            solve_ver = 1;
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

    void (Weights::*psolve)() =
        (naive ? &Weights::solve_naive : &Weights::solve);
    if (solve_ver == 1) { psolve = &Weights::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Weights weights(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (weights.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        weights.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}

static int test_specific(int argc, char ** argv)
{
    int rc = 0;
    return rc;
}

static int test_random(int argc, char ** argv)
{
    int rc = 0;
    int ai = 0;
    u_t n_tests = strtoul(argv[ai++], 0, 0);
    for (u_t ti = 0; (rc == 0) && (ti < n_tests); ++ti)
    {
        cerr << "Tested: " << ti << '/' << n_tests << '\n';
    }
    return rc;
}

static int test(int argc, char ** argv)
{
    int rc = ((argc > 1) && (string(argv[1]) == string("specific"))
        ? test_specific(argc - 1, argv + 1)
        : test_random(argc, argv));
    return rc;
}

int main(int argc, char **argv)
{
    int rc = ((argc > 1) && (string(argv[1]) == string("test"))
        ? test(argc - 2, argv + 2)
        : real_main(argc, argv));
    return rc;
}
