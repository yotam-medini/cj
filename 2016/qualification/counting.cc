// CodeJam
// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <fstream>
#include <string>
#include <utility>

#include <cstdlib>

using namespace std;

typedef unsigned long ul_t;

static unsigned dbg_flags;

class Counting
{
 public:
    Counting(istream& fi);
    void solve();
    void print_solution(ostream&);
 private:
    void get_digits(ul_t n);
    ul_t N;
    bool digits[10];
    unsigned n_digits;
    ul_t last;
};

Counting::Counting(istream& fi) :
    n_digits(0),
    last(0)
{
    fi >> N;
    fill_n(digits, 10, false);
}

void Counting::solve()
{
    if (N > 0)
    {
        unsigned loop = 0;
        while (n_digits < 10)
        {
            if (loop > 1000) { cerr << "fuck\n"; exit(1); }
            last += N;
            get_digits(last);
            ++loop;   
        }
    }
}

void Counting::get_digits(ul_t n)
{
    while (n > 0)
    {
        unsigned digit = n % 10;
        n /= 10;
        if (!digits[digit])
        {
            ++n_digits;
            digits[digit] = true;
        }
    }
}

void Counting::print_solution(ostream &fo)
{
    if (last == 0)
    {
        fo << " INSOMNIA";
    }
    else
    {
        fo << ' ' << last;
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
        Counting counting(*pfi);
        // cerr << "Case ci="<<ci << " (ci+1)="<<ci+1 << "\n";
        counting.solve();
        fout << "Case #"<< ci+1 << ":";
        counting.print_solution(fout);
        fout << "\n";
        fout.flush();
        
    }
    
    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}

