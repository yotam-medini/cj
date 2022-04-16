// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <utility>
#include <vector>
// #include <map>
// #include <set>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<ull_t> vull_t;

static unsigned dbg_flags;

class Query1
{
 public:
    Query1(u_t p, ull_t v) : pos(p), val(v) {}
    u_t pos;
    ull_t val;
};

class Query2
{
 public:
    Query2(ull_t _s, u_t _l, u_t _r) : S(_s), L(_l), R(_r) {}
    ull_t S;
    u_t L, R;
};

union UQ
{
 public:
     UQ() {}
     ~UQ() {}
     Query1 query1;
     Query2 query2;
};

class Query
{
 public:
    Query() {}
    u_t qt;
    UQ u;
};
typedef vector<Query> vquery_t;

class PQueries
{
 public:
    PQueries(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    ull_t sum_formula_naive(const Query2& query2) const;
    u_t N, Q;
    ull_t P;
    vull_t A;
    vquery_t queries;
    vull_t solution;
    u_t q2_count;
};

PQueries::PQueries(istream& fi) : q2_count(0)
{
    fi >> N >> Q >> P;
    A.reserve(N);
    copy_n(istream_iterator<ull_t>(fi), N, back_inserter(A));
    queries.reserve(Q);
    while (queries.size() < Q)
    {
        Query query;
        fi >> query.qt;
        switch (query.qt)
        {
         case 1:
            {
                u_t pos; ull_t val;
                fi >> pos >> val;
                query.u.query1 = Query1(pos, val);
            }
            break;
         case 2:
            {
                ull_t S, R; u_t L;
                fi >> S >> L >> R;
                query.u.query2 = Query2(S, L, R);
                ++q2_count;
            }
            break;
         default:
            cerr << "bad query type: " << query.qt << '\n';
            exit(1);
        }
        queries.push_back(query);
    }
}

void PQueries::solve_naive()
{
    solution.reserve(q2_count);
    for (const Query& query: queries)
    {
        switch (query.qt)
        {
         case 1:
            A[query.u.query1.pos - 1] = query.u.query1.val;
            break;
         case 2:
            solution.push_back(sum_formula_naive(query.u.query2));
            break;
        }
    }
}

ull_t PQueries::sum_formula_naive(const Query2& query2) const
{
    ull_t total = 0;
    for (u_t i = query2.L - 1; i <= query2.R - 1; ++i)
    {
        const u_t a = A[i];
        const u_t amod = a % P;
        ull_t a_ps = 1, amod_ps = 1;
        for (u_t s = 0; s < query2.S; ++s)
        {
            a_ps *= a;
            amod_ps *= amod;
        }
        ull_t n = a_ps - amod_ps;
        u_t padic = 0;
        if (n > 0)
        {
            while (n % P == 0)
            {
                ++padic;
                n /= P;
            }
        }
        total += padic;
    }
    return total;
}

void PQueries::solve()
{
    solve_naive();
}

void PQueries::print_solution(ostream &fo) const
{
    for (ull_t x: solution)
    {
        fo << ' ' << x;
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

    void (PQueries::*psolve)() =
        (naive ? &PQueries::solve_naive : &PQueries::solve);
    if (solve_ver == 1) { psolve = &PQueries::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        PQueries pqueries(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (pqueries.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        pqueries.print_solution(fout);
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
