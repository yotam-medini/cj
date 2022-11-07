// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

// #include <algorithm>
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
typedef vector<ull_t> vull_t;

static unsigned dbg_flags;

void get_primes(vull_t& primes, ull_t till)
{
    // Eratosthenes
    vector<bool> sieve(till + 1, true);
    for (ull_t n = 2; n <= till; ++n)
    {
        if (sieve[n])
        {
            primes.push_back(n);
            for (ull_t s = n*n; s <= till; s += n)
            {
                sieve[s] = false;
            }
        }
    }
}

static vull_t primes;

class PrimePower
{
 public:
    PrimePower(ull_t _prime=0, u_t _power=0) : prime(_prime), power(_power) {}
    ull_t prime;
    u_t power;
};
typedef vector<PrimePower> vpp_t;

class PaliFactor
{
 public:
    PaliFactor(istream& fi);
    PaliFactor(const vu_t&) {}; // TBD for test_case
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    ull_t get_solution() const { return 0; }
 private:
    bool is_palindrome(ull_t n) const;
    void factorize(ull_t n);
    void iterate(ull_t pre_factor, size_t pfi);
    ull_t A;
    ull_t solution;
    vpp_t pfactors;
};

PaliFactor::PaliFactor(istream& fi) : solution(0)
{
    fi >> A;
}

void PaliFactor::solve_naive()
{
    for (ull_t n = 1; n <= A; ++n)
    {
        if ((A % n == 0) && is_palindrome(n))
        {
            ++solution;
        }
    }
}

void PaliFactor::solve()
{
    if (primes.empty())
    {
        get_primes(primes, 100000); // square-root of 10^10
    }
    factorize(A);
    iterate(1ull, 0);
}

bool PaliFactor::is_palindrome(ull_t n) const
{
    vu_t digits;
    while (n > 0)
    {
        digits.push_back(n % 10);
        n /= 10;
    }
    bool pali = true;
    for (size_t i = 0, j = digits.size() - 1; pali && (i < j); ++i, --j)
    {
        pali = digits[i] == digits[j];
    }
    return pali;
}

void PaliFactor::factorize(ull_t n)
{
    const size_t np = primes.size();
    for (size_t pi = 0; (n > 1) && (pi < np); ++pi)
    {
        const ull_t p = primes[pi];
        if (n % p == 0)
        {
            u_t power = 0;
            while (n % p == 0)
            {
                ++power;
                n /= p;
            }
            pfactors.push_back(PrimePower(p, power));
        }
    }
    if (n > 1)
    {
        pfactors.push_back(PrimePower(n, 1));
    }
}

void PaliFactor::iterate(ull_t pre_factor, size_t pfi)
{
    if (pfi == pfactors.size())
    {
        if (is_palindrome(pre_factor))
        {
            ++solution;
        }
    }
    else
    {
        const PrimePower& pf = pfactors[pfi];
        for (u_t power = 0; power <= pf.power; ++power, pre_factor *= pf.prime)
        {
             iterate(pre_factor, pfi + 1);
        }
    }
}

void PaliFactor::print_solution(ostream &fo) const
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

    void (PaliFactor::*psolve)() =
        (naive ? &PaliFactor::solve_naive : &PaliFactor::solve);
    if (solve_ver == 1) { psolve = &PaliFactor::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        PaliFactor palifactor(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (palifactor.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        palifactor.print_solution(fout);
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
    if (dbg_flags & 0x100) { save_case("palifactor-curr.in"); }
    if (small)
    {
        PaliFactor p{vu_t()};
        p.solve_naive();
        solution_naive = p.get_solution();
    }
    {
        PaliFactor p{vu_t()};
        p.solve();
        solution = p.get_solution();
    }
    if (small && (solution != solution_naive))
    {
        rc = 1;
        cerr << "Failed: solution = " << solution << " != " <<
            solution_naive << " = solution_naive\n";
        save_case("palifactor-fail.in");
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
