// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <numeric>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;

static unsigned dbg_flags;

void permutation_first(vu_t &p, u_t n)
{
    p = vu_t(n);
    iota(p.begin(), p.end(), 0);
}

bool permutation_next(vu_t &p)
{
    u_t n = p.size();

    // find the last 'in order'
    int j = n - 1 - 1;
    while ((j >= 0) && (p[j] > p[j + 1]))
    {
        --j;
    }
    bool ret = j >= 0;
    if (ret)
    {
        // Find last > p[j]. Must find since p[j] < p[j+1]
        u_t l;
        for (l = n - 1; p[j] >= p[l]; --l);
        swap(p[j], p[l]);
        reverse(p.begin() + j + 1, p.end());
    }
    return ret;
}

class Sanagram
{
 public:
    Sanagram(istream& fi);
    Sanagram(const string& _s) : S(_s) {}
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    const string& get_solution() const { return solution; }
 private:
    string S;
    string solution;
};

Sanagram::Sanagram(istream& fi)
{
    fi >> S;
}

void Sanagram::solve_naive()
{
    const size_t n = S.size();
    vu_t p;
    bool found = false;
    permutation_first(p, n);
    for (permutation_first(p, n); (!found) && permutation_next(p); )
    {
        string a;
        bool good = true;
        for (size_t i = 0; good && (i < n); ++i)
        {
            const char c = S[p[i]];
            a.push_back(c);
            good = (c != S[i]);
        }
        if (good)
        {
            found = true;
            solution = a;
        }
    }
}

void Sanagram::solve()
{
    const u_t N = S.size();
    vu_t orig_places[26];
    u_t max_rep = 0;
    for (u_t i = 0; i < N; ++i)
    {
        u_t ci = S[i] - 'a';
        orig_places[ci].push_back(i);
        if (max_rep < orig_places[ci].size())
        {
            max_rep = orig_places[ci].size();
        }
    }
    if (2*max_rep <= N)
    {
        vu_t orig_to_sort(N, 0);
        vu_t sort_to_orig(N, 0);
        for (u_t ci = 0, i = 0; ci < 26; ++ci)
        {
            const vu_t& opci = orig_places[ci];
            for (u_t j = 0, je = opci.size(); j < je; ++j, ++i)
            {
                const u_t oi = opci[j];
                orig_to_sort[oi] = i;
                sort_to_orig[i] = oi;
            }
        }
        solution = string(N, ' ');
        const u_t move = N/2;
        for (u_t i = 0; i < N; ++i)
        {
            u_t si = orig_to_sort[i];
            u_t si_move = (si + move) % N;
            u_t i_move = sort_to_orig[si_move];
            solution[i_move] = S[i];
        }
    }
}

void Sanagram::print_solution(ostream &fo) const
{
    static const string impossible("IMPOSSIBLE");
    fo << ' ' << (solution.empty() ? impossible : solution);
}

static int test(int argc, char **argv)
{
    int rc = 0;
    int ai = 0;
    const u_t n_tests = strtoul(argv[ai++], 0, 0);
    const u_t maxlen = strtoul(argv[ai++], 0, 0);
    const u_t maxc = strtoul(argv[ai++], 0, 0);
    for (u_t ti = 0; (rc == 0) && (ti < n_tests); ++ti)
    {
        cerr << "tested: " << ti << '/' << n_tests << '\n';
        size_t sz = rand() % maxlen;
        if (sz == 0) { sz = 1; }
        string s;
        while (s.size() < sz)
        {
            char c = 'a' + (rand() % maxc);
            s.push_back(c);
        }
        Sanagram real(s);
        real.solve();
        const string& solution = real.get_solution();
        if (sz <= 8)
        {
            Sanagram naive(s);
            naive.solve_naive();
            const string& solution_naive = naive.get_solution();
            if (solution_naive.empty() != solution.empty())
            {
                rc = 1;
                cerr << "Inconsistent: sanagram(" << s << "): real: " <<
                    solution << ", naive: " << solution_naive << '\n';
            }
        }
        if (rc == 0)
        {
            cerr << "sanagram( " << s << " ) = " << solution << '\n';
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
            exit(rc);
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

    void (Sanagram::*psolve)() =
        (naive ? &Sanagram::solve_naive : &Sanagram::solve);
    if (solve_ver == 1) { psolve = &Sanagram::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Sanagram sanagram(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (sanagram.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        sanagram.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
