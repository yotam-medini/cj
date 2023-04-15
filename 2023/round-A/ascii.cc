// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

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

static unsigned dbg_flags;

ull_t isqrt(ull_t n)
{
    ull_t r = 0;
    if (n > 0)
    {
        const ull_t nll = n;
        ull_t low = 1;
        ull_t high = nll + 1;
        while (low + 1 < high)
        {
            ull_t mid = (low + high)/2;
            if (mid*mid <= nll)
            {
                low = mid;
            }
            else
            {
                high = mid;
            }
            
        }
        r = low;
    }
    return r;
}


class Ascii
{
 public:
    Ascii(istream& fi);
    Ascii(ull_t _N=1) : N(_N), solution('?') {};
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    char get_solution() const { return solution; }
 private:
    ull_t N;
    char solution;
};

Ascii::Ascii(istream& fi) : solution('?')
{
    fi >> N;
}

void Ascii::solve_naive()
{
    ull_t nc = 0;
    for (ull_t rep = 1; solution == '?'; ++rep)
    {
        for (ull_t az = 0; (az < 26) && (solution == '?'); ++az)
        {
            for (u_t x = 0; (x < rep) && (solution == '?'); ++x, ++nc)
            {
                if (nc + 1 == N)
                {
                    solution = 'A' + az;
                }
            }
        }
    }
}

void Ascii::solve()
{
    const ull_t NB = N/26;
    ull_t r = isqrt(2*NB);
    while (r*(r + 1) < 2*NB)
    {
        ++r;
    }
    while (r*(r + 1) > 2*NB)
    {
        --r;
    }
    ull_t more = N - 26*((r*(r + 1))/2);
    solution = 'Z';
    if (more > 0)
    {
        ull_t az = (more - 1) / (r + 1);
        solution = 'A' + az;
    }
}

void Ascii::print_solution(ostream &fo) const
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

    void (Ascii::*psolve)() =
        (naive ? &Ascii::solve_naive : &Ascii::solve);
    if (solve_ver == 1) { psolve = &Ascii::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Ascii ascii(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (ascii.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        ascii.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}

#if 0
static u_t rand_range(u_t nmin, u_t nmax)
{
    u_t r = nmin + rand() % (nmax + 1 - nmin);
    return r;
}
#endif

static void save_case(const char* fn)
{
    ofstream f(fn);
    f << "1\n";
    f.close();
}

static int test_case(ull_t N)
{
    int rc = 0;
    char solution('*'), solution_naive('*');
    bool small = N < 0x1000;
    if (dbg_flags & 0x100) { save_case("ascii-curr.in"); }
    if (small)
    {
        Ascii p{N};
        p.solve_naive();
        solution_naive = p.get_solution();
    }
    {
        Ascii p{N};
        p.solve();
        solution = p.get_solution();
    }
    if (small && (solution != solution_naive))
    {
        rc = 1;
        cerr << "Failed: solution = " << solution << " != " <<
            solution_naive << " = solution_naive\n";
        save_case("ascii-fail.in");
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
    const ull_t Nmin = strtoul(argv[ai++], nullptr, 0);
    const ull_t Nmax = strtoul(argv[ai++], nullptr, 0);
    const ull_t n_tests = Nmax + 1 - Nmin;
    cerr << "n_tests=" << n_tests <<
        ", Nmin=" << Nmin << ", Nmax=" << Nmax <<
        '\n';
    for (ull_t ti = 0, N = Nmin; (rc == 0) && (N < Nmax); ++ti, ++N)
    {
        cerr << "Tested: " << ti << '/' << n_tests << '\n';
        rc = test_case(N);
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
