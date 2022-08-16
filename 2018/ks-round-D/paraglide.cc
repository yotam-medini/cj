// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <array>
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
typedef array<ull_t, 2> aull2_t;
typedef vector<aull2_t> vaull2_t;

static unsigned dbg_flags;

class PGSolver
{
 public:
    PGSolver(const vaull2_t& _towers, const vaull2_t& _ballons):
        towers(_towers), ballons(_ballons)
        {}
        u_t solve_naive();
        u_t solve();
 private:
    vaull2_t towers;
    vaull2_t ballons;
};

u_t PGSolver::solve_naive()
{
    u_t solution = 0;
    for (const aull2_t& ballon: ballons)
    {
        bool caught = false;
        for (const aull2_t& tower: towers)
        {
            if ((!caught) && (ballon[1] <= tower[1]))
            {
                ull_t dy = tower[1] - ballon[1];    
                ull_t dx = (ballon[0] <= tower[0]
                    ? tower[0] - ballon[0] : ballon[0] - tower[0]);
                caught = dx <= dy;
            }
        }
        if (caught)
        {
            ++solution;
        }
    }
    return solution;
}


u_t PGSolver::solve()
{
    return 0;
}

class Paraglide
{
 public:
    Paraglide(istream& fi);
    Paraglide(const vu_t&) {}; // TBD for test_case
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    u_t get_solution() const { return 0; }
 private:
    void set_towers_ballons();
    u_t N, K;
    ull_t p1, p2, A1, B1, C1, M1;
    ull_t h1, h2, A2, B2, C2, M2;
    ull_t x1, x2, A3, B3, C3, M3;
    ull_t y1, y2, A4, B4, C4, M4;
    vaull2_t towers;
    vaull2_t ballons;
    u_t solution;
};

Paraglide::Paraglide(istream& fi) : solution(0)
{
    fi >> N >> K;
    fi >> p1 >> p2 >> A1 >> B1 >> C1 >> M1;
    fi >> h1 >> h2 >> A2 >> B2 >> C2 >> M2;
    fi >> x1 >> x2 >> A3 >> B3 >> C3 >> M3;
    fi >> y1 >> y2 >> A4 >> B4 >> C4 >> M4;
    set_towers_ballons();    
}

void Paraglide::set_towers_ballons()
{
    towers.reserve(N);
    towers.push_back(aull2_t{p1, h1});
    towers.push_back(aull2_t{p2, h2});
    for (u_t m2 = 0, m1 = 1; towers.size() < N; m2 = m1++)
    {
        ull_t pm2 = towers[m2][0];
        ull_t pm1 = towers[m1][0];
        ull_t hm2 = towers[m2][1];
        ull_t hm1 = towers[m1][1];
        ull_t p = (A1 * pm1 + B1 * pm2 + C1) % M1 + 1;
        ull_t h = (A2 * hm1 + B2 * hm2 + C2) % M2 + 1;
        towers.push_back(aull2_t{p, h});
    }

    ballons.reserve(K);
    ballons.push_back(aull2_t{x1, y1});
    ballons.push_back(aull2_t{x2, y2});
    for (u_t m2 = 0, m1 = 1; ballons.size() < K; m2 = m1++)
    {
        ull_t xm2 = ballons[m2][0];
        ull_t xm1 = ballons[m1][0];
        ull_t ym2 = ballons[m2][1];
        ull_t ym1 = ballons[m1][1];
        ull_t x = (A3 * xm1 + B3 * xm2 + C3) % M3 + 1;
        ull_t y = (A4 * ym1 + B4 * ym2 + C4) % M4 + 1;
        ballons.push_back(aull2_t{x, y});
    }
}

void Paraglide::solve_naive()
{
    solution = PGSolver(towers, ballons).solve_naive();    
}

void Paraglide::solve()
{
    solution = PGSolver(towers, ballons).solve();    
}

void Paraglide::print_solution(ostream &fo) const
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

    void (Paraglide::*psolve)() =
        (naive ? &Paraglide::solve_naive : &Paraglide::solve);
    if (solve_ver == 1) { psolve = &Paraglide::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Paraglide paraglide(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (paraglide.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        paraglide.print_solution(fout);
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
    u_t solution(-1), solution_naive(-1);
    bool small = rc == 0;
    if (small)
    {
        Paraglide p{vu_t()};
        p.solve_naive();
        solution_naive = p.get_solution();
    }
    {
        Paraglide p{vu_t()};
        p.solve();
        solution = p.get_solution();
    }
    if (small && (solution != solution_naive))
    {
        rc = 1;
        cerr << "Failed: solution="<<solution << " != " <<
            solution_naive << " = solution_naive\n";
        ofstream f("paraglide-fail.in");
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
