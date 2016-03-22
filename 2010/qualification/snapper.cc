// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <fstream>

#include <cstdlib>

using namespace std;

static unsigned dbg_flags;

class Snapper
{
 public:
    Snapper(istream& fi);
    void solve();
    void print_solution(ostream&);
 private:
    unsigned n, k;
    bool on;
};

Snapper::Snapper(istream& fi) : on(false)
{
    fi >> n >> k;
}

void Snapper::solve()
{
    // on = (n ? (((k >> (n - 1)) & 0x1) != 0) : 0);
    unsigned mask = (1 << n) - 1;
    on = (k & mask) == mask;
}

void Snapper::print_solution(ostream &fo)
{
    fo << " " << (on ? "ON" : "OFF");
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
        Snapper snapper(*pfi);
        // cerr << "Case ci="<<ci << " (ci+1)="<<ci+1 << "\n";
        snapper.solve();
        fout << "Case #"<< ci+1 << ":";
        snapper.print_solution(fout);
        fout << "\n";
        fout.flush();
        
    }
    
    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}

