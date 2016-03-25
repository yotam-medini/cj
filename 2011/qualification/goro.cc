// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <fstream>
#include <vector>


using namespace std;

static unsigned dbg_flags;

typedef unsigned long ul_t;
typedef vector<ul_t> vul_t;
typedef vector<bool> vb_t;

class Goro
{
 public:
    Goro(istream& fi);
    void solve();
    void print_solution(ostream&);
 private:
    vul_t numbers;
    unsigned solution;
};

Goro::Goro(istream& fi) : solution(0)
{
    unsigned N;
    fi >> N;
    for (unsigned i = 0; i < N; ++i)
    {
        ul_t n;
        fi >> n;
        numbers.push_back(n);
    }
}

void Goro::solve()
{
    for (unsigned n = 0, N = numbers.size(); n < N; ++n)
    {
        if (numbers[n] != n + 1)
        {
            ++solution;
        }
    }
}

void Goro::print_solution(ostream &fo)
{
    char buf[0x20];
    sprintf(buf, "%.6f", float(solution));
    fo << " " << buf;
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
        Goro goro(*pfi);
        // cerr << "Case ci="<<ci << " (ci+1)="<<ci+1 << "\n";
        goro.solve();
        fout << "Case #"<< ci+1 << ":";
        goro.print_solution(fout);
        fout << "\n";
        fout.flush();
        
    }
    
    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}

