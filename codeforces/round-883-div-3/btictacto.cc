// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

// #include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
// typedef vector<ul_t> vul_t;

static unsigned dbg_flags;

class Btictacto
{
 public:
    Btictacto(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    string board[3];
    string solution;
};

Btictacto::Btictacto(istream& fi)
{
    for (int r = 0; r < 3; ++r)
    {
        fi >> board[r];
    }
}

void Btictacto::solve_naive()
{
    for (int i = 0; i < 3; ++i)
    {
        if ((board[i][0] == board[i][1]) && (board[i][0] == board[i][2]))
        {
            if (board[i][0] != '.')
            {
                solution.push_back(board[i][0]);
            }
        }
        if ((board[0][i] == board[1][i]) && (board[0][i] == board[2][i]))
        {
            if (board[0][i] != '.')
            {
                solution.push_back(board[0][i]);
            }
        }
    }
    if (board[1][1] != '.')
    {
        if ((board[0][0] == board[1][1]) && (board[0][0] == board[2][2]))
        {
            solution.push_back(board[1][1]);
        }
        if ((board[0][2] == board[1][1]) && (board[0][2] == board[2][0]))
        {
            solution.push_back(board[1][1]);
        }
    }
    while (solution.size() > 1)
    {
        solution.pop_back();
    }
    if (solution.empty())
    {
        solution = string("DRAW");
    }
}

void Btictacto::solve()
{
    solve_naive();
}

void Btictacto::print_solution(ostream &fo) const
{
    fo << solution << '\n';
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

    void (Btictacto::*psolve)() =
        (naive ? &Btictacto::solve_naive : &Btictacto::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Btictacto btictacto(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (btictacto.*psolve)();
        btictacto.print_solution(fout);
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
