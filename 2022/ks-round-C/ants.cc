// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
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
typedef long long ll_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
typedef vector<ull_t> vull_t;

static unsigned dbg_flags;

class Ant
{
 public:
    Ant(ll_t p=0, int d=0) : hcm_pos(p), dir(d) {}
    void step() { hcm_pos += ll_t(dir); }
    ll_t hcm_pos; // half cm position;
    int dir; // +1 || -1
};
typedef vector<Ant> vant_t;

class Ants
{
 public:
    Ants(istream& fi);
    Ants(const vu_t&) {}; // TBD for test_case
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    ull_t get_solution() const { return 0; }
 private:
    u_t N;
    ull_t L;
    vull_t P;
    vu_t D;
    vu_t solution;
};

Ants::Ants(istream& fi)
{
    fi >> N >> L;
    P.reserve(N);
    D.reserve(N);
    while (P.size() < N)
    {
        ull_t p; u_t d;
        fi >> p >> d;
        P.push_back(p);
        D.push_back(d);
    }
}

void Ants::solve_naive()
{
    vant_t ants; ants.reserve(N);
    vu_t active;
    for (u_t i = 0; i < N; ++i)
    {
        ants.push_back(Ant(2*P[i], D[i] == 0 ? -1 : +1));
        active.push_back(i);
    }
    // simulate
    const ll_t Lhcm = 2*L;
    while (!active.empty())
    {
        vu_t drop;
        for (u_t i: active)
        {
            Ant& ant = ants[i];
            ant.step();
            if ((ant.hcm_pos == 0) || (ant.hcm_pos == Lhcm))
            {
                drop.push_back(i);
            }
        }
        if (!drop.empty())
        {
            solution.insert(solution.end(), drop.begin(), drop.end());
            for (u_t i = drop.size(); i > 0; )
            {
                --i;
                u_t ai = drop[i];
                active[ai] = active.back();
                active.pop_back();
            }
            sort(active.begin(), active.end());
        }
        for (u_t i = 0; i < active.size(); ++i)
        {
            for (u_t j = i + 1; j < active.size(); ++j)
            {
                if (ants[i].hcm_pos == ants[j].hcm_pos)
                {
                    swap(ants[i].dir, ants[j].dir);
                }
            }
        }
    }
}

void Ants::solve()
{
    solve_naive();
}

void Ants::print_solution(ostream &fo) const
{
    for (u_t a: solution) { fo << ' ' << (a + 1); }
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
            dbg_flags = strtoul(argv[++ai], nullptr, 0);
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

    void (Ants::*psolve)() =
        (naive ? &Ants::solve_naive : &Ants::solve);
    if (solve_ver == 1) { psolve = &Ants::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Ants ants(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (ants.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        ants.print_solution(fout);
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

static void save_case(const char* fn)
{
    ofstream f(fn);
    f << "1\n";
    f.close();
}

static int test_case(int argc, char ** argv)
{
    int rc = rand_range(0, 1);
    ull_t solution(-1), solution_naive(-1);
    bool small = rc == 0;
    if (dbg_flags & 0x100) { save_case("ants-curr.in"); }
    if (small)
    {
        Ants p{vu_t()};
        p.solve_naive();
        solution_naive = p.get_solution();
    }
    {
        Ants p{vu_t()};
        p.solve();
        solution = p.get_solution();
    }
    if (small && (solution != solution_naive))
    {
        rc = 1;
        cerr << "Failed: solution = " << solution << " != " <<
            solution_naive << " = solution_naive\n";
        save_case("ants-fail.in");
    }
    if (rc == 0) { cerr << "  ..." <<
        (small ? " (small) " : " (large) ") << " --> " <<
        solution << '\n'; }
    return rc;
}

static int test_random(int argc, char ** argv)
{
    int rc = 0;
    int ai = 0;
    if (string(argv[ai]) == string("-debug"))
    {
        dbg_flags = strtoul(argv[ai + 1], nullptr, 0);
        ai += 2;
    }
    const u_t n_tests = strtoul(argv[ai++], nullptr, 0);
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
