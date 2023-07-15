// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <array>
#include <string>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef long long ll_t;
typedef array<ll_t, 2> all2_t;
typedef vector<all2_t> vall2_t;
typedef vector<ll_t> vll_t;
typedef vector<vll_t> vvll_t;

static unsigned dbg_flags;

class Ccomp
{
 public:
    Ccomp(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    size_t n, m;
    ll_t h;
    vvll_t t;
    vall2_t scores;
    int solution;
};

Ccomp::Ccomp(istream& fi) : solution(0)
{
    fi >> n >> m >> h;
    t.reserve(n);
    while (t.size() < n)
    {
        t.push_back(vll_t());
        copy_n(istream_iterator<ll_t>(fi), m, back_inserter(t.back()));
    }
}

void Ccomp::solve_naive()
{
    scores.reserve(n);
    for (const vll_t& tp: t)
    {
        vll_t ts(tp);
        sort(ts.begin(), ts.end());
        ll_t solved = 0;
        ll_t penalty = 0;
        ll_t tcurr = 0;
        for (size_t i = 0; (i < m) && (tcurr < h); ++i)
        {
            if (tcurr + ts[i] <= h)
            {
                ++solved;
                penalty += tcurr + ts[i];
            }
            tcurr += ts[i];
        }
        scores.push_back(all2_t{solved, -penalty});
    }
    const all2_t rudolf = scores[0];
    sort(scores.begin(), scores.end());
    auto iter = upper_bound(scores.begin(), scores.end(), rudolf);
    size_t pos = scores.end() - iter;
    solution = pos + 1;
}

void Ccomp::solve()
{
    solve_naive();
}

void Ccomp::print_solution(ostream &fo) const
{
    fo << solution << '\n';
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

    void (Ccomp::*psolve)() =
        (naive ? &Ccomp::solve_naive : &Ccomp::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Ccomp ccomp(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (ccomp.*psolve)();
        ccomp.print_solution(fout);
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
