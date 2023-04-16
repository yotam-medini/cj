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
typedef vector<u_t> vu_t;
typedef vector<ull_t> vull_t;

static unsigned dbg_flags;

class Collecting
{
 public:
    Collecting(istream& fi);
    Collecting(
       const vull_t &_A,
       u_t _La, u_t _Ra,
       u_t _Lb, u_t _Rb
    ) : N(_A.size()), A(_A), La(_La), Ra(_Ra), Lb(_Lb), Rb(_Rb), solution(0) {}
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    ull_t get_solution() const { return solution; }
 private:
    void set_pre_sum();
    u_t N;
    vull_t A;
    u_t La, Ra;
    u_t Lb, Rb;
    ull_t solution;
    vull_t pre_sum;
};

Collecting::Collecting(istream& fi) : solution(0)
{
    fi >> N;    
    copy_n(istream_iterator<u_t>(fi), N, back_inserter(A));
    fi >> La >> Ra >> Lb >> Rb;
}

static void max_by(ull_t& v, ull_t x)
{
    if (v < x)
    {
        v = x;
    }
}

void Collecting::solve_naive()
{
    set_pre_sum();
    for (u_t x = La; x <= Ra; ++x)
    {
        ull_t b_max = 0;
        for (u_t y = Lb; y <= Rb; ++y)
        {
            if (x != y)
            {
                ull_t a = A[x - 1], b = A[y - 1];
                const int xstep = (x < y ? 1 : -1);
                const int ystep = -xstep;
                ull_t xs = x, ys = y;
                while (xs + xstep != ys)
                {
                    xs += xstep;
                    a += A[xs - 1];
                    if (xs != ys + ystep)
                    {
                        ys += ystep;
                        b += A[ys - 1];
                    }
                }
                if (x < y)
                {
                    for (u_t i = 1; i < x; ++i)
                    {
                        a += A[i - 1];
                    }
                    for (u_t i = y + 1; i <= N; ++i)
                    {
                        b += A[i - 1];
                    }
                }
                else // y < x
                {
                    for (u_t i = 1; i < y; ++i)
                    {
                        b += A[i - 1];
                    }
                    for (u_t i = x + 1; i <= N; ++i)
                    {
                        a += A[i - 1];
                    }

                }
                if (a + b != pre_sum[N]) {
                    cerr << "ERROR @ " << __LINE__ << '\n';
                    exit(1);
                }
                max_by(b_max, b);
            }
        }
        u_t a = pre_sum[N] - b_max;
        max_by(solution, a);
    }
}

void Collecting::set_pre_sum()
{
    pre_sum.reserve(N + 1);
    pre_sum.push_back(0);
    for (ull_t x: A)
    {
        pre_sum.push_back(pre_sum.back() + x);
    }
}

void Collecting::solve()
{
    set_pre_sum();
    for (u_t x = La; x <= Ra; ++x)
    {
        ull_t Bleft = 0, Bright = 0;
        if (Lb < x)
        {
            const u_t y = min(Rb, x - 1);
            const u_t delta = x - y;
            const u_t e = x - delta / 2;
            Bleft = pre_sum[e - 1] - pre_sum[0];
        }
        if (x < Rb)
        {
            const u_t y = max(Lb, x + 1);
            u_t delta = y - x;
            u_t b = x + 1 + delta/2;
            Bright = pre_sum[N] - pre_sum[b - 1];
        }
        if ((Lb < x) && (x < Rb))
        {
            Bleft = pre_sum[x - 1] - pre_sum[0];
            Bright = pre_sum[N] - pre_sum[x];
        }
        const ull_t B = max(Bleft, Bright);
        const ull_t candid = pre_sum[N] - B;
        max_by(solution, candid);
    }
}

void Collecting::print_solution(ostream &fo) const
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

    void (Collecting::*psolve)() =
        (naive ? &Collecting::solve_naive : &Collecting::solve);
    if (solve_ver == 1) { psolve = &Collecting::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Collecting collecting(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (collecting.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        collecting.print_solution(fout);
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

static void save_case(
    const char* fn, 
    const vull_t &A,
    u_t La, u_t Ra,
    u_t Lb, u_t Rb)
{ 
    const u_t N = A.size();
    ofstream f(fn);
    f << "1\n" << N;
    char sep = '\n';
    for (u_t x: A) { f << sep << x; sep = ' '; }
    f << '\n';
    f << La << ' ' << Ra << ' ' << Lb << ' ' << Rb << '\n';
    f.close();
}

static int test_case(
    const vull_t &A,
    u_t La, u_t Ra,
    u_t Lb, u_t Rb)
{
    int rc = 0;
    const u_t N = A.size();
    ull_t solution(-1), solution_naive(-1);
    bool small = (N < 0x20);
    if (dbg_flags & 0x100) {
        save_case("collecting-curr.in", A, La, Ra, Lb, Rb); }
    if (small)
    {
        Collecting p(A, La, Ra, Lb, Rb);
        p.solve_naive();
        solution_naive = p.get_solution();
    }
    {
        Collecting p(A, La, Ra, Lb, Rb);
        p.solve();
        solution = p.get_solution();
    }
    if (small && (solution != solution_naive))
    {
        rc = 1;
        cerr << "Failed: solution = " << solution << " != " <<
            solution_naive << " = solution_naive\n";
        save_case("collecting-fail.in", A, La, Ra, Lb, Rb);
    }
    if (rc == 0) { cerr << "  ..." <<
        (small ? " (small) " : " (large) ") << " N=" << N << " --> " <<
        solution << '\n'; }
    return rc;
}

static void rand_LR(u_t& L, u_t& R, u_t N)
{
    L = rand_range(1, N);
    R = rand_range(1, N);
    if (L > R)
    {
        swap(L, R);
    }
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
    const u_t Amax = strtoul(argv[ai++], nullptr, 0);
    cerr << "n_tests=" << n_tests <<
        ", Nmin=" << Nmin << ", Nmax=" << Nmax <<
        ", Amax=" << Amax <<
        '\n';
    for (u_t ti = 0; (rc == 0) && (ti < n_tests); ++ti)
    {
        cerr << "Tested: " << ti << '/' << n_tests << '\n';
        const u_t N = rand_range(Nmin, Nmax);
        vull_t A; A.reserve(N);
        while (A.size() < N)
        {
            A.push_back(rand_range(1, Amax));
        }
        u_t La, Ra, Lb, Rb;
        bool good_quad = false;
        while (!good_quad)
        {
            rand_LR(La, Ra, N);
            rand_LR(Lb, Rb, N);
            good_quad = !((La <= Lb) && (Lb == Rb) && (Rb <= Ra));
        }
        rc = test_case(A, La, Ra, Lb, Rb);
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
