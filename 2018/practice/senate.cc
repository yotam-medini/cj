// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2018/March/31

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <utility>

#include <cstdlib>

using namespace std;

typedef unsigned long ul_t;
typedef vector<unsigned> vu_t;
typedef pair<unsigned, unsigned> uu_t;
typedef priority_queue<uu_t> pq_t;

static unsigned dbg_flags;

class Senate
{
 public:
    Senate(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    vu_t parties;
    vu_t solution;
};

Senate::Senate(istream& fi)
{
    unsigned n;
    fi >> n;
    parties.reserve(n);
    for (unsigned i = 0; i < n; ++i)
    {
        unsigned psz;
        fi >> psz;
        parties.push_back(psz);
    }
}

void Senate::solve_naive()
{
    pq_t pq;
    for (unsigned i = 0, n = parties.size(); i != n; ++i)
    {
        unsigned sz = parties[i];
        pq.push(uu_t(sz, i));
    }

    while (!pq.empty())
    {
        const uu_t &top = pq.top();
        unsigned i = top.second;
        solution.push_back(i);
        unsigned sz = --parties[i];
        pq.pop();
        if (sz > 0)
        {
            pq.push(uu_t(sz, i));
        }
    }
}


void Senate::solve()
{
    solve_naive();
}

void Senate::print_solution(ostream &fo) const
{
    unsigned n = solution.size();
    bool even = (n % 2) == 0;
    bool first_loop = true;
    for (unsigned i = 0; i < n; )
    {
        static const char *upcase = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
        unsigned pi0 = solution[i++];
        fo << ' ' << upcase[pi0];
        bool second = (even || !first_loop);
        if (second)
        {
            unsigned pi1 = solution[i++];
            fo << upcase[pi1];
        }
        first_loop = false;
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

    void (Senate::*psolve)() =
        (naive ? &Senate::solve_naive : &Senate::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Senate senate(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (senate.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        senate.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
