// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <iterator>
#include <vector>
#include <utility>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
typedef vector<ull_t> vull_t;

static unsigned dbg_flags;

class OversizedPancake
{
 public:
    OversizedPancake(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    bool cuts_can();
    bool can_val_add(ull_t v, u_t need);
    bool can_add(u_t need);
    bool can_val_advance(ull_t v, u_t need, u_t ai, u_t pending);
    u_t N, D;
    vull_t a;
    vull_t sa;
    vu_t pans_cuts;
    u_t solution;
};

OversizedPancake::OversizedPancake(istream& fi) : solution(0)
{
    fi >> N >> D;
    copy_n(istream_iterator<ull_t>(fi), N, back_inserter(a));
}

void OversizedPancake::solve_naive()
{
    if (N == 1)
    {
        solution = D - 1;
    }
    else if (D > 1)
    {
        sa = a;
        sort(sa.begin(), sa.end());
        pans_cuts = vu_t(size_t(N), 0);
        for (solution = 0; !cuts_can(); ++solution)
        {
            if (dbg_flags & 0x1) { cerr <<"tried solution="<<solution<<'\n'; }
        }
    }
}

bool OversizedPancake::cuts_can()
{
    bool can = false;
    // select pre-size
    for (u_t i = 0, j = 0; (i < N) && !can; i = j)
    {
        const ull_t v = sa[i];
        for (j = i + 1; (j < N) && (sa[j] == v); ++j) {}
        u_t nv = j - i;
        can = (D <= nv);
        can = can || can_val_add(v, D - nv);
    }
    can = can || can_add(D - solution);
    return can;
}

bool OversizedPancake::can_val_add(ull_t v, u_t need)
{
    bool can = can_val_advance(v, need, 0, solution);
    return can;
}

bool OversizedPancake::can_val_advance(ull_t v, u_t need, u_t ai, u_t pending)
{
    bool can = false;
    if (pending == 0)
    {
        u_t made = 0;
        for (u_t i = 0; (i < N) && !can; ++i)
        {
            const u_t pc = pans_cuts[i];
            if (pc > 0)
            {
                u_t q = sa[i] / v;
                u_t cuts = (sa[i] % v == 0 ? q - 1 :  q);
                u_t got = (pc >= cuts ? q : min(q, pc));
                made += got;
                can = (made >= need);
            }
        }
    }
    else
    {
        for (; (ai < N) && sa[ai] == v; ++ai)
        {
            pans_cuts[ai] = 0;
        }
        if (ai < N)
        {
            for (u_t cuts = 0; (cuts <= pending) && !can; ++cuts)
            {
                pans_cuts[ai] = cuts;
                can = can_val_advance(v, need, ai + 1, pending - cuts);
            }
        }
    }
    return can;
}

bool OversizedPancake::can_add(u_t need)
{
    bool can = false;
    return can;
}

void OversizedPancake::solve()
{
    solve_naive();
}

void OversizedPancake::print_solution(ostream &fo) const
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

    void (OversizedPancake::*psolve)() =
        (naive ? &OversizedPancake::solve_naive : &OversizedPancake::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        OversizedPancake overpan(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (overpan.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        overpan.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
