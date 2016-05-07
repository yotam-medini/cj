// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <fstream>
#include <string>
// #include <set>
// #include <map>
// #include <vector>
#include <utility>
#include <algorithm>

#include <cstdlib>
#include <cstring>
// #include <gmpxx.h>

using namespace std;

// typedef mpz_class mpzc_t;
typedef unsigned long ul_t;
// typedef vector<ul_t> vul_t;

#if 0
static const char *dnames = [
    "ZERO",  // Z
    "ONE",                             // O
    "TWO",                       // T
    "THREE",                //R
    "FOUR",  // U
    "FIVE",              // V
    "SIX",   // X
    "SEVEN",       // S
    "EIGHT", // G
    "NINE"
};
#endif

static unsigned dbg_flags;

class Digits
{
 public:
    Digits(istream& fi);
    void solve();
    void print_solution(ostream&) const;
 private:
    void substruct(const char *sub, int n);
    string s;
    int AZcount[26];
    int solution[10];
};

Digits::Digits(istream& fi)
{
    fi >> s;
}

void Digits::solve()
{
    fill_n(&solution[0], 10, -1);
    fill_n(&AZcount[0], 26, 0);
    const char *cs = s.c_str();
    for (unsigned ci = 0, sz = strlen(cs); ci != sz; ++ci)
    {
        char c = cs[ci];
        AZcount[c - 'A']++;
    }

    unsigned n;

    solution[0] = n = AZcount['Z' - 'A'];
    substruct("ZERO", n);

    solution[4] = n = AZcount['U' - 'A'];
    substruct("FOUR", n);

    solution[6] = n = AZcount['X' - 'A'];
    substruct("SIX", n);

    solution[8] = n = AZcount['G' - 'A'];
    substruct("EIGHT", n);

    solution[7] = n = AZcount['S' - 'A'];
    substruct("SEVEN", n);

    solution[5] = n = AZcount['V' - 'A'];
    substruct("FIVE", n);

    solution[3] = n = AZcount['R' - 'A'];
    substruct("THREE", n);

    solution[2] = n = AZcount['T' - 'A'];
    substruct("TWO", n);

    solution[1] = n = AZcount['O' - 'A'];
    substruct("ONE", n);

    solution[9] = n = AZcount['I' - 'A'];
    substruct("NINE", n);

    for (unsigned ci = 0; ci < 16; ++ci)
    {
        if (AZcount[ci] != 0)
        {
            cerr << "ERROR ci=" << ci << "\n";
            exit(1);
        }
    }
}

void Digits::substruct(const char *sub, int n)
{
    for (const char *p = sub; *p; ++p)
    {
        char c = *p;
        AZcount[c - 'A'] -= n;
    }
}

void Digits::print_solution(ostream &fo) const
{
    fo << " ";
    for (unsigned di = 0; di < 10; ++di)
    {
        char c = "0123456789"[di];
        for (int x = 0; x < solution[di]; ++x)
        {
            fo << c;
        }
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
        Digits problem(*pfi);
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
