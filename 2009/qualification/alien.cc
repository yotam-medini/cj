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

typedef vector<string> vs_t;

class Bool26
{
 public:
    Bool26() { fill_n(f, 26, false); }
    bool f[26];
};

class Alien
{
 public:
    Alien(const vs_t& vdict, unsigned vL, istream& fi);
    void solve();
    void print_solution(ostream&);
 private:
    unsigned solution;
    const vs_t &dict;
    unsigned L;
    string pattern;
};

Alien::Alien(const vs_t& vdict, unsigned vL, istream& fi) :
    solution(0), dict(vdict), L(vL)
{
    fi >> pattern;
}

void Alien::solve()
{
    const char *csp = pattern.c_str();
    Bool26 *bf = new Bool26[L];
    unsigned ci = 0;
    for (unsigned bi = 0; bi < L; ++bi)
    {
        char c = csp[ci++];
        if (c == '(')
        {
            while ((c = csp[ci++]) != ')')
            {
                bf[bi].f[c - 'a'] = true;
            }
        }
        else
        {
            bf[bi].f[c - 'a'] = true;
        }
    }

    for (auto di = dict.begin(), de = dict.end(); di != de; ++di)
    {
        const char *dword = (*di).c_str();
        bool match = true;
        for (unsigned ci = 0; match && ( ci < L); ++ci)
        {
            match = bf[ci].f[dword[ci] - 'a'];
        }
        if (match)
        {
            ++solution;
        }
    }

    delete [] bf;
}

void Alien::print_solution(ostream &fo)
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

    unsigned L, D, n_cases;
    *pfi >> L >> D >> n_cases;

    vs_t dict;
    for (unsigned di = 0; di < D; ++di)
    {
        string word;
        *pfi >> word;
        dict.push_back(word);
    }
    

    ostream &fout = *pfo;
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Alien problem(dict, L, *pfi);
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

