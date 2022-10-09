// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

// #include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <numeric>
#include <string>
#include <utility>
#include <vector>
// #include <iterator>
// #include <map>
// #include <set>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
typedef vector<vu_t> vvu_t;
typedef array<u_t, 2> au2_t;
typedef vector<au2_t> vau2_t;
typedef vector<bool> vb_t;

static unsigned dbg_flags;

void combination_first(vu_t &c, u_t n, u_t k)
{
    c = vu_t(k);
    iota(c.begin(), c.end(), 0);
}

bool combination_next(vu_t &c, u_t n)
{
    u_t j = 0, k = c.size();

    // sentinels (Knuth) (Why the second (0) ??!)
    c.push_back(n);  c.push_back(0);

    while ((j < k) && (c[j] + 1 == c[j + 1]))
    {
        c[j] = j;
        ++j;
    }
    bool ret = j < k;
    if (ret)
    {
        ++(c[j]);
    }

    c.pop_back(); c.pop_back(); // the sentinels

    return ret;
}

class Suspects
{
 public:
    Suspects(istream& fi);
    Suspects(const vu_t&) {}; // TBD for test_case
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    ull_t get_solution() const { return 0; }
 private:
    void set_witness_innocents();
    u_t N, M, K;
    vau2_t statements;
    u_t solution;
    vvu_t witness_innocents;
};

Suspects::Suspects(istream& fi) : solution(0)
{
    fi >> N >> M >> K;
    statements.reserve(M);
    while (statements.size() < M)
    {
        u_t A, B;
        fi >> A >> B;
        statements.push_back(au2_t{A, B});
    }
}

void Suspects::solve_naive()
{
    set_witness_innocents();
    vb_t innocents(N, true);
    for (u_t ncs = 1; ncs <= K; ++ncs) // # cookie stealers
    {
        vu_t comb;
        combination_first(comb, N, ncs);
        for (bool more = true; more; more = combination_next(comb, N))
        {
            vb_t cs(N, false);
            for (u_t i: comb) { cs[i] = true; }
            bool possible = true;
            for (u_t g = 0; possible && (g < N); ++g)
            {
                if (!cs[g]) // guest is assumed innocent
                {
                    const vu_t& g_innocents = witness_innocents[g];
                    for (u_t si = 0; possible && (si < g_innocents.size()); ++si)
                    {
                        u_t og = g_innocents[si];
                        possible = !cs[og];
                    }
                }
            }
            if (possible)
            {
                for (u_t i: comb) { innocents[i] = false; }
            }
        }
    }
    for (u_t i = 0; i < N; ++i)
    {
        solution += innocents[i] ? 1 : 0;
    }
}

void Suspects::solve()
{
    solve_naive();
}

void Suspects::set_witness_innocents()
{
    witness_innocents = vvu_t(N, vu_t());
    for (const au2_t& s: statements)
    {
        witness_innocents[s[0] - 1].push_back(s[1] - 1);
    }
}

void Suspects::print_solution(ostream &fo) const
{
    fo << ' ' << solution;
}

static int real_main(int argc, char ** argv)
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

    void (Suspects::*psolve)() =
        (naive ? &Suspects::solve_naive : &Suspects::solve);
    if (solve_ver == 1) { psolve = &Suspects::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Suspects suspects(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (suspects.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        suspects.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}

static u_t rand_range(u_t nmin, u_t nmax)
{
    u_t r = nmin + rand() % (nmax + 1 - nmin);
    return r;
}

static int test_case(int argc, char ** argv)
{
    int rc = rand_range(0, 1);
    ull_t solution(-1), solution_naive(-1);
    bool small = rc == 0;
    if (small)
    {
        Suspects p{vu_t()};
        p.solve_naive();
        solution_naive = p.get_solution();
    }
    {
        Suspects p{vu_t()};
        p.solve();
        solution = p.get_solution();
    }
    if (small && (solution != solution_naive))
    {
        rc = 1;
        cerr << "Failed: solution="<<solution << " != " <<
            solution_naive << " = solution_naive\n";
        ofstream f("suspects-fail.in");
        f << "1\n";
        f.close();
    }
    return rc;
}

static int test_random(int argc, char ** argv)
{
    int rc = 0;
    int ai = 0;
    u_t n_tests = strtoul(argv[ai++], 0, 0);
    for (u_t ti = 0; (rc == 0) && (ti < n_tests); ++ti)
    {
        cerr << "Tested: " << ti << '/' << n_tests << '\n';
        rc = test_case(argc, argv);
    }
    return rc;
}

int main(int argc, char **argv)
{
    int rc = ((argc > 1) && (string(argv[1]) == string("test"))
        ? test_random(argc - 2, argv + 2)
        : real_main(argc, argv));
    return rc;
}
