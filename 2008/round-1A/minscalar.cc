// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <utility>
#include <algorithm>

#include <cstdlib>
#include <gmpxx.h>

using namespace std;

// typedef long long int ll_t;
// typedef vector<ll_t> vll_t;
typedef mpz_class mpzc_t;
typedef vector<mpzc_t> vmpz_t;

static unsigned dbg_flags;

class MinScalar
{
 public:
    MinScalar(istream& fi);
    void solve();
    void print_solution(ostream&);
 private:
    vmpz_t v1, v2;
    mpzc_t result;
};

MinScalar::MinScalar(istream& fi)
{
    unsigned n;
    fi >> n;
    v1.reserve(n);
    v2.reserve(n);
    for (unsigned i = 0; i < n; ++i)
    {
        int x;
        fi >> x;
        v1.push_back(x);
    }
    for (unsigned i = 0; i < n; ++i)
    {
        int x;
        fi >> x;
        v2.push_back(x);
    }
}

void MinScalar::solve()
{
    static mpzc_t z(0);
    sort(v1.begin(), v1.end());
    sort(v2.begin(), v2.end(), greater<mpzc_t>());
    result = inner_product(v1.begin(), v1.end(), v2.begin(), z);
}

void MinScalar::print_solution(ostream &fo)
{
    fo << ' ' << result;
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

    if (argc > 3) { dbg_flags = strtoul(argv[3], 0, 0); }

    unsigned n_cases;
    *pfi >> n_cases;

    ostream &fout = *pfo;
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        MinScalar problem(*pfi);
        // cerr << "Case ci="<<ci << " (ci+1)="<<ci+1 << "\n";
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

