// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef long long ll_t;
// typedef vector<ul_t> vul_t;
class Athelete {
 public:
    Athelete(ll_t s=0, ll_t e=0) : s_(s), e_(e) {}
    ll_t s_, e_;
};
typedef vector<Athelete> vathelte_t;
bool operator<(const Athelete& a0, const Athelete& a1) {
    return (a0.s_ < a1.s_) || ((a0.s_ == a1.s_) && (a0.e_ < a1.e_));
}

static unsigned dbg_flags;

class Arigged
{
 public:
    Arigged(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    size_t n;
    vathelte_t atheletes;
    ll_t solution;
};

Arigged::Arigged(istream& fi) : solution(-1)
{
    fi >> n;
    atheletes.reserve(n);
    while (atheletes.size() < n) {
        ll_t s, e;
        fi >> s >> e;
        atheletes.push_back(Athelete{s, e});
    }
}

void Arigged::solve_naive()
{
    bool possible = true;
    for (size_t i = 1; possible && (i < n); ++i) {
        possible =
            (atheletes[i].s_ < atheletes[0].s_) ||
            (atheletes[i].e_ < atheletes[0].e_);
    }
    solution = possible ? atheletes[0].s_ : -1;
}

void Arigged::solve()
{
    solve_naive();
}

void Arigged::print_solution(ostream &fo) const
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

    void (Arigged::*psolve)() =
        (naive ? &Arigged::solve_naive : &Arigged::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Arigged arigged(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (arigged.*psolve)();
        arigged.print_solution(fout);
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
