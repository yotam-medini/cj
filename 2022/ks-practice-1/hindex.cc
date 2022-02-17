// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <utility>
#include <vector>
#include <numeric>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<ull_t> vull_t;

static unsigned dbg_flags;

class Hindex
{
 public:
    Hindex(istream& fi);
    Hindex(const vull_t& _c) : N(_c.size()), c(_c) {}
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    const vull_t& get_solution() const { return solution; }
 private:
    ull_t N;
    vull_t c;
    vull_t solution;
};

Hindex::Hindex(istream& fi)
{
    fi >> N;
    c.reserve(N);
    solution.reserve(N);
    copy_n(istream_iterator<ull_t>(fi), N, back_inserter(c));
}

void Hindex::solve_naive()
{
    ull_t hmax = 0;
    for (ull_t i = 0; i < N; ++i)
    {
        hmax = max(hmax, c[i]);
        ull_t hindex = 0;
        for (ull_t candid = 1; candid <= hmax; ++candid)
        {
            ull_t count = 0;
            for (ull_t j = 0; j <= i; ++j)
            {
                if (c[j] >= candid)
                {
                    ++count;
                }
            }
            if (count >= candid)
            {
                hindex = candid;
            }
        }
        solution.push_back(hindex);
    }
}

void Hindex::solve()
{
    vull_t cc(c); // a copy
    solution.push_back(1);
    for (ull_t n = 2; n <= N; ++n)
    {
        const ull_t last = solution.back();
        const ull_t candidate = last + 1;
        const vull_t::iterator b = cc.begin();
        const vull_t::iterator pos = b + (n - candidate);
        nth_element(b, pos, b + n);
        const ull_t v = *pos;
        const ull_t h = (v >= candidate ? candidate : last);
        solution.push_back(h);
    }
}

void Hindex::print_solution(ostream &fo) const
{
    for (ull_t h: solution)
    {
        fo << ' ' << h;
    }
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

    void (Hindex::*psolve)() =
        (naive ? &Hindex::solve_naive : &Hindex::solve);
    if (solve_ver == 1) { psolve = &Hindex::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Hindex hindex(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (hindex.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        hindex.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}

static int test_case(const vull_t& c)
{
    int rc = 0;
    Hindex production(c);
    production.solve();
    const vull_t& solution = production.get_solution();
    if (c.size() < 0x10)
    {
        Hindex naive(c);
        naive.solve_naive();
        const vull_t& naive_solution = naive.get_solution();;
        if (solution != naive_solution)
        {
            cerr << "Inconsistent: specific";
            for (ull_t h: c) { cerr << ' ' << h; }
            cerr << '\n';
            rc = 1;
        }
    }
    return rc;    
}

static int test_specific(int argc, char ** argv)
{
    int rc = 0;
    vull_t c; c.reserve(argc);
    for (int ai = 0; ai < argc; ++ai)
    {
        c.push_back(strtoul(argv[ai++], 0, 0));
    }
    rc =  test_case(c);
    return rc;
}

static int test_random(int argc, char ** argv)
{
    int rc = 0;
    int ai = 0;
    u_t n_tests = strtoul(argv[ai++], 0, 0);
    ull_t nmin = strtoul(argv[ai++], 0, 0);
    ull_t nmax = strtoul(argv[ai++], 0, 0);
    ull_t hmax = strtoul(argv[ai++], 0, 0);
    ull_t ndelta = nmax - nmin;
    ull_t hdelta = hmax - 1;
    for (u_t ti = 0; (rc == 0) && (ti < n_tests); ++ti)
    {
        cerr << "Tested: " << ti << '/' << n_tests << '\n';
        ull_t n = nmin + (ndelta > 0 ? rand() % ndelta : 0);
        vull_t c; c.reserve(n);
        while (c.size() < n)
        {
            c.push_back(1 + (hdelta > 0 ? rand() % hdelta : 0));
        }
        test_case(c);
    }
    return rc;
}

static int test(int argc, char ** argv)
{
    int rc = ((argc > 1) && (string(argv[1]) == string("specific"))
        ? test_specific(argc - 1, argv + 1)
        : test_random(argc, argv));
    return rc;
}

int main(int argc, char **argv)
{
    int rc = ((argc > 1) && (string(argv[1]) == string("test"))
        ? test(argc - 2, argv + 2)
        : real_main(argc, argv));
    return rc;
}
