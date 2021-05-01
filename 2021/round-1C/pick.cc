// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
// #include <map>
// #include <set>
#include <string>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<ul_t> vul_t;

static unsigned dbg_flags;

string dtos(const double x, u_t precision=6)
{
    char buf[0x20];
    snprintf(buf, sizeof(buf), "%.*f", precision, x);
    string ret(buf);
    return ret;
}

class Pick
{
 public:
    Pick(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    u_t get_n_wins(const u_t p0, const u_t p1) const;
    u_t N, K;
    vul_t sold;
    vul_t ssold;
    double solution;
};

Pick::Pick(istream& fi) : solution(0.)
{
    fi >> N >> K;
    copy_n(istream_iterator<ul_t>(fi), N, back_inserter(sold));
}

void Pick::solve_naive()
{
    ssold = sold;
    sort(ssold.begin(), ssold.end());
    u_t n_win_max = 0;
    u_t p0_best = 0, p1_best = 0;
    for (u_t p0 = 1; p0 <= K; ++p0)
    {
        for (u_t p1 = p0 + 1; p1 <= K; ++p1)
        {
            u_t nw = get_n_wins(p0, p1);
            if (n_win_max < nw)
            {
                n_win_max = nw;
                p0_best = p0;
                p1_best = p1;
            }
        }
    }
    if (dbg_flags & 0x1) { cerr << "best: p0="<<p0_best << ", p1="<<p1_best <<
        ", n_win_max=" << n_win_max << ", K="<<K << '\n'; }
    solution = double(n_win_max) / double(K);
}

void Pick::solve()
{
    ssold = sold;
    sort(ssold.begin(), ssold.end());
    vul_t wins; wins.reserve(ssold.size());
    u_t dmax = 0;
    for (u_t i = 1, inext = 2; i < N; i = inext)
    {
        u_t d = ssold[i] - ssold[i - 1];
        if (dmax < d)
        {
            dmax = d;
        }
        u_t w = d/2;
        wins.push_back(w);
        for (inext = i; (inext < N) && (ssold[inext] == ssold[i]); ++inext)
        {
            ;
        }
    }
    wins.push_back(ssold[0] - 1);
    wins.push_back(K - ssold.back());
    sort(wins.begin(), wins.end());
    size_t wsz = wins.size();
    u_t n_wins = wins[wsz - 2] + wins[wsz - 1];
    if (n_wins + 1 < dmax)
    {
        n_wins = dmax - 1;
    }
    if (dbg_flags & 0x1) {
        cerr << "dmax="<<dmax << ", n_wins=" << n_wins << '\n'; }
    solution = double(n_wins) / double(K);
}

u_t Pick::get_n_wins(const u_t p0, const u_t p1) const
{
    u_t n_win = 0;
    for (u_t x = 1; x <= K; ++x)
    {
        u_t sold_delta = K + 1;
        for (u_t t: ssold)
        {
            const u_t d = (x < t ? t - x : x - t);
            if (sold_delta > d)
            {
                sold_delta = d;
            }
        }

        u_t my_delta = K + 1;
        for (u_t px: {p0, p1})
        {
            const u_t d = (px < x ? x - px : px - x);
            if (my_delta > d)
            {
                my_delta = d;
            }
        }

        if ((my_delta < sold_delta) ||
            ((my_delta == sold_delta) && (p0 == p1)))
        {
            ++n_win;
        }
    }
    if (dbg_flags & 0x2) { cerr << "p0="<<p0 << ", p1="<<p1 <<
        ", n_win=" << n_win << '\n'; }
    return n_win;
}

void Pick::print_solution(ostream &fo) const
{
   fo << ' ' << dtos(solution);
}

int main(int argc, char ** argv)
{
    const string dash("-");

    bool naive = false;
    bool tellg = false;
    u_t solve_ver = 0;
    int rc = 0, ai;

    for (ai = 1; (rc == 0) && (ai < argc) && (argv[ai][0] == '-') &&
        argv[ai][1] != '\0'; ++ai)
    {
        const string opt(argv[ai]);
        if (opt == string("-naive"))
        {
            naive = true;
        }
        else if (opt == string("-solve1"))
        {
            solve_ver = 1;
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

    void (Pick::*psolve)() =
        (naive ? &Pick::solve_naive : &Pick::solve);
    if (solve_ver == 1) { psolve = &Pick::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Pick pick(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (pick.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        pick.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
