// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <array>
#include <fstream>
#include <iostream>
#include <limits>
#include <string>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
typedef vector<vu_t> vvu_t;
typedef vector<string> vstr_t;
typedef array<u_t, 2> au2_t;
typedef vector<au2_t> vau2_t;

#include "hungarian.cc"

static unsigned dbg_flags;

u_t dist(const au2_t& p0, const au2_t& p1)
{
    u_t d = 0;
    for (u_t i: {0, 1})
    {
        u_t di = (p0[i] < p1[i] ? p1[i] - p0[i] : p0[i] - p1[i]);
        d += di;
    }
    return d;
}

class Retiling
{
 public:
    Retiling(istream& fi);
    Retiling(const vu_t&) {}; // TBD for test_case
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    ull_t get_solution() const { return 0; }
 private:
    void dmat_fill(vvu_t& dmat, const vau2_t& Lset, vau2_t& Rset);
    u_t R, C, F, S;
    vstr_t floors[2]; // current, want
    ull_t solution;
};

Retiling::Retiling(istream& fi) : solution(0)
{
    // copy_n(istream_iterator<u_t>(fi), N, back_inserter(a));
    fi >> R >> C >> F >> S;
    for (u_t i: {0, 1})
    {
        vstr_t& floor = floors[i];
        floor.reserve(R);
        while (floor.size() < R)
        {
            string row;
            fi >> row;
            floor.push_back(row);
        }
    }
}

void Retiling::solve_naive()
{
    ull_t x_count[2] = {0, 0};
    for (u_t r = 0; r < R; ++r)
    {
        const string& row_s = floors[0][r];
        const string& row_t = floors[1][r];
        for (u_t c = 0; c < C; ++c)
        {
            if (row_s[c] != row_t[c])
            {
                u_t zo = row_s[c] == 'M' ? 0 : 1;
                ++x_count[zo];
            }
        }
    }
    if (dbg_flags & 0x1) {
        cerr << "#(M2G)=" << x_count[0] << ", #(G2M)=" << x_count[1] << '\n'; }
    const u_t zo = (x_count[0] < x_count[1] ? 0 : 1);
    const char mg = "MG"[zo];
    const u_t nx_low = x_count[zo], nx_high = x_count[1 - zo];
    vau2_t LR[2];
    vau2_t &Lset = LR[0], &Rset = LR[1]; 
    Lset.reserve(nx_low); Rset.reserve(nx_high);
    for (u_t r = 0; r < R; ++r)
    {
        const string& row_s = floors[0][r];
        const string& row_t = floors[1][r];
        for (u_t c = 0; c < C; ++c)
        {
            if (row_s[c] != row_t[c])
            {
                u_t lri = row_s[c] == mg ? 0 : 1;
                LR[lri].push_back(au2_t{r, c});
            }
        }
    }
    vvu_t dmat;
    dmat_fill(dmat, Lset, Rset);
    vu_t matching;
    minimal_matching(matching, dmat);
    for (u_t i = 0; i < nx_low; ++i)
    {
        const u_t j = matching[i];
        const ull_t d = dmat[i][j];
        ull_t pay = min<ull_t>(S*d, 2*F);
        solution += pay;
    }
    solution += (nx_high - nx_low)*F;
}

void Retiling::dmat_fill(vvu_t& dmat, const vau2_t& Lset, vau2_t& Rset)
{
    const u_t Lsize = Lset.size(); // >= Lset.size();
    const u_t N = Rset.size(); // >= Lset.size();
    dmat.clear();
    dmat.reserve(N);
    for (u_t i = 0; i < Lsize; ++i)
    {
        const au2_t& Li = Lset[i];
        vu_t drow; drow.reserve(N);
        for (u_t j = 0; j < N; ++j)
        {
            const au2_t& Rj = Rset[j];
            drow.push_back(dist(Li, Rj));
        }
        dmat.push_back(drow);
    }
    const u_t infi = numeric_limits<int>::max() / ((N - Lsize) + 1);
    dmat.insert(dmat.end(), N - Lsize, vu_t(N, infi));
}

void Retiling::solve()
{
    solve_naive();
}

void Retiling::print_solution(ostream &fo) const
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

    void (Retiling::*psolve)() =
        (naive ? &Retiling::solve_naive : &Retiling::solve);
    if (solve_ver == 1) { psolve = &Retiling::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Retiling retiling(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (retiling.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        retiling.print_solution(fout);
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
    if (dbg_flags & 0x100) { save_case("retiling-curr.in"); }
    if (small)
    {
        Retiling p{vu_t()};
        p.solve_naive();
        solution_naive = p.get_solution();
    }
    {
        Retiling p{vu_t()};
        p.solve();
        solution = p.get_solution();
    }
    if (small && (solution != solution_naive))
    {
        rc = 1;
        cerr << "Failed: solution = " << solution << " != " <<
            solution_naive << " = solution_naive\n";
        save_case("retiling-fail.in");
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
    const u_t Nmin = strtoul(argv[ai++], nullptr, 0);
    const u_t Nmax = strtoul(argv[ai++], nullptr, 0);
    cerr << "n_tests=" << n_tests <<
        ", Nmin=" << Nmin << ", Nmax=" << Nmax <<
        '\n';
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
