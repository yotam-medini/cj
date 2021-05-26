// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

// #include <algorithm>
#include <fstream>
#include <iostream>
// #include <iterator>
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

static unsigned dbg_flags;

static const ull_t BIG_MOD = 1000000000 + 7;

class Smaller
{
 public:
    Smaller(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    ull_t sub_solve(u_t skip) const;
    void fwd(string& base);
    u_t N, K;
    string s;
    ull_t solution;
    vector<ull_t> kps;
};

Smaller::Smaller(istream& fi) : solution(0)
{
    fi >> N >> K >> s;
}

void Smaller::solve_naive()
{
    string base;
    fwd(base);
}

void Smaller::fwd(string& base)
{
    const size_t half = (N + 1)/2;
    const size_t sz = base.size();
    if (sz == half)
    {
        // complete
        string pali(base);
        pali.insert(pali.end(), N - sz, ' ');
        for (size_t i = 0, j = N - 1; j >= sz; ++i, --j)
        {
            pali[j] = pali[i];
        }
        if (pali < s)
        {
            if (dbg_flags & 0x1) { cerr << pali << '\n'; }
            solution = (solution + 1) % BIG_MOD;
        }
    }
    else
    {
        for (u_t ci = 0; ci < K; ++ci)
        {
            base.push_back('a' + ci);
            fwd(base);
            base.pop_back();
        }
    }
}

#if 0
void Smaller::solve()
{
    solve_naive();
}
#endif

#if 1
void Smaller::solve()
{
#if 0
    const u_t half = (N + 1)/2;
    for (u_t skip = 0; skip < half; ++skip)
    {
        ull_t add = sub_solve(skip);
        solution = (solution + add) % BIG_MOD;
    }
#endif
    kps.reserve(N + 1);
    kps.push_back(1);
    for (u_t p = 1; p <= N; ++p)
    {
        ull_t kpnext = (kps.back() + K) % BIG_MOD;
        kps.push_back(kpnext);
    }
    solution = sub_solve(0);
}

ull_t Smaller::sub_solve(u_t skip) const
{
    const char cmax = 'a' + K;
    const u_t half = (N + 1)/2;
    ull_t n = 0;
    ull_t kp = kps[half - skip];
    ull_t add = (min(s[skip], cmax) - 'a') * kp;
    add = add % BIG_MOD;
    n = (n + add) % BIG_MOD;
    if ((s[skip] < cmax) && (skip < half))
    {
        add = sub_solve(skip + 1);
        n = (n + add) % BIG_MOD;
    }
    return n;
}
#endif

#if 0
void Smaller::solve()
{
    const char cmax = 'a' + K;
    const u_t half = (N + 1)/2;
    ull_t n = 0;
    ull_t kp = 1;
    for (u_t p = 0; p < half; ++p)
    {
        const u_t ci = half - p - 1;
        ull_t add = (min(s[ci], cmax) - 'a') * kp;
        n = (n + add) % BIG_MOD;
        kp = (kp * K) % BIG_MOD;
    }
    int ieq = int(half) - 1;
    for ( ; (ieq >= 0) && (s[ieq] == s[N - ieq - 1]); --ieq)
    {
        ;
    }
    if ((ieq >= 0) && (s[ieq] < s[N - ieq - 1]))
    {
       bool below = true;
       for (u_t ci = 0; (ci < half) && below; ++ci)
       {
           below = s[ci] < cmax;
       }
       if (below)
       {
           n = (n + 1) % BIG_MOD;
       }
    }
    solution  = n;
}
#endif

void Smaller::print_solution(ostream &fo) const
{
    fo << ' ' << solution;
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

    void (Smaller::*psolve)() =
        (naive ? &Smaller::solve_naive : &Smaller::solve);
    if (solve_ver == 1) { psolve = &Smaller::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Smaller smaller(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (smaller.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        smaller.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
