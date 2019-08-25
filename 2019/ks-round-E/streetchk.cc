// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

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

// primes_to_1000000
vull_t primes;

void eras()
{
    // ull_t M = 1000000000ull;
    ull_t M = 1000000;
    vector<bool> sieve(vector<bool>::size_type(M), true);
    for (ull_t p = 2; p < M; ++p)
    {
        while ((p < M) && !sieve[p])
        {
            ++p;
        }
        if (sieve[p])
        {
            primes.push_back(p);
        }
        for (ull_t x = p*p; x < M; x += p)
        {
            sieve[x] = false;
        }
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
    void nab(ull_t& na, ull_t& nb, ull_t x) const;
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

void Streetchk::solve()
{
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

void Streetchk::print_solution(ostream &fo) const
{
    fo << ' ' << solution;
}

int main(int argc, char ** argv)
{
    eras();
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
