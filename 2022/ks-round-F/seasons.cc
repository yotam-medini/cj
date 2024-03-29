// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
#include <numeric>
#include <string>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef long long ll_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
typedef vector<ull_t> vull_t;

static unsigned dbg_flags;

class Seed
{
 public:
   Seed(ull_t q=0, ull_t l=0, ull_t v=0) : Q(q), L(l), V(v) {}
   ull_t Q, L, V;
};
typedef vector<Seed> vseed_t;
bool lt_value(const Seed& s0, const Seed& s1)
{
    return s0.V < s1.V;
}

class Seasons
{
 public:
    Seasons(istream& fi);
    Seasons(const vu_t&) {}; // TBD for test_case
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    ull_t get_solution() const { return 0; }
 private:
    ull_t D, N, X;
    vseed_t seeds;
    ull_t solution;
};

Seasons::Seasons(istream& fi) : solution(0)
{
    fi >> D >> N >> X;
    seeds.reserve(N);
    while (seeds.size() < N)
    {
        ull_t q, l, v;
        fi >> q >> l >> v;
        seeds.push_back(Seed(q, l, v));
    }
}

void Seasons::solve_naive()
{
    vseed_t s_seeds(seeds);
    sort(s_seeds.begin(), s_seeds.end(),
        [](const Seed& s0, const Seed& s1) -> bool
        {
            bool lt = (s0.L < s1.L) || ((s0.L == s1.L) && (s0.V < s1.V));
            return lt;
        });
    ull_t si = 0;
    for (ull_t d = D - 1; (d >= 1) && (si < N); --d)
    {
        ull_t day_planted = 0;
        ull_t to_mature = D - d;
        if (s_seeds[si].L <= to_mature)
        {
            ull_t si_up = si;
            for ( ; (si_up + 1 < N) && (s_seeds[si_up].L <= to_mature); ++si_up)
            {}
            bool day_done = false;
            while ((day_planted < X) && !day_done)
            {
                ll_t sj = si_up;
                for ( ; (sj >= ll_t(si)) && (s_seeds[sj].Q == 0); --sj) { ; }
                if (sj >= ll_t(si))
                {
                    Seed& s_seed = s_seeds[sj];
                    ull_t plant = min(X - day_planted, s_seed.Q);
                    solution += plant * s_seed.V;
                    s_seed.Q -= plant;
                    day_planted += plant;
                }
                else
                {
                    day_done = true;
                    ++si;
                }
            }
        }
    }
}

void Seasons::solve()
{
    vseed_t s_seeds(seeds);
    sort(s_seeds.begin(), s_seeds.end(),
        [](const Seed& s0, const Seed& s1) -> bool
        {
            bool lt = (s0.L < s1.L);
            return lt;
        });
    // vseed_t::const_iterator iter_b = s_seeds.begin();
    ull_t si = 0;
    vseed_t seeds_heap;
    for (ull_t d = D - s_seeds[0].L; d >= 1; )
    {
        const ull_t mature = s_seeds[si].L;
        ull_t si_end = si; // end this mature level
        for ( ; (si_end < N) && (s_seeds[si_end].L == mature); ++si_end) {}
        const ull_t d_next = (si_end < N ? D - s_seeds[si_end].L : 0);
        const ull_t delta_days = d - d_next;
        for (ull_t sj = si; sj < si_end; ++sj)
        {
            seeds_heap.push_back(s_seeds[sj]);
            push_heap(seeds_heap.begin(), seeds_heap.end(), lt_value);
        }
        ull_t allocation = delta_days * X;
        while ((allocation > 0) && !seeds_heap.empty())
        {
            Seed& seed = seeds_heap.front();
            ull_t plant = min(allocation, seed.Q);
            solution += plant * seed.V;
            allocation -= plant;
            seed.Q -= plant;
            if (seed.Q == 0)
            {
                pop_heap(seeds_heap.begin(), seeds_heap.end(), lt_value);
                seeds_heap.pop_back();
            }
        }
        si = si_end;
        d = d_next;
    }
}

void Seasons::print_solution(ostream &fo) const
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

    void (Seasons::*psolve)() =
        (naive ? &Seasons::solve_naive : &Seasons::solve);
    if (solve_ver == 1) { psolve = &Seasons::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Seasons seasons(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (seasons.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        seasons.print_solution(fout);
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
        Seasons p{vu_t()};
        p.solve_naive();
        solution_naive = p.get_solution();
    }
    {
        Seasons p{vu_t()};
        p.solve();
        solution = p.get_solution();
    }
    if (small && (solution != solution_naive))
    {
        rc = 1;
        cerr << "Failed: solution="<<solution << " != " <<
            solution_naive << " = solution_naive\n";
        ofstream f("seasons-fail.in");
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
