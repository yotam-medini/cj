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
typedef unsigned long long ull_t;
typedef vector<ull_t> vull_t;

static unsigned dbg_flags;

class Robot
{
 public:
    Robot(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    string prog;
    ull_t solx, soly;
};

Robot::Robot(istream& fi) : solx(0), soly(0)
{
    fi >> prog;
}

void Robot::solve_naive()
{
    static const ull_t SIZE = 1000000000;
    ull_t x = 0, y = 0;
    ull_t m = 1;
    vull_t mult;
    mult.push_back(m);
    for (u_t i = 0, e = prog.size(); i !=e; ++i)
    {
        char c = prog[i];
        switch (c)
        {
         case 'E':
             x = (x + m) % SIZE;
             break;
         case 'W':
             x = (x + (SIZE - m)) % SIZE;
             break;
         case 'S':
             y = (y + m) % SIZE;
             break;
         case 'N':
             y = (y + (SIZE - m)) % SIZE;
             break;
         case '(':
             break;
         case ')':
             mult.pop_back();
             m = mult.back();
             break;
         default:
             u_t digit = c - '0';
             m = (digit * m) % SIZE;
             mult.push_back(m);
        }
    }
    solx = x + 1;
    soly = y + 1;
}

void Robot::solve()
{
    solve_naive();
    
}

void Robot::print_solution(ostream &fo) const
{
    fo << ' ' << solx << ' ' << soly;
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

    void (Robot::*psolve)() =
        (naive ? &Robot::solve_naive : &Robot::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Robot robot(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (robot.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        robot.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
