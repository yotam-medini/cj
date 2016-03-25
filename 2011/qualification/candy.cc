// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <utility>

#include <cstdlib>

using namespace std;

static unsigned dbg_flags;

typedef unsigned long ul_t;
typedef vector<ul_t> vul_t;

class Candy
{
 public:
    Candy(istream& fi);
    void solve();
    void print_solution(ostream&);
 private:
    vul_t candies;
    bool can;
    ul_t solution;
};

Candy::Candy(istream& fi) : can(false), solution(0)
{
    unsigned N;
    fi >> N;
    for (unsigned i = 0; i < N; ++i)
    {
        ul_t pile;
        fi >> pile;
        candies.push_back(pile);
    }
}

void Candy::solve()
{
    ul_t xor_candies = 0;
    ul_t patrick = candies[0];
    for (auto ci = candies.begin(), ce = candies.end(); ci != ce; ++ci)
    {
        ul_t pile = *ci;
        xor_candies ^= pile;
        solution += pile;
        if (patrick > pile)
        {
            patrick = pile;
        }
    }
    can = xor_candies == 0;
    solution -= patrick;
}

void Candy::print_solution(ostream &fo)
{
    if (can)
    {
        fo << " " << solution;
    }
    else
    {
        fo << " NO";
    }
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
        Candy candy(*pfi);
        // cerr << "Case ci="<<ci << " (ci+1)="<<ci+1 << "\n";
        candy.solve();
        fout << "Case #"<< ci+1 << ":";
        candy.print_solution(fout);
        fout << "\n";
        fout.flush();
        
    }
    
    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}

