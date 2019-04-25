// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <iostream>
#include <fstream>
#include <string>
// #include <set>
// #include <map>
#include <array>
#include <vector>
#include <utility>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef pair<u_t, u_t> uu_t;
typedef vector<uu_t> vuu_t;
typedef array<u_t, 26> u26_t;
typedef vector<u26_t> vu26_t;


static unsigned dbg_flags;

class Palin
{
 public:
    Palin(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    u_t n, q;
    string block;
    vuu_t questions;
    u_t solution;
    vu26_t counters;
};

Palin::Palin(istream& fi) : solution(0)
{
    fi >> n >> q;
    fi >> block;
    for (u_t i = 0; i < q; ++i)
    {
        u_t l, r;
        fi >> l >> r;
        questions.push_back(uu_t(l, r));
    }
}

void Palin::solve_naive()
{
    u_t az[26];
    for (u_t qi = 0; qi < q; ++qi)
    {
        fill_n(&az[0], 26, 0);
        const uu_t &quest = questions[qi];
        for (u_t si = quest.first - 1; si <= quest.second - 1; ++si)
        {
            char c = block[si];
            u_t azi = c - 'A';
            ++az[azi];
        }
        u_t n_odd = 0;
        for (u_t azi = 0; azi != 26; ++azi)
        {
            if ((az[azi] % 2) != 0)
            {
                ++n_odd;
            }
        }
        if (n_odd < 2)
        {
            ++solution;
        }
    }
}

void Palin::solve()
{
    counters.reserve(n + 1);
    u26_t counter;
    fill_n(&counter[0], 26, 0);
    counters.push_back(counter);
    for (u_t si = 0; si < n; ++si)
    {
        char c = block[si];
        u_t azi = c - 'A';
        ++counter[azi];
        counters.push_back(counter);
    }
    for (u_t qi = 0; qi < q; ++qi)
    {
        const uu_t &quest = questions[qi];
        u_t b = quest.first - 1;
        u_t e = quest.second;
        const u26_t &b_counter = counters[b];
        const u26_t &e_counter = counters[e];
        u_t n_odd = 0;
        for (u_t azi = 0; (azi != 26) && (n_odd < 2); ++azi)
        {
            u_t nc = e_counter[azi] - b_counter[azi];
            if ((nc % 2) != 0)
            {
                ++n_odd;
            }
        }
        if (n_odd < 2)
        {
            ++solution;
        }
    }
}

void Palin::print_solution(ostream &fo) const
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

    void (Palin::*psolve)() =
        (naive ? &Palin::solve_naive : &Palin::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Palin palin(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (palin.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        palin.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
