// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <utility>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef pair<ul_t, ul_t> uu_t;
typedef map<uu_t, bool> uu2bool_t;

static unsigned dbg_flags;

class Game
{
 public:
    Game(ul_t va=1, ul_t vb=1) : a0(va), b0(vb) {}
    bool winning() { return ab_winning(a0, b0); }
 private:
    bool ab_winning(ul_t a, ul_t b);
    ul_t a0, b0;
    uu2bool_t win_memo;
};

bool Game::ab_winning(ul_t a, ul_t b)
{
    bool win = false;
    uu_t key(a, b);
    auto er = win_memo.equal_range(key);
    if (er.first == er.second)
    {
        if (a > b) { swap(a, b); }
        win = (a < b);
        if (win)
        {
            win = false;
            ul_t nk = b / a;
            ul_t r = b % a;
            if (r == 0)
            {
                --nk;
            }
            for (ul_t k = (nk > 1 ? nk - 1 : nk); (k <= nk) && !win; ++k)
            {
                win = (!ab_winning(a, b - k*a));
            }
        }
        uu2bool_t::value_type v(key, win);
        win_memo.insert(er.first, v);
    }
    else
    {
        win = (*(er.first)).second;
    }
    return win;
}

class NumberGame
{
 public:
    NumberGame(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    ul_t a1, a2, b1, b2;
    ull_t solution;
};

NumberGame::NumberGame(istream& fi) : solution(0)
{
    fi >> a1 >> a2 >> b1 >> b2;
}

void NumberGame::solve_naive()
{
    ull_t n_win = 0;
    for (ul_t a = a1; a <= a2; ++a)
    {
        for (ul_t b = b1; b <= b2; ++b)
        {
            Game g(a, b);
            bool a_win = g.winning();
            // cerr << "Game("<<a << ","<<b<<")= A-win?=" << int(a_win) << "\n";
            if (a_win)
            {
                ++n_win;
            }
        }
    }
    solution = n_win;
}

void NumberGame::solve()
{
    solve_naive();
}

void NumberGame::print_solution(ostream &fo) const
{
    fo << " " << solution;
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

    void (NumberGame::*psolve)() =
        (naive ? &NumberGame::solve_naive : &NumberGame::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        NumberGame problem(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

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
