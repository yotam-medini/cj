// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <utility>

#include <cstdlib>
#include <vector>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<ull_t> vull_t;

static unsigned dbg_flags;

void _get_primes(vull_t& uprimes, ull_t till)
{
    uprimes.clear();
    // Eratosthenes
    vector<bool> sieve(till + 1, true);
    for (ull_t n = 2; n <= till; ++n)
    {
        if (sieve[n])
        {
            uprimes.push_back(n);
            for (ull_t s = n*n; s <= till; s += n)
            {
                sieve[s] = false;
            }
        }
    }
}

static vull_t primes;

void get_primes_till(ull_t till)
{
    static ull_t got_till = 0;
    if (got_till < till)
    {
        _get_primes(primes, till);
        got_till = till;
    }
}

class Streetchk
{
 public:
    Streetchk(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    void solve1();
    void nab(ull_t& na, ull_t& nb, ull_t x) const;
    bool interesting(ull_t x) const;
    ull_t L, R;
    ull_t solution;
};

Streetchk::Streetchk(istream& fi) : solution(0)
{
    fi >> L >> R;
}

void Streetchk::solve_naive()
{
    for (ull_t x = L; x <= R; ++x)
    {
        ull_t na = 0, nb = 0;
        for (ull_t a = 1; a <= x; a += 2)
        {
            if (x % a == 0)
            {
                ++na;
            }
        }
        for (ull_t b = 2; b <= x; b += 2)
        {
            if (x % b == 0)
            {
                ++nb;
            }
        }
        ull_t delta = (na < nb ? nb - na : na - nb);
        if (delta <= 2)
        {
            ++solution;
        }
    }
}

void Streetchk::solve1()
{
    get_primes_till(R);
    for (ull_t x = L; x <= R; ++x)
    {
        ull_t na = 0, nb = 0;
        nab(na, nb, x);
        ull_t delta = (na < nb ? nb - na : na - nb);
        if (delta <= 2)
        {
            ++solution;
        }
    }
}

void Streetchk::nab(ull_t& na, ull_t& nb, ull_t x) const
{
    ull_t p2 = 0;
    while (x % 2 == 0)
    {
        ++p2;
        x /= 2;
    }
    if (x == 1)
    {
        na = 1;
        nb = p2;
    }
    else
    {
        ull_t sna = 1;
        u_t pi = 1;
        while (x > 1)
        {
            while ((x % primes[pi]) != 0)
            {
                ++pi;
            }
            ull_t p = primes[pi];
            u_t pp = 0;
            while ((x % p) == 0)
            {
                x /= p;
                ++pp;
            }
            sna *= (pp + 1);
        }
        ull_t nd = (p2 + 1) * sna;
        na = sna;
        nb = nd - na;
    }
}

void Streetchk::solve()
{
    get_primes_till(R);
    for (ull_t x = L; x <= R; ++x)
    {
        if (interesting(x))
        {
            ++solution;
        }
    }
}

bool Streetchk::interesting(ull_t x) const
{
    bool lt2 = false;
    // Let b number of odd divisors = \prod_{i>2} (k_i + 1)
    if (x % 16 == 0) // 16=2^4 k1=4+
    {
        // d = (5+)b - b) - b = (3+)b > 2
        lt2 = false;
    }
    else if (x % 8 == 0)
    {
        lt2 = (x == 8);
    }
    else if (x % 4 == 0)
    {
        // d = 3b - 2b = b
        lt2 = binary_search(primes.begin(), primes.end(), x);
    }
    else if (x % 2 == 0)
    {
        lt2 = true;
    }
    else
    {
        lt2 = binary_search(primes.begin(), primes.end(), x);
    }
    
    return lt2;
}

void Streetchk::print_solution(ostream &fo) const
{
    fo << ' ' << solution;
}

int main(int argc, char ** argv)
{
    const string dash("-");

    bool naive = false;
    bool tellg = false;
    int rc = 0, ai;

    for (ai = 1; (rc == 0) && (ai < argc) && (argv[ai][0] == '-') &&
        argv[ai][1] != '\0'; ++ai)
    {
        const string opt(argv[ai]);
        if (opt == string("-naive"))
        {
            naive = true;
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

    void (Streetchk::*psolve)() =
        (naive ? &Streetchk::solve_naive : &Streetchk::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Streetchk streetchk(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (streetchk.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        streetchk.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
