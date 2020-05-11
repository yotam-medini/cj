// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef long long ll_t;
typedef unsigned long long ull_t;

static unsigned dbg_flags;

class Expogo
{
 public:
    Expogo(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    ll_t x, y;
    bool possible;
    string solution;
};

Expogo::Expogo(istream& fi) : possible(false)
{
    fi >> x >> y;
}

u_t mod4(ll_t x)
{
    u_t ret = (x >= 0 ? x % 4 : (4 - x) % 4);
    return ret;
}

void Expogo::solve_naive()
{
    ll_t sxy[2] = {x, y};
    possible = true;
    while (possible && ((sxy[0] != 0) || (sxy[1] != 0)))
    {
        bool xnow = (sxy[0] % 2 != 0);
        bool ynow = (sxy[1] % 2 != 0);
        possible = (xnow != ynow);
        if (possible)
        {
            u_t xyi = int(ynow);
            u_t xyj = 1 - xyi;
            ll_t w = sxy[xyi]; // must be odd
            bool pos_step = false;
            if (sxy[xyj] == 0)
            {
                if ((w == 1) || (w == 3))
                {
                    pos_step = true;
                }
                else if ((w == -1) || (w == -3))
                {
                    pos_step = false;
                }
                else
                {
                    pos_step = (mod4(w - 1) != 0);
                }
            }
            else
            {
                bool buddy_next_turn = ((sxy[xyj]/2) % 2 != 0);
                pos_step = (buddy_next_turn == (mod4(w - 1) == 0));
            }
            static const char *dim_dirs[2] = {"WE", "SN"};
            static int m1p1[2] = {-1, +1};
            const int ipos_step = int(pos_step);
            const char d = dim_dirs[xyi][ipos_step];
            solution.push_back(d);
            sxy[xyi] -= m1p1[ipos_step];
            sxy[0] /= 2;
            sxy[1] /= 2;
        }
    }
}

void Expogo::solve()
{
    solve_naive();
}

void Expogo::print_solution(ostream &fo) const
{
    if (possible)
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

    void (Expogo::*psolve)() =
        (naive ? &Expogo::solve_naive : &Expogo::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Expogo expogo(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (expogo.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        expogo.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
