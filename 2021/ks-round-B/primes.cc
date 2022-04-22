// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
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
typedef vector<ul_t> vul_t;
typedef vector<ull_t> vull_t;
typedef vector<bool> vb_t;

static unsigned dbg_flags;

ull_t sqroot(ull_t n)
{
    ull_t r = 0;
    if (n > 0)
    {
        r = 1;
        for (bool iter = true; iter; )
        {
            ull_t d = n / r;
            ull_t rmin = min(r, d);
            ull_t rmax = max(r, d);
            if (rmax - rmin > 1)
            {
                ull_t rnext = (r + d)/2;
                r = rnext;
            }
            else
            {
                iter = false;
                r =  (rmax * rmax <= n) ? rmax : rmin;
            }
        }
    }
    return r;
}

const vul_t& get_primes(unsigned long at_least_until)
{
    static vul_t primes;
    static ull_t until = 0;

    if (primes.empty() || until < at_least_until)
    {
        until = at_least_until;
        primes.clear();
        vb_t sieve(vb_t::size_type(until + 1), true);

        for (ull_t d = 2; d * d <= until; ++d)
        {
            bool show_d = (dbg_flags & 0x1);
            show_d = show_d || ((dbg_flags & 0x2) && ((d & (d - 1)) == 0));
            if (show_d) { cerr << __func__ << " d=" << d << '\n'; }
            if (sieve[d])
            {
                for (ul_t np = d*d; np <= until; np += d)
                {
                    sieve[np] = false;
                }
            }
        }
        // for (auto si = sieve.begin(); si != sieve.end(); ++si)
        for (ul_t si = 2; si <= until; ++si)
        {
            if (sieve[si])
            {
                primes.push_back(si);
            }
        }
        if (dbg_flags & 0x3) {
            cerr << "#(primes)=" << primes.size() << '\n'; 
            for (size_t i = primes.size() - 8; i < primes.size(); ++i) {
                cerr << "P[" << i << "] = " << primes[i] << '\n'; }
        }
    }
    return primes;
}


class Primes
{
 public:
    Primes(istream& fi);
    Primes(ul_t _Z) : Z(_Z), solution(0) {}
    void solve_naive();
    void solve1();
    void solve2();
    void solve();
    void print_solution(ostream&) const;
    ull_t get_solution() const { return solution; }
 private:
    enum { pMaxLarge = 10000000 };
    bool composed2(ull_t n);
    bool is_prime(ull_t k);
    ull_t Z;
    ull_t solution;
};

Primes::Primes(istream& fi) : solution(0)
{
    fi >> Z;
}

void Primes::solve_naive()
{
    const vul_t& primes = get_primes(1000000);
    ull_t r = 1;
    for (u_t pi = 1; r < Z; ++pi)
    {
        r = primes[pi] * primes[pi - 1];
        if (r <= Z)
        {
            solution = r;
        }
    }
}

void Primes::solve1()
{
    static vull_t compose; 
    if (compose.empty())
    {
        const vul_t& primes = get_primes(1000000000);
        const size_t sz = primes.size();
        compose.reserve(sz);
        for (size_t i = 1; i < sz; ++i)
        {
            ull_t pp = ull_t(primes[i - 1]) * ull_t(primes[i]);
            compose.push_back(pp);
        }
    }
    const vull_t& ccompose = compose;
    auto er = equal_range(ccompose.begin(), ccompose.end(), Z);
    size_t ri = (er.first - ccompose.begin());
    if (er.first == er.second)
    {
        --ri;
    }
    solution = ccompose[ri];
}

void Primes::solve2()
{
    ull_t n;
    for (n = Z; !composed2(n); --n)
    {
        ;
    }
    solution = n;
}

bool Primes::composed2(ull_t n)
{
    const vul_t& primes = get_primes(pMaxLarge);
    const size_t psz = primes.size();
    bool ret = false;
    size_t pi = 0;
    ull_t div = 0;
    while ((pi + 1 < psz) && (div == 0))
    {
        if (n % primes[pi] == 0)
        {
            div = primes[pi];
            ret = n == ull_t(primes[pi]) * ull_t(primes[pi + 1]);
        }
        else
        {
            ++pi;
        }
    }
    if (div == 0)
    {
        ull_t d = primes[psz - 1];
        while ((div == 0) && (d*d < n))
        {
            if (n % d == 0)
            {
                div = d;
                const ull_t d1 = n / d;
                ret = is_prime(d1);
                for (ull_t k = d + 1; ret && (k < d1) && !is_prime(k); ++k)
                {
                    ;
                }
            }
            else
            {
                ++d;
            }
        }
    }
    return ret;
}

bool Primes::is_prime(ull_t k)
{
    const vul_t& primes = get_primes(pMaxLarge);
    const size_t psz = primes.size();
    bool ret = true;
    for (size_t pi = 0; ret && (pi < psz) && (primes[pi] * primes[pi] <= k);
         ++pi)
    {
        ret = (k % ull_t(primes[pi])) != 0;
    }
    if (ret)
    {
        for (ull_t d = pMaxLarge; (ret && (k > 1) && (d * d) <= k); ++d)
        {
            ret = (k % d != 0);
        }
    }
    return ret; 
}

void Primes::solve()
{
    const ull_t zroot = sqroot(Z);
    ull_t p1, p2;
    for (p2 = zroot + 1; !is_prime(p2); ++p2) {}
    for (p1 = p2 - 1; !is_prime(p1); --p1) {}
    solution = p1 * p2;
    if (solution > Z)
    {
        p2 = p1;
        for (p1 = p2 - 1; !is_prime(p1); --p1) {}
        solution = p1 * p2;
    }
}

void Primes::print_solution(ostream &fo) const
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
        else if (opt == string("-solve2"))
        {
            solve_ver = 2;
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

    void (Primes::*psolve)() =
        (naive ? &Primes::solve_naive : &Primes::solve);
    if (solve_ver == 1) { psolve = &Primes::solve1; }
    if (solve_ver == 2) { psolve = &Primes::solve2; }
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Primes primes(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (primes.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        primes.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}

static int test_case(ull_t Z)
{
    int rc = 0;
    const bool small = (Z <= 10000);
    ull_t solution = 0, solution_naive = 0;
    if (small)
    {
        Primes p(Z);
        p.solve_naive();
        solution_naive = p.get_solution();
    }
    {
        Primes p(Z);
        p.solve();
        solution = p.get_solution();
    }
    if (small && (solution != solution_naive))
    {
        rc = 1;
        cerr << "Inconsistent: solution="<<solution << " != " << 
            solution_naive << " = solution_naive\n";
        ofstream f("primes-fail.in");
        f << "1\n" << Z << '\n';
        f.close();
    }
    return rc;
}

static int test_random(int argc, char ** argv)
{
    int rc = 0;
    int ai = 0;
    ull_t Zmin = strtoul(argv[ai++], 0, 0);
    ull_t Zmax = strtoul(argv[ai++], 0, 0);
    for (ull_t Z = Zmin; (rc == 0) && (Z < Zmax); ++Z)
    {
        cerr << "Testing: " << Z << " (Zmax=" << Zmax << ")\n";
        rc = test_case(Z);
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
