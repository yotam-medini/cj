// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <iostream>
#include <fstream>
#include <string>
// #include <set>
// #include <map>
#include <numeric>
#include <vector>
#include <utility>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
typedef vector<ul_t> vul_t;

static unsigned dbg_flags;

void combination_first(vu_t &c, u_t n, u_t k)
{
    c = vu_t(k);
    iota(c.begin(), c.end(), 0);
}

bool combination_next(vu_t &c, u_t n)
{
    u_t j = 0, k = c.size();

    // sentinels (Knuth) (Why the second (0) ??!)
    c.push_back(n);  c.push_back(0);

    while ((j < k) && (c[j] + 1 == c[j + 1]))
    {
        c[j] = j;
        ++j;
    }
    bool ret = j < k;
    if (ret)
    {
        ++(c[j]);
    }

    c.pop_back(); c.pop_back(); // the sentinels

    return ret;
}

class Fstall
{
 public:
    Fstall(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    ull_t comb_price(const vu_t &comb) const;
    u_t k, n;
    vul_t x;
    vul_t c;
    ull_t solution;
};

Fstall::Fstall(istream& fi) : solution(0)
{
    fi >> k >> n;
    ull_t v;
    x.reserve(n);
    c.reserve(n);
    for (u_t i = 0; i < n; ++i)
    {
        fi >> v;
        x.push_back(v);
    }
    for (u_t i = 0; i < n; ++i)
    {
        fi >> v;
        c.push_back(v);
    }
}

void Fstall::solve_naive()
{
    vu_t comb;
    combination_first(comb, n, k + 1);
    
    ull_t best_price = comb_price(comb);
    while (combination_next(comb, n))
    {
        ull_t price = comb_price(comb);
        if (best_price > price)
        {
            best_price = price;
        }
    }
    solution = best_price;
}

void Fstall::solve()
{
    solve_naive();
}

ull_t Fstall::comb_price(const vu_t &comb) const
{
    ull_t base = 0;
    for (u_t pi: comb)
    {
        base += c[pi];
    }
    ull_t smin_distance(-1);
    for (u_t p: comb)
    {
        ull_t xp = x[p];
        ull_t sdist = 0;
        for (u_t q: comb)
        {
            ull_t xq = x[q];
            ull_t delta = (xp < xq ? xq - xp : xp - xq);
            sdist += delta;
        }
        if (smin_distance > sdist)
        {
            smin_distance = sdist;
        }
    }
    ull_t ret = base + smin_distance;
    return ret;
}

void Fstall::print_solution(ostream &fo) const
{
    fo << ' ' << solution;
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

    void (Fstall::*psolve)() =
        (naive ? &Fstall::solve_naive : &Fstall::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Fstall fstall(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (fstall.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        fstall.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
