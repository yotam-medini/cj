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
// #include <gmpxx.h>

using namespace std;

// typedef mpz_class mpzc_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
// typedef vector<ul_t> vul_t;

static unsigned dbg_flags;
static const  char *XO = "XO";

class TicTacToe
{
 public:
    TicTacToe(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    char board[4][4];
    const char *solution;
};

TicTacToe::TicTacToe(istream& fi) : solution("")
{
    string line;
    for (unsigned i = 0; i < 4; ++i)
    {
        getline(fi, line);
        for (unsigned j = 0; j < 4; ++j)
        {
            board[i][j] = line[j];
        }
    }
    // getline(fi, line);
}

void TicTacToe::solve_naive()
{
    int won = -1;
    bool any_empty = false, may;
    for (unsigned xoi = 0; won == -1 && xoi != 2; ++xoi)
    {
        const char c = XO[xoi];
        unsigned i;
        for (i = 0; won == -1 && i < 4; ++i)
        {
            unsigned j;
            for (j = 0; 
                 (j < 4) && (board[i][j] == c || board[i][j] == 'T'); ++j) {}
            if (j == 4) { won = xoi; }

            for (j = 0; 
                 (j < 4) && (board[j][i] == c || board[j][i] == 'T'); ++j) {}
            if (j == 4) { won = xoi; }

            for (j = 0; j < 4; ++j)
            {
                if (board[i][j] == '.') 
                {
                    any_empty = true;
                }
            }
        }

        may = true;
        for (i = 0; i < 4 && may; ++i)
        {
            char cii = board[i][i];
            may = cii == c || cii == 'T';
        }
        if (may) { won = xoi; }

        may = true;
        for (i = 0; i < 4 && may; ++i)
        {
            char cii = board[i][3 - i];
            may = cii == c || cii == 'T';
        }
        if (may) { won = xoi; }
    }
    if (won != -1)
    {
        solution = (won == 0 ? "X won" : "O won");
    }
    else if (any_empty)
    {
        solution = "Game has not completed";
    }
    else
    {
        solution = "Draw";
    }
    
}

void TicTacToe::solve()
{
    solve_naive();
}

void TicTacToe::print_solution(ostream &fo) const
{
    fo << ' ' << solution;
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

    void (TicTacToe::*psolve)() =
        (naive ? &TicTacToe::solve_naive : &TicTacToe::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        TicTacToe tictactoe(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (tictactoe.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        tictactoe.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
