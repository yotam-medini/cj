// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <iterator>
#include <fstream>
#include <array>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
typedef array<u_t, 2> au2_t;
typedef vector<au2_t> vau2_t;

static unsigned dbg_flags;

class Repkeep
{
 public:
    Repkeep(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    u_t n, q, k;
    vu_t a;
    vau2_t queries;
    vu_t solution;
};

Repkeep::Repkeep(istream& fi) : solution(0)
{
    fi >> n >> q >> k;
    copy_n(istream_iterator<u_t>(fi), n, back_inserter(a));
    queries.reserve(q);
    for (u_t i = 0; i < q; ++i)
    {
        au2_t lr;
        fi >> lr[0] >> lr[1];
        queries.push_back(lr);
    }
}

void Repkeep::solve_naive()
{
    for (const au2_t& query: queries)
    {
        const u_t l = query[0] - 1;
        const u_t r = query[1] - 1;
        u_t nrep = a[l] - 1;
        for (u_t i = l; i < r; ++i)
        {
            u_t delta = a[i + 1] - a[i];
            if (delta > 1)
            {
                nrep += 2*(delta - 1);
            }
        }
        nrep += k - a[r];
        solution.push_back(nrep);
    }
}

void Repkeep::solve()
{
    for (const au2_t& query: queries)
    {
        const u_t l = query[0] - 1;
        const u_t r = query[1] - 1;
        const u_t low = a[l] - 1;
        const u_t in_free = (a[r] - a[l] - 1) - (r - l - 1);
        const u_t high = k - a[r];
        const u_t s = low + 2*in_free + high;
        solution.push_back(s);
    }    

}

#if 0
void Repkeep::solve()
{
    solve_naive();
    vu_t reps;
    reps.push_back(0);
    u_t delta = (n > 1 ? a[1] - a[0] : 0);
    u_t rep = (a[0] - 1) + delta;
    reps.push_back(rep);
    for (u_t i = 1; i < n; ++i)
    {
        delta = (a[i] - a[i - 1]) - 1;
        rep = delta;
        delta = (i + 1 < n ? a[i + 1] : k) - a[i];
        rep += delta;
        reps.push_back(rep);
    }
    for (const au2_t& query: queries)
    {
        const u_t l = query[0] - 1;
        const u_t r = query[1];
        const u_t s = reps[r] - reps[l];
        solution.push_back(s);
    }    
}
#endif

void Repkeep::print_solution(ostream &fo) const
{
    for (u_t s: solution)
    {
        fo << s << '\n';
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
    unsigned n_cases = 1;

    void (Repkeep::*psolve)() =
        (naive ? &Repkeep::solve_naive : &Repkeep::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Repkeep repkeep(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (repkeep.*psolve)();
        repkeep.print_solution(fout);
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
