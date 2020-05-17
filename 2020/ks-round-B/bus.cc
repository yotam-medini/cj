// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <utility>
#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<ull_t> vull_t;

static unsigned dbg_flags;

class Bus
{
 public:
    Bus(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    bool can(ul_t start) const;
    u_t n;
    ull_t d;
    vull_t x;
    ull_t solution;
};

Bus::Bus(istream& fi) : solution(0)
{
    fi >> n >> d;
    x.reserve(n);
    for (u_t i = 0; i < n; ++i)
    {
        ull_t xi;
        fi >> xi;
        x.push_back(xi);
    }
}

void Bus::solve_naive()
{
    ull_t dlow = 1, dhigh = d + 1;
    while (dlow + 1 < dhigh)
    {
        ull_t dm = (dlow + dhigh)/2;
        if (can(dm))
        {
            dlow = dm;
        }
        else
        {
            dhigh = dm;
        }
    }
    solution = dlow;
}

void Bus::solve()
{
    solve_naive();
}

bool Bus::can(ul_t start) const
{
    ul_t t = start;
    for (u_t i = 0; i < n; ++i)
    {
        ull_t p = x[i];
        ull_t m = t % p;
        if (m != 0)
        {
            ull_t wait = p - m;
            t += wait;
        }
    }
    bool ret = (t <= d);
    return ret;
}

void Bus::print_solution(ostream &fo) const
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

    void (Bus::*psolve)() =
        (naive ? &Bus::solve_naive : &Bus::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Bus bus(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (bus.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        bus.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
