// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
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

class Trash
{
 public:
    Trash(istream& fi);
    Trash(u_t _n, const string& _s) : N(_n), S(_s), solution(0) {}
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    ull_t get_solution() const { return solution; }
 private:
    u_t N;
    string S;
    ull_t solution;
};

Trash::Trash(istream& fi) : solution(0)
{
    fi >> N >> S;
}

void Trash::solve_naive()
{
    for (u_t i = 0; i < N; ++i)
    {
        u_t jup = i;
        int jdown = i;
        for ( ; (jup < N) && S[jup] == '0'; ++jup) {}
        for ( ; (jdown >= 0) && S[jdown] == '0'; --jdown) {}
        u_t d_up = (jup < N ? jup - i : N);
        u_t d_down = (jdown >= 0 ? i - jdown : N);
        u_t d = min(d_up, d_down);
        solution += d;
    }
}

void Trash::solve()
{
    int last = -1, i = 0;
    ull_t add = 0;
    for (i = 0; i < int(N); ++i)
    {
        if (S[i] == '1')
        {
            add = 0;
            if (last == -1) 
            {
                if (i > 0)
                {
                    ull_t d = i;
                    add = ((d + 1)*d)/2;
                }
            }
            else
            {
                ull_t d = (i - last - 1);
                ull_t dh = d/2;
                if (dh > 0)
                {
                    add = dh*(dh + 1);
                }
                if (d % 2 == 1)
                {
                    add += dh + 1;
                }
            }
            solution += add;
            last = i;
        }
    }
    ull_t d = N - last - 1;
    add = (d*(d + 1))/2;
    solution += add;
}

void Trash::print_solution(ostream &fo) const
{
    fo << ' ' << solution;
}

static int test(int argc, char ** argv)
{
    int rc = 0;
    int ai = 0;
    u_t n_tests = strtoul(argv[ai++], 0, 0);
    u_t nmin = strtoul(argv[ai++], 0, 0);
    u_t nmax = strtoul(argv[ai++], 0, 0);
    u_t dn = nmax - nmin;
    for (u_t ti = 0; (rc == 0) && (ti < n_tests); ++ti)
    {
        cerr << "Tested: " << ti << '/' << n_tests << '\n';
        u_t n = nmin + (dn > 0 ? rand() % dn : 0);
        string s;
        while (s.size() < n)
        {
            s.push_back("01"[rand() % 2]);
        }
        ull_t solution_naive, solution;
        {
            Trash trash_naive(n, s);
            trash_naive.solve();
            solution_naive = trash_naive.get_solution();
        }
        {
            Trash trash(n, s);
            trash.solve();
            solution = trash.get_solution();
        }
        if (solution != solution_naive)
        {
            cerr << "Failed with: n="<<n << ", s="<<s << '\n';
            rc = 1;
        }
        
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

    void (Trash::*psolve)() =
        (naive ? &Trash::solve_naive : &Trash::solve);
    if (solve_ver == 1) { psolve = &Trash::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Trash trash(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (trash.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        trash.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
