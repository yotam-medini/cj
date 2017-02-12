// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <utility>
#include <algorithm>

#include <cstdlib>

using namespace std;

typedef unsigned long ul_t;
typedef vector<ul_t> vul_t;

static unsigned dbg_flags;

class TextMsg
{
 public:
    TextMsg(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    ul_t P, K, L;
    vul_t freq;
    ul_t solution;
};

TextMsg::TextMsg(istream& fi) :
    solution(0)
{
    fi >> P >> K >> L;
    ul_t n;
    for (ul_t i = 0; i < L; ++i) 
    {
        fi >> n;
        freq.push_back(n);
    }
}

void TextMsg::solve_naive()
{
    sort(freq.begin(), freq.end());
    ul_t press = 1;
    ul_t key = 0;
    for (auto li = freq.rbegin(), le = freq.rend(); li != le; ++li)
    {
        ul_t f = *li;
        solution += press * f;
        ++key;
        if (key == K)
        {
            key = 0;
            ++press;
        }
    }
}

void TextMsg::solve()
{
    solve_naive();
}

void TextMsg::print_solution(ostream &fo) const
{
    fo << " " << solution;
}

int main(int argc, char ** argv)
{
    const string dash("-");

    unsigned n_opts = 0;
    bool naive = false;

    if ((argc > 1) && (string(argv[1]) == "-naive"))
    {
        naive = true;
        n_opts = 1;
    }
    int ai_in = n_opts + 1;
    int ai_out = ai_in + 1;
    int ai_dbg = ai_out + 1;
    istream *pfi = (argc <= ai_in || (string(argv[ai_in]) == dash))
         ? &cin
         : new ifstream(argv[ai_in]);
    ostream *pfo = (argc <= ai_out || (string(argv[ai_out]) == dash))
         ? &cout
         : new ofstream(argv[ai_out]);

    if ((!pfi) || (!pfo) || pfi->fail() || pfo->fail())
    {
        cerr << "Open file error\n";
        exit(1);
    }

    if (ai_dbg < argc) { dbg_flags = strtoul(argv[ai_dbg], 0, 0); }

    unsigned n_cases;
    *pfi >> n_cases;

    void (TextMsg::*psolve)() =
        (naive ? &TextMsg::solve_naive : &TextMsg::solve);

    ostream &fout = *pfo;
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        if (getenv("TELLG"))
        {
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg="<<pfi->tellg() << "\n";
        }
        TextMsg problem(*pfi);
        (problem.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        problem.print_solution(fout);
        fout << "\n";
        fout.flush();

    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
