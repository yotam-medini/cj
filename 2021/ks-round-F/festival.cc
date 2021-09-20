// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
// #include <iterator>
// #include <map>
// #include <set>
#include <numeric>
#include <string>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;

static unsigned dbg_flags;

class Attraction
{
 public:
   Attraction(u_t _h=0, u_t _s=0, u_t _e=0) : h(_h), s(_s), e(_e) {}
   u_t h, s, e;
};
typedef vector<Attraction> vattr_t;

bool operator<(const Attraction& a0, const Attraction& a1)
{
    return a0.h < a1.h;
}

class Festival
{
 public:
    Festival(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    u_t D, N, K;
    vattr_t attractions;
    ull_t solution;
};

Festival::Festival(istream& fi) : solution(0)
{
    fi >> D >> N >> K;
    attractions.reserve(N);
    for (u_t i = 0; i < N; ++i)
    {
        u_t h, s, e;
        fi >> h >> s >> e;
        Attraction a(h, s, e);
        attractions.push_back(a);
    }
}

void Festival::solve_naive()
{
    int best_day = -1;
    for (ull_t d = 1; d <= D; ++d)
    {
        vu_t h;
        for (const Attraction& a: attractions)
        {
            if ((a.s <= d) && (d <= a.e))
            {
                h.push_back(a.h);
            }
        }
        sort(h.begin(), h.end());
        u_t hsz = h.size();
        u_t hu = min(hsz, K);
        ull_t hbest = 0;
        for (u_t i = hsz - hu; i < hsz; ++i)
        {
            hbest += h[i];
        }
        if (solution < hbest)
        {
            solution = hbest;
            best_day = d;
        }
    }
    if (dbg_flags & 0x1) { cerr << "best_day=" << best_day << '\n'; }
}

void Festival::solve()
{
    vattr_t sattr(attractions);
    sort(sattr.begin(), sattr.end());
    int best_day = -1;
    for (ull_t d = 1; d <= D; ++d)
    {
        vu_t h;
        for (const Attraction& a: sattr)
        {
            if ((a.s <= d) && (d <= a.e))
            {
                h.push_back(a.h);
            }
        }
        u_t hsz = h.size();
        u_t hu = min(hsz, K);
        ull_t hbest = 0;
        for (u_t i = hsz - hu; i < hsz; ++i)
        {
            hbest += h[i];
        }
        if (solution < hbest)
        {
            solution = hbest;
            best_day = d;
        }
    }
    if (dbg_flags & 0x1) { cerr << "best_day=" << best_day << '\n'; }
}

void Festival::print_solution(ostream &fo) const
{
    fo << ' ' << solution;
}

static int test(int argc, char ** argv)
{
    int rc = 0;
    int ai = 0;
    u_t n_tests = strtoul(argv[ai++], 0, 0);
    for (u_t ti = 0; (rc == 0) && (ti < n_tests); ++ti)
    {
        cerr << "Tested: " << ti << '/' << n_tests << '\n';
    }
    return rc;
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
        else if (opt == string("-test"))
        {
            rc = test(argc - (ai + 1), argv + (ai + 1));
            return rc;
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

    void (Festival::*psolve)() =
        (naive ? &Festival::solve_naive : &Festival::solve);
    if (solve_ver == 1) { psolve = &Festival::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Festival festival(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (festival.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        festival.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
