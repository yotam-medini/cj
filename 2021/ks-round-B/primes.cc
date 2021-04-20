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
typedef vector<ul_t> vul_t;
typedef vector<bool> vb_t;

static unsigned dbg_flags;

const vul_t& get_primes(unsigned long at_least_until)
{
    static vul_t primes;
    static ul_t until;

    if (primes.empty() || until < at_least_until)
    {
        until = at_least_until;
        primes.clear();
        vb_t sieve(vb_t::size_type(until + 1), true);

        for (ul_t d = 2; d * d <= until; ++d)
        {
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
    }
    return primes;
}


class Primes
{
 public:
    Primes(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    ull_t Z;
    ull_t solution;
};

Primes::Primes(istream& fi) : solution(0)
{
    fi >> Z;
}

void Primes::solve_naive()
{
    const vul_t& primes = get_primes(100000);
    // const vul_t& primes = get_primes(1000000000);
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

void Primes::solve()
{
    solve_naive();
}

void Primes::print_solution(ostream &fo) const
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

    void (Primes::*psolve)() =
        (naive ? &Primes::solve_naive : &Primes::solve);
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
