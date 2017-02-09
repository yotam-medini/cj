// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <utility>

#include <cstdlib>

using namespace std;

typedef unsigned long ul_t;
typedef unsigned long long ull_t;

static unsigned dbg_flags;

typedef unsigned long ul_t;
typedef vector<ul_t> vul_t;
typedef vector<vul_t> vvul_t;
typedef vector<bool> vb_t;

const vul_t &get_primes(unsigned long at_least_until)
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

class NumSets
{
 public:
    NumSets(istream& fi);
    void solve();
    void print_solution(ostream&) const;
 private:
    void dfs(unsigned root);
    ull_t A, B, P;
    unsigned n_sets;
    vvul_t adj;
    vul_t visited;
};

NumSets::NumSets(istream& fi) :
    n_sets(0)
{
    fi >> A >> B >> P;
}

void NumSets::dfs(unsigned root)
{
    visited[root] = true;
    const vul_t &adjr = adj[root];
    for (vul_t::const_iterator i = adjr.begin(), e = adjr.end(); i != e; ++i)
    {
        unsigned vi = *i;
        if (!visited[vi])
        {
            dfs(vi);
        }
    }
}

void NumSets::solve()
{
    ull_t B_A = B - A;
    ull_t B_Ap1 = B_A + 1;

    const vul_t& primes = get_primes(B_Ap1);
    
    // build graph -- adj;
    const vul_t zvul; 
    adj = vvul_t(vvul_t::size_type(B_Ap1), zvul);
    unsigned pi = 0;
    while (primes[pi] < P) { ++pi; }
    while (primes[pi] <= B_A)
    {
        ul_t p = primes[pi];
        ul_t k0 = (p - (A % p)) % p;
        vul_t& adjk0 = adj[k0];
        for (unsigned k = k0 + p; k < B_Ap1; k += p)
        {
             adjk0.push_back(k);
             adj[k].push_back(k0);
        }
        ++pi;
    }

    visited = vul_t(vul_t::size_type(B_Ap1), false);
    unsigned vi = 0;
    while (vi < B_Ap1)
    {
        if (!visited[vi])
        {
            ++n_sets;
            dfs(vi);
        }
        ++vi;
    }
}

void NumSets::print_solution(ostream &fo) const
{
    fo << " " << n_sets;
}

int main(int argc, char ** argv)
{
    const string dash("-");

    istream *pfi = (argc < 2 || (string(argv[1]) == dash))
         ? &cin
         : new ifstream(argv[1]);
    ostream *pfo = (argc < 3 || (string(argv[2]) == dash))
         ? &cout
         : new ofstream(argv[2]);

    if ((!pfi) || (!pfo) || pfi->fail() || pfo->fail())
    {
        cerr << "Open file error\n";
        exit(1);
    }

    if (argc > 3) { dbg_flags = strtoul(argv[3], 0, 0); }
    // bool large = (argc > 4) && !strcmp(argv[4], "-large");

    unsigned n_cases;
    *pfi >> n_cases;

    ostream &fout = *pfo;
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        if (getenv("TELLG"))
        {
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg="<<pfi->tellg() << "\n";
        }
        NumSets problem(*pfi);
        problem.solve();
        fout << "Case #"<< ci+1 << ":";
        problem.print_solution(fout);
        fout << "\n";
        fout.flush();

    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
