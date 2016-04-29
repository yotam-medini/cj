// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <fstream>
#include <string>
// #include <set>
// #include <map>
// #include <vector>
#include <utility>

#include <cstdlib>
#include <gmpxx.h>

using namespace std;

typedef mpz_class mpzc_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<ul_t> vul_t;
typedef vector<ull_t> vull_t;

static unsigned dbg_flags;

class Crop
{
 public:
    Crop(istream& fi);
    void solve();
    void print_solution(ostream&) const;
 private:
    unsigned n;
    ull_t A, B, C, D, x0, y0, M;
    mpzc_t solution;
};

Crop::Crop(istream& fi) :
    solution(0)
{
    fi >> n >> A >> B >> C >> D >> x0 >> y0 >> M;
}

void Crop::solve()
{
    vul_t x(vul_t::size_type(n), 0);
    vul_t y(vul_t::size_type(n), 0);

    x[0] = x0 % 3;
    y[0] = y0 % 3;
    ull_t xp = x0;
    ull_t yp = y0;
if (dbg_flags & 0x1) { cerr << "[0]=(" << xp <<", "<<yp<<")\n";}
    for (unsigned i = 1; i < n; ++i)
    {
        ull_t xnext = (A*xp + B) % M;
        ull_t ynext = (C*yp + D) % M;
if (dbg_flags & 0x1) { cerr << "[" << i << "]=(" << xnext <<", "<<ynext<<")\n";}
        x[i] = xnext % 3;
        y[i] = ynext % 3;
        xp = xnext;
        yp = ynext;
    }
    for (unsigned i = 0; i < n; ++i)
    {
        for (unsigned j = i + 1; j < n; ++j)
        {
            unsigned xij = x[i] + x[j];
            unsigned yij = y[i] + y[j];
            for (unsigned k = j + 1; k < n; ++k)
            {
                if ( (((xij + x[k]) % 3) == 0) && (((yij + y[k]) % 3) == 0) )
                {
                    ++solution;
                }
            }    
        }    
    }    

}

void Crop::print_solution(ostream &fo) const
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
        Crop problem(*pfi);
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

