// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <vector>
#include <utility>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<ul_t> vul_t;

static unsigned dbg_flags;

class Hindex
{
 public:
    Hindex(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    ul_t n;
    vul_t a;
    vul_t solution;
};

Hindex::Hindex(istream& fi) : solution(0)
{
    fi >> n;
    a = vul_t(vul_t::size_type(n), 0);
    for (ul_t i = 0; i < n; ++i)
    {
        fi >> a[i];
    }
}

void Hindex::solve_naive()
{
    solution = vul_t(vul_t::size_type(n), 0);
    for (ul_t i = 0; i < n; ++i)
    {
        ul_t hindex = 0;
        for (ul_t h = 1; h <= i + 1; ++h)
        {
            ul_t hn = 0;
            for (ul_t p = 0; (p <= i) && (hn < h); ++p)
            {
                if (a[p] >= h)
                {
                    ++hn;
                }
            }
            if (hindex < hn)
            {
                hindex = hn;
            }             
        }
        solution[i] = hindex;
    }
}

void Hindex::solve()
{

    vul_t as;
    solution = vul_t(vul_t::size_type(n), 0);
    multiset<ul_t> aset;
    as.reserve(n);
    for (ul_t i = 0, sz = 1; i < n; i = sz++)
    {
        aset.insert(aset.end(), a[i]);
        as = vul_t(aset.begin(), aset.end());
        // find higest i  [0, sz), such that
        // as[i] <= sz - i
        ul_t low = 1, high = sz + 1;
        while (low + 1 < high)
        {
            ul_t mid = (low + high)/2;
            if (as[i + 1 - mid] >= mid)
            {
                low = mid;
            }
            else
            {
                high = mid;
            }
        }
        solution[i] = low;
    }
}

void Hindex::print_solution(ostream &fo) const
{
    for (ul_t i = 0; i < n; ++i)
    {
        fo << ' ' << solution[i];
    }
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

    void (Hindex::*psolve)() =
        (naive ? &Hindex::solve_naive : &Hindex::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Hindex hindex(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (hindex.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        hindex.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
