// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <array>
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
typedef long long ll_t;
typedef vector<ull_t> vull_t;
typedef vector<vull_t> vvull_t;

static unsigned dbg_flags;

class Inflate
{
 public:
    Inflate(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    u_t N, P;
    vvull_t X;
    ull_t solution;
};

Inflate::Inflate(istream& fi) : solution(0)
{
    fi >> N >> P;
    X.reserve(N);
    for (u_t c = 0; c < N; ++c)
    {
        vull_t x;
        copy_n(istream_iterator<u_t>(fi), P, back_inserter(x));
        X.push_back(x);
    }
}

void Inflate::solve_naive()
{
    solution = ull_t(-1); // infinite
    const ull_t mask_max = 1ull << N;
    ull_t best_mask = 0;
    for (ull_t mask = 0; mask < mask_max; ++mask)
    {
         ull_t cand_sol = 0;
         ull_t pos = 0;
         for (u_t c = 0; c < N; ++c)
         {
              const vull_t& xc = X[c];
              ull_t xmin = *min_element(xc.begin(), xc.end());
              ull_t xmax = *max_element(xc.begin(), xc.end());
              bool up = ((1ull << c) & mask) == 0;
              if (up)
              {
                  cand_sol += (pos < xmin) ? xmin - pos : pos - xmin;
                  cand_sol += xmax - xmin;
                  pos = xmax;
              }
              else // down
              {
                  cand_sol += (pos < xmax) ? xmax - pos : pos - xmax;
                  cand_sol += xmax - xmin;
                  pos = xmin;
              }
         }
         if (solution > cand_sol)
         {
             solution = cand_sol;
             best_mask = mask;
         }
    }
    if (dbg_flags & 0x1) { cerr << "best_mask=" << hex << best_mask << '\n'; }
}

void Inflate::solve()
{
    typedef array<ull_t, 2> aull2_t;
    typedef array<ll_t, 2> all2_t;
    aull2_t so_far{0, 0};
    all2_t  last{0, 0};
    for (u_t c = 0; c < N; ++c)
    {
        const vull_t& xc = X[c];
        all2_t next;
        next[0] = *min_element(xc.begin(), xc.end());
        next[1] = *max_element(xc.begin(), xc.end());
        ll_t delta = next[1] - next[0];
        ull_t d01 = abs(last[0] - next[1]);
        ull_t d11 = abs(last[1] - next[1]);
        ull_t d00 = abs(last[0] - next[0]);
        ull_t d10 = abs(last[1] - next[0]);
        // so_far[0] += min(d01, d11) + delta;
        // so_far[1] += min(d00, d10) + delta;
        aull2_t so_far_next{0, 0};
        so_far_next[0] = min(so_far[0] + d01, so_far[1] + d11) + delta;
        so_far_next[1] = min(so_far[0] + d00, so_far[1] + d10) + delta;
        so_far = so_far_next;
        if (dbg_flags & 0x1) { cerr << "c=" << c << ", so_far={" << 
            so_far[0] << ", " << so_far[1] << "}\n"; }
        last = next;
    }
    solution = min(so_far[0], so_far[1]);
}

void Inflate::print_solution(ostream &fo) const
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

    void (Inflate::*psolve)() =
        (naive ? &Inflate::solve_naive : &Inflate::solve);
    if (solve_ver == 1) { psolve = &Inflate::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Inflate inflate(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (inflate.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        inflate.print_solution(fout);
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
