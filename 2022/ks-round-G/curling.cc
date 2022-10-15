// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <array>
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
typedef vector<ll_t> vll_t;
typedef vector<u_t> vu_t;
typedef array<ll_t, 2> all2_t;
typedef vector<all2_t> vall2_t;
typedef array<u_t, 2> au2_t;

static unsigned dbg_flags;

class Curling
{
 public:
    Curling(istream& fi);
    Curling(const vu_t&) {}; // TBD for test_case
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    ull_t get_solution() const { return 0; }
 private:
    ll_t Rs, Rh;
    au2_t NM;
    vall2_t team_stones[2]; // red, yellow
    au2_t solution;
};

Curling::Curling(istream& fi) : solution{0, 0}
{
    fi >> Rs >> Rh;
    for (u_t i: {0, 1})
    {
        fi >> NM[i];
        while (team_stones[i].size() < NM[i])
        {
            ll_t x, y;
            fi >> x >> y;
            all2_t stone{x, y};
            team_stones[i].push_back(stone);
        }
    }
}

void Curling::solve_naive()
{
    const ll_t Rhs = Rh + Rs;
    const ll_t inside_sq_dist = Rhs*Rhs;
    vll_t team_sq_dists[2];
    for (u_t i: {0, 1})
    {
        const vall2_t& stones = team_stones[i];
        vll_t& sq_dists = team_sq_dists[i];
        sq_dists.reserve(stones.size());
        for (const all2_t& stone: stones)
        {
             ll_t d2 = stone[0]*stone[0] + stone[1]*stone[1];
             sq_dists.push_back(d2);
        }
        // sort(sq_dists.begin(), sq_dists.end());
    }
    for (u_t i: {0, 1})
    {
        const vll_t& sq_dists = team_sq_dists[i];
        if (!sq_dists.empty())
        {
            const vll_t& sqd_other = team_sq_dists[1 - i];
            if (sqd_other.empty())
            {
                for (ll_t d2: sq_dists)
                {
                    if (d2 <= inside_sq_dist)
                    {
                        ++solution[i];
                    }
                } 
            }
            else
            {
                const ll_t min_other = 
                    *min_element(sqd_other.begin(), sqd_other.end());
                for (ll_t d2: sq_dists)
                {
                    if ((d2 <= inside_sq_dist) && (d2 < min_other))
                    {
                        ++solution[i];
                    }
                } 
            }
        }
    }
}

void Curling::solve()
{
    solve_naive();
}

void Curling::print_solution(ostream &fo) const
{
    fo << ' ' << solution[0] << ' ' << solution[1];
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

    void (Curling::*psolve)() =
        (naive ? &Curling::solve_naive : &Curling::solve);
    if (solve_ver == 1) { psolve = &Curling::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Curling curling(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (curling.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        curling.print_solution(fout);
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
    if (dbg_flags & 0x100) { save_case("curling-curr.in"); }
    if (small)
    {
        Curling p{vu_t()};
        p.solve_naive();
        solution_naive = p.get_solution();
    }
    {
        Curling p{vu_t()};
        p.solve();
        solution = p.get_solution();
    }
    if (small && (solution != solution_naive))
    {
        rc = 1;
        cerr << "Failed: solution = " << solution << " != " <<
            solution_naive << " = solution_naive\n";
        save_case("curling-fail.in");
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
