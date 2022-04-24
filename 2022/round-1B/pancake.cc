// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <iterator>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<ull_t> vull_t;

static unsigned dbg_flags;

class Pancake
{
 public:
    Pancake(istream& fi);
    Pancake(const vull_t& _D) : N(_D.size()), D(_D), solution(0) {}
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    u_t get_solution() const { return solution; }
 private:
    u_t N;
    vull_t D;
    u_t solution;
};

Pancake::Pancake(istream& fi) : solution(0)
{
    fi >> N;
    copy_n(istream_iterator<u_t>(fi), N, back_inserter(D));
}

void Pancake::solve_naive()
{
    ull_t mask_max = 1ull << N;
    for (ull_t mask = 0; mask < mask_max; ++mask)
    {
        ull_t curr_max = 0;
        u_t pay = 0;
        u_t l = 0, r = N - 1;
        for (u_t i = 0; i < N; ++i)
        {
            ull_t deli(-1);
            if ((mask & (1ull << i)) == 0)
            {
                deli = D[l++];
            }
            else
            {
                deli = D[r--];
            }
            if (deli >= curr_max)
            {
                ++pay;
                if (curr_max < deli)
                {
                    curr_max = deli;
                }
            }
        }
        if (solution < pay)
        {
            solution = pay;
        }
    }
}

void Pancake::solve()
{
    ull_t curr_max = 0;
    u_t l = 0, r = N;
    while (l < r)
    {
        ull_t d(-1);
        if (D[l] < D[r - 1])
        {
            d = D[l++];
        }
        else
        {
            d = D[--r];
        }
        if (d >= curr_max)
        {
            ++solution;
            if (curr_max < d)
            {
                curr_max = d;
            }
        }
    }
}

void Pancake::print_solution(ostream &fo) const
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

    void (Pancake::*psolve)() =
        (naive ? &Pancake::solve_naive : &Pancake::solve);
    if (solve_ver == 1) { psolve = &Pancake::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Pancake pancake(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (pancake.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        pancake.print_solution(fout);
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

static int test_case(const vull_t& D)
{
    int rc = 0;
    const u_t N = D.size();
    const bool small = N <= 20;
    u_t solution = 0, solution_naive = 0;
    if (small)
    {
        Pancake p(D);
        p.solve_naive();
        solution_naive = p.get_solution();
    }
    {
        Pancake p(D);
        p.solve();
        solution = p.get_solution();
    }
    if (small && (solution != solution_naive))
    {
        rc = 1;
        cerr << "Fail: solution="<<solution << ", naive="<<solution_naive<<'\n';
        ofstream f("pancake-fail.in");
        f << "1\n" << D.size();
        const char* sep = "\n";
        for (ull_t d: D) { f << sep << d; sep = " "; }
        f << '\n';
        f.close();
    }
    cerr << "   N=" << N << ", solution=" << solution << '\n';
    return rc;
}

static int test_random(int argc, char ** argv)
{
    int rc = 0;
    int ai = 0;
    u_t n_tests = strtoul(argv[ai++], 0, 0);
    u_t Nmin = strtoul(argv[ai++], 0, 0);
    u_t Nmax = strtoul(argv[ai++], 0, 0);
    ull_t Dmax = strtoul(argv[ai++], 0, 0);
    cerr << "Mmin="<<Nmax << ", Nmax="<<Nmax << ", dmax="<<Dmax << '\n'; 
    for (u_t ti = 0; (rc == 0) && (ti < n_tests); ++ti)
    {
        cerr << "Tested: " << ti << '/' << n_tests << '\n';
        u_t N = rand_range(Nmin, Nmax);
        vull_t D;
        while (D.size() < N)
        {
            D.push_back(rand_range(1, Dmax));
        }
        rc = test_case(D);
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
