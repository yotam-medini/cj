// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <vector>
#include <utility>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<ull_t> vull_t;

static unsigned dbg_flags;

class Train
{
 public:
    Train(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    u_t n, p;
    vull_t skills;
    ull_t solution;
};

Train::Train(istream& fi) : solution(0)
{
    fi >> n >> p;
    skills.reserve(n);
    for (u_t i = 0; i < n; ++i)
    {
        u_t skill;
        fi >> skill;
        skills.push_back(skill);
    }
}

void Train::solve_naive()
{
    sort(skills.begin(), skills.end());
    ull_t best = ull_t(-1);
    for (u_t lead = p - 1; lead < n; ++lead)
    {
        u_t lead_skill = skills[lead];
        ull_t hours = 0;
        for (u_t i = lead - (p - 1); i < lead; ++i)
        {
            hours += (lead_skill - skills[i]);
        }
        if (best > hours)
        {
            best = hours;
        }
    }
    solution = best;
}

void Train::solve()
{
    sort(skills.begin(), skills.end());
    solution = ull_t(-1);
    ull_t psum = 0;
    for (u_t i = 0; i < p; ++i)
    {
        psum += skills[i];
    }
    for (u_t lead = p - 1, lead1 = lead + 1; lead < n; lead = lead1++)
    {
        ull_t hours = p * skills[lead] - psum;
        if (solution > hours)
        {
            solution = hours;
        }
        if (lead1 < n)
        {
            psum += skills[lead1];
            psum -= skills[lead1 - p];
        }
    }
}

void Train::print_solution(ostream &fo) const
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

    void (Train::*psolve)() =
        (naive ? &Train::solve_naive : &Train::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Train problem(*pfi);
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
