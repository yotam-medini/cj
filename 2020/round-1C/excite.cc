// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <utility>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
// typedef vector<ul_t> vul_t;

static unsigned dbg_flags;

class Excite
{
 public:
    Excite(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    int x, y;
    string path;
    int solution;
};

Excite::Excite(istream& fi) : solution(-1)
{
    fi >> x >> y >> path;
}

void Excite::solve_naive()
{
    int t = 0, xt = x, yt = y;
    for (char c: path)
    {
        ++t;
        switch (c)
        {
         case 'N':
             yt += 1;
             break;
         case 'E':
             xt += 1;
             break;
         case 'S':
             yt -= 1;
             break;
         case 'W':
             xt -= 1;
             break;
         default:
             cerr << "Error input\n";
             exit(1);
        }
        int d1 = abs(xt) + abs(yt);
        if (t >= d1)
        {
            if ((solution == -1) || (solution > t))
            {
                solution = t;
            }
        }
    }
}

void Excite::solve()
{
   solve_naive();
}

void Excite::print_solution(ostream &fo) const
{
    if (solution >= 0)
    {
        fo << ' ' << solution;
    }
    else
    {
        fo << " IMPOSSIBLE";
    }
}

int main(int argc, char ** argv)
{
    const string dash("-");

    bool naive = false;
    bool tellg = false;
    int rc = 0, ai;

    for (ai = 1; (rc == 0) && (ai < argc) && (argv[ai][0] == '-') &&
        argv[ai][1] != '\0'; ++ai)
    {
        const string opt(argv[ai]);
        if (opt == string("-naive"))
        {
            naive = true;
        }
        else if (opt == string("-debug"))
        {
            dbg_flags = strtoul(argv[++ai], 0, 0);
        }
        else if (opt == string("-tellg"))
        {
            tellg = true;
        }
        else
        {
            cerr << "Bad option: " << opt << "\n";
            return 1;
        }
    }

    int ai_in = ai;
    int ai_out = ai + 1;
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

    string ignore;
    unsigned n_cases;
    *pfi >> n_cases;
    getline(*pfi, ignore);

    void (Excite::*psolve)() =
        (naive ? &Excite::solve_naive : &Excite::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Excite excite(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (excite.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        excite.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
