// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <iostream>
#include <fstream>
#include <string>
#include <set>
// #include <map>
#include <vector>
#include <utility>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef set<u_t> setu_t;
typedef vector<u_t> vu_t;
typedef vector<ull_t> vull_t;
typedef vector<bool> vb_t;
typedef vector<setu_t> vsetu_t;

static unsigned dbg_flags;

class Lguests
{
 public:
    Lguests(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    u_t n, g, m;
    vb_t cw;
    vull_t h;
    vull_t solution;
    vsetu_t last;
};

Lguests::Lguests(istream& fi)
{
    fi >> n >> g >> m;
    cw.reserve(g);
    h.reserve(g);
    u_t x;
    for (u_t i = 0; i < g; ++i)
    {
        char c;
        fi >> x >> c;
        // fi.getline() ?
        if (c != 'C' && c != 'A')
        {
             cerr << "bad c="<<c << "\n";
             exit(1);
        }
        cw.push_back(c == 'C');
        h.push_back(x - 1);
    }
}

void Lguests::solve_naive()
{
    last = vsetu_t(vsetu_t::size_type(n), setu_t());
    for (u_t step = 0; step <= m; ++step)
    {
        vsetu_t occ = vsetu_t(vsetu_t::size_type(n), setu_t());
        for (u_t gi = 0; gi < g; ++gi)
        {
            u_t hgi = (h[gi] + (step % n) * (cw[gi] ? 1 : n - 1)) % n;
            occ[hgi].insert(gi);
        }
        for (u_t ci = 0; ci < n; ++ci)
        {
            if (!occ[ci].empty())
            {
                swap(last[ci], occ[ci]);
            }
        }
    }
    solution.reserve(g);
    for (u_t gi = 0; gi < g; ++gi)
    {
        u_t remember = 0;
        for (const setu_t& s: last)
        {
            if (s.find(gi) != s.end())
            {
                ++remember;                
            }
        }
        solution.push_back(remember);
    }
}

void Lguests::solve()
{
    solve_naive();
}

void Lguests::print_solution(ostream &fo) const
{
    for (u_t remember: solution)
    {
        fo << ' ' << remember;
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

    void (Lguests::*psolve)() =
        (naive ? &Lguests::solve_naive : &Lguests::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Lguests lguests(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (lguests.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        lguests.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
