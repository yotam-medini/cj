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

class Gain
{
 public:
    Gain(istream& fi);
    Gain(const vull_t& _A, const vull_t& _B, u_t _K) : 
        N(A.size()), M(B.size()), K(_K), A(_A), B(_B) {};
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    ull_t get_solution() const { return solution; }
 private:
    u_t N, M, K;
    vull_t A, B;
    ull_t solution;
};

Gain::Gain(istream& fi) : solution(0)
{
    fi >> N;
    copy_n(istream_iterator<ull_t>(fi), N, back_inserter(A));
    fi >> M;
    copy_n(istream_iterator<ull_t>(fi), M, back_inserter(B));
    fi >> K;
}

#if 0
static void max_by(ull_t& v, ull_t x)
{
    if (v < x)
    {
        v = x;
    }
}
#endif

void Gain::solve_naive()
{
    const u_t ignore = (N + M) - K;
    const u_t na_max = min(N, ignore);
    const u_t nb_max = min(M, ignore);
    vull_t Asums, Bsums; 
    Asums.reserve(N + 1); Bsums.reserve(M + 1);
    Asums.push_back(0);
    Bsums.push_back(0);
    for (const ull_t a: A) { Asums.push_back(Asums.back() + a); }
    for (const ull_t b: B) { Bsums.push_back(Bsums.back() + b); }
    u_t best_na = 0, best_ia = 0, best_ib;
    for (u_t na = ignore - nb_max; na <= na_max; ++na)
    {
        ull_t Amin = Asums.back(), Bmin = Bsums.back();
        u_t tbest_ia = 0, tbest_ib;
        for (u_t ib = 0, ie = na; ie <= N; ++ib, ++ie)
        {
            const ull_t deltaA = Asums[ie] - Asums[ib];
            if (Amin > deltaA)
            {
                Amin = deltaA;
                tbest_ia = ib;
            }
        }
        const u_t nb = ignore - na;
        for (u_t ib = 0, ie = nb; ie <= M; ++ib, ++ie)
        {
            const ull_t deltaB = Bsums[ie] - Bsums[ib];
            if (Bmin > deltaB)
            {
                Bmin = deltaB;
                tbest_ib = ib;
            }
        }
        ull_t candidate = (Asums.back() - Amin) + (Bsums.back() - Bmin);
        if (solution < candidate)
        {
            solution = candidate;
            best_na = na;
            best_ia = tbest_ia;
            best_ib = tbest_ib;
        }
    }
    if (dbg_flags & 0x1) {
        cerr <<"best: #(A)="<<best_na << ", #(B)=" << K - best_na <<
            ", ia=" << best_ia << ", ib=" << best_ib << '\n'; }
}

void Gain::solve()
{
    solve_naive();
}

void Gain::print_solution(ostream &fo) const
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

    void (Gain::*psolve)() =
        (naive ? &Gain::solve_naive : &Gain::solve);
    if (solve_ver == 1) { psolve = &Gain::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Gain gain(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (gain.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        gain.print_solution(fout);
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

static int test_case(const vull_t& A, const vull_t& B, u_t K)
{
    int rc = rand_range(0, 1);
    ull_t solution(-1), solution_naive(-1);
    bool small = rc == 0;
    if (small)
    {
        Gain p{A, B, K};
        p.solve_naive();
        solution_naive = p.get_solution();
    }
    {
        Gain p{A, B, K};
        p.solve();
        solution = p.get_solution();
    }
    if (small && (solution != solution_naive))
    {
        rc = 1;
        cerr << "Failed: solution="<<solution << " != " <<
            solution_naive << " = solution_naive\n";
        ofstream f("gain-fail.in");
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
        vull_t A, B; u_t K = 0;
        rc = test_case(A, B, K);
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
