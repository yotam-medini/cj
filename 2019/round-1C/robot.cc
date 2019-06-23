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
typedef vector<string> vs_t;
typedef vector<u_t> vu_t;

static unsigned dbg_flags;

class Robot
{
 public:
    Robot(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    bool find(string &psol, vs_t &players);
    u_t a;
    vs_t progs;
    bool possible;
    string solution;
};

Robot::Robot(istream& fi) : possible(true)
{
    fi >> a;
    progs.reserve(a);
    for (u_t i = 0; i < a; ++i)
    {
        string s;
        fi >> s;
        progs.push_back(s);
    }
}

void Robot::solve_naive()
{
    vs_t players(progs);
    possible = find(solution, players);
}

bool Robot::find(string &psol, vs_t &players)
{
    bool ret = false;;
    u_t bits = 0;
    u_t si = psol.size();
    for (const string &p: players)
    {
        u_t sz = p.size();
        char c = p[si % sz];
        if (c == 'R')
        {
            bits |= 4;
        }
        else if (c == 'P')
        {
            bits |= 2;
        }
        else // c == 'S'
        {
            bits |= 1;
        }
    }
    switch (bits)
    {
     case 0:
        ret = true;
     case 1: // S
        psol.push_back('R');
        ret = true;
        break;
     case 2: // P
        psol.push_back('S');
        ret = true;
        break;
     case 4: // R
        psol.push_back('P');
        ret = true;
        break;
     case 7:
        ret = false;
        break;
     default:
        {
            char cnext = ' ', c_lose = ' ';
            switch (bits)
            {
             case 3: // S+P
                cnext = 'S';
                c_lose = 'P';
                break;
             case 5: // R+S
                cnext = 'R';
                c_lose = 'S';
                break;
             case 6: // R+P
                cnext = 'P';
                c_lose = 'R';
                break;
            }

            psol.push_back(cnext);
            for (u_t pi = 0; pi < players.size(); )
            {
                const string &p = players[pi];
                u_t sz = p.size();
                char c = p[si % sz];
                if (c == c_lose)
                {
                    swap(players[pi], players.back());
                    players.pop_back();
                }
                else
                {
                    ++pi;
                }
            }
            ret = find(psol, players);
        }
    }
    ret = ret && (psol.size() <= 500);
    return ret;
}

void Robot::solve()
{
    solve_naive();
}

void Robot::print_solution(ostream &fo) const
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
