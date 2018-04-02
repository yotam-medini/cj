// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <algorithm>
// #include <set>
// #include <map>
#include <vector>
#include <utility>

#include <cstdlib>
// #include <gmpxx.h>

using namespace std;

// typedef mpz_class mpzc_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
// typedef vector<ul_t> vul_t;

static unsigned dbg_flags;

class Horse
{
 public:
   Horse(double p=0, double s=1) : position(p), speed(s) {}
   double position;
   double speed;
};

bool operator<(const Horse &h0, const Horse &h1)
{
    bool lt = (h0.position < h1.position) ||
        ((h0.position == h1.position) && (h0.speed < h1.speed));
    return lt;
}

typedef vector<Horse> vhorse_t;

class Cruise
{
 public:
    Cruise(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    double destination;
    vhorse_t horses;
    vhorse_t horses_sorted;
    double solution;
};

Cruise::Cruise(istream& fi) : solution(0)
{
    unsigned nh;
    fi >> destination >> nh;
    horses.reserve(nh);
    for (unsigned i = 0; i < nh; ++i)
    {
        double position, speed;
        fi >> position >> speed;
        horses.push_back(Horse(position, speed));
    } 
}

void Cruise::solve_naive()
{
    horses_sorted = horses;
    sort(horses_sorted.begin(), horses_sorted.end());
    double atime = 0;
    const vhorse_t &cshorses = horses_sorted;
    for (auto ih = cshorses.rbegin(), eh = cshorses.rend(); ih != eh; ++ih)
    {
        const Horse &horse = *ih;
        double self_time = (destination - horse.position) / horse.speed;
        if (atime < self_time)
        {
            atime = self_time;
        }
    }
    solution = destination / atime;
}

void Cruise::solve()
{
    solve_naive();
}

void Cruise::print_solution(ostream &fo) const
{
    fo << " " << fixed << setprecision(6) << solution;
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

    void (Cruise::*psolve)() =
        (naive ? &Cruise::solve_naive : &Cruise::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Cruise cruise(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (cruise.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        cruise.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
