// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <cstdlib>

#include <gmpxx.h>

using namespace std;

static unsigned dbg_flags;

typedef vector<mpz_class> vmpz_t;

static mpz_class gcd(mpz_class m, mpz_class n)
{
   while (n)
   {
      mpz_class t = n;
      n = m % n;
      m = t;
   }
   return m;
}

class FairWarn
{
 public:
    FairWarn(istream& fi);
    void solve();
    void print_solution(ostream&);
 private:
    vmpz_t events;
    mpz_class solution; 
};

FairWarn::FairWarn(istream& fi) : solution(0)
{
    vmpz_t::size_type n;
    fi >> n;
    events.reserve(n);
    events.insert(events.end(), n, 0);
    mpz_class t;
    for (vmpz_t::size_type i = 0; i < n; ++i)
    {
        fi >> events[i];
    }
}

void FairWarn::solve()
{
    static const mpz_class z(0);
    sort(events.begin(), events.end());
    mpz_class g = 0;
    const vmpz_t::size_type n = events.size();
    for (vmpz_t::size_type k = 1; k < n; ++k)
    {
        g = gcd(events[k] - events[k - 1], g);
    }
    mpz_class r = events[0] % g;
    solution = (r == z ? z : g - r);    
}

void FairWarn::print_solution(ostream &fo)
{
    fo << " " << solution;
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
    // bool large = (argc > 4) && !strcmp(argv[4], "-large");

    unsigned n_cases;
    *pfi >> n_cases;

    ostream &fout = *pfo;
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        FairWarn fairwarn(*pfi);
        // cerr << "Case ci="<<ci << " (ci+1)="<<ci+1 << "\n";
        fairwarn.solve();
        fout << "Case #"<< ci+1 << ":";
        fairwarn.print_solution(fout);
        fout << "\n";
        fout.flush();
        
    }
    
    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}

