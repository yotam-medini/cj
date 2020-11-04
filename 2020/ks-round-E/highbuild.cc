// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

// #include <algorithm>
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
typedef vector<u_t> vu_t;

static unsigned dbg_flags;

class HighBuild
{
 public:
    HighBuild(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    void solve_normal(u_t a_only, u_t b_only);
    void solve_common_only();
    void solve_slave_left(vu_t& sol, u_t right_only) const;
    // void solve_slave_a(u_t b_only);
    // void solve_slave_b(u_t a_only);
    u_t n, a, b, c;
    vu_t solution;
};

HighBuild::HighBuild(istream& fi)
{
    fi >> n >> a >> b >> c;
}

void HighBuild::solve_naive()
{
    for (u_t i = 0; i < n; ++i)
    {
        solution.push_back(1);
    }
    bool possible = false, done = false;
    while (!(possible or done))
    {
        u_t na = 0, nb = 0, nc = 0;
        for (u_t i = 0; i < n; ++i)
        {
            bool asee = true, bsee = true;
            for (u_t j = 0; (j < i) && asee; ++j)
            {
                asee = solution[j] <= solution[i];
            }
            for (u_t j = i + 1; (j < n) && bsee; ++j)
            {
                bsee = solution[j] <= solution[i];
            }
            na += (asee ? 1 : 0);
            nb += (bsee ? 1 : 0);
            nc += (asee && bsee ? 1 : 0);
        }
        possible = (a == na) && (b == nb) && (c == nc);
        if (!possible)
        {
            u_t k;
            for (k = 0; (k < n) && (solution[k] == n); ++k) 
            {}
            done = (k == n);
            if (!done)
            {
                for (u_t low = 0; low < k; ++low)
                {
                    solution[low] = 1;
                }
                ++solution[k];
            }
        }
    }
    if (!possible)
    {
        solution.clear();
    }    
}

void HighBuild::solve()
{
    const u_t a_only = a - c;
    const u_t b_only = b - c;
    if (n == 1)
    {
         solution.push_back(1);
    }
    else if ((a_only > 0) && (b_only > 0))
    {
         solve_normal(a_only, b_only);
    }
    else if ((a_only == 0) && (b_only == 0))
    {
        solve_common_only();
    }
    else if (a_only == 0)
    {
        // solve_slave_a(b_only);
        solve_slave_left(solution, b_only);
    }
    else // b_only == 0
    {
        // solve_slave_b(a_only);
        vu_t mirror;
        solve_slave_left(mirror, a_only);
        solution.insert(solution.end(), mirror.rbegin(), mirror.rend());
    }
}

void HighBuild::solve_normal(u_t a_only, u_t b_only)
{
    const bool possible = (a + b - c <= n);
    if (possible)
    {
        const u_t extra = n - (a_only + c + b_only);
        if (extra == 0)
        {
            solution.insert(solution.end(), a_only, 1);
            solution.insert(solution.end(), c, 2);
            solution.insert(solution.end(), b_only, 1);
        }
        else
        {
            solution.insert(solution.end(), a_only, 2);
            solution.insert(solution.end(), extra, 1);
            solution.insert(solution.end(), c, 3);
            solution.insert(solution.end(), b_only, 2);
        }
    }
}

#if 0
void HighBuild::solve_normal(u_t a_only, u_t b_only)
{
    const bool possible = (a + b - c <= n);
    if (possible)
    {
        solution = vu_t(n, 0);
        for (u_t i = 0; i < a_only; ++i)
        {
            solution[i] = i + 2;
        }
        const u_t a_high = (a_only > 0 ? solution[a_only - 1] : 0);
        for (u_t i = 0; i < b_only; ++i)
        {
            solution[(n - 1) - i] = i + 2;
        }
        const u_t b_high = (b_only > 0 ? solution[n - b_only] : 0);
        if (dbg_flags & 0x1) {
          cerr << "a_high="<<a_high << ", b_high="<<b_high << '\n'; }
        const u_t common = max(a_high, b_high) + 1;
        for (u_t i = 0; i < c; ++i)
        {
            solution[i + a_only] = common;
        }
        for (u_t i = a_only + c; i + b_only < n; ++i)
        {
            solution[i] = 1;
        }
    }
}
#endif

void HighBuild::solve_common_only()
{
    if (c >= 2)
    {
        solution.insert(solution.end(), c - 1, 2);
        solution.insert(solution.end(), n - c, 1);
        solution.push_back(2);
    }  
}

void HighBuild::solve_slave_left(vu_t& sol, u_t right_only) const
{
     const u_t extra = n - (c + right_only);
     if (extra == 0)
     {
         sol.insert(sol.end(), c, 2);
         sol.insert(sol.end(), right_only, 1);
     }
     else
     {
         sol.insert(sol.end(), c, 3);
         sol.insert(sol.end(), extra, 1);
         sol.insert(sol.end(), right_only, 2);
     }
}

#if 0
void HighBuild::solve_slave_a(u_t b_only)
{
    const u_t M = b_only + 2;
    solution.insert(solution.end(), c, M);
    solution.insert(solution.end(), n - c - b_only, 1);
    for (u_t v = M - 1; solution.size() < n; --v)
    {
        solution.push_back(v);
    }    
}

void HighBuild::solve_slave_b(u_t a_only)
{
    for (u_t i = 0; i < a_only; ++i)
    {
        solution.push_back(i + 2);
    }
    solution.insert(solution.end(), n - c - a_only, 1);
    const u_t M = a_only + 2;
    solution.insert(solution.end(), c, M);
}
#endif

void HighBuild::print_solution(ostream &fo) const
{
    if (solution.empty())
    {
        fo << " IMPOSSIBLE";
    }
    else
    {
        for (u_t x: solution)
        {
            fo << ' ' << x;
        }
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

    void (HighBuild::*psolve)() =
        (naive ? &HighBuild::solve_naive : &HighBuild::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        HighBuild highbuild(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (highbuild.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        highbuild.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
