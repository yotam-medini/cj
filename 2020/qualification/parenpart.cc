// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
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
typedef vector<ul_t> vul_t;

static unsigned dbg_flags;

class Task
{
 public:
    Task(u_t s=0, u_t e=0, u_t i=0, u_t w=0):
        start(s), end(e), idx(i), who(w) {}
    u_t start;
    u_t end;
    u_t idx;
    u_t who;
};
bool operator<(const Task& t0, const Task& t1)
{
    bool lt = t0.start < t1.start;
    if (t0.start == t1.start)
    {
        lt = t0.end < t1.end;
        if (t0.end == t1.end)
        {
            lt = t0.idx < t1.idx;
        }
    }
    return lt;
}

typedef vector<Task> vtask_t;

class ParenPart
{
 public:
    ParenPart(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    u_t N;
    vtask_t tasks;
    string solution;
};

ParenPart::ParenPart(istream& fi)
{
    fi >> N;
    tasks.reserve(N);
    for (u_t i = 0; i < N; ++i)
    {
        u_t s, e;
        fi >> s >> e;
        tasks.push_back(Task(s, e, i));
    }
}

void ParenPart::solve_naive()
{
    sort(tasks.begin(), tasks.end());
    tasks[0].who = 0;
    u_t end0 = tasks[0].end;
    u_t end1 = 0;
    bool possible = true;
    for (u_t i = 1; possible && (i < N); ++i)
    {
        Task& t = tasks[i];
        if (end0 <= t.start)
        {
            t.who = 0;
            end0 = t.end;
        }
        else if (end1 <= t.start)
        {
            t.who = 1;
            end1 = t.end;
        }
        else
        {
            possible = false;
        }
    }
    if (possible)
    {
        solution = string(size_t(N), ' ');
        for (u_t i = 0; i < N; ++i)
        {
            const Task& t = tasks[i];
            solution[t.idx] = "CJ"[t.who];
        }
    }
    else
    {
        solution = "IMPOSSIBLE";
    }
}

void ParenPart::solve()
{
    solve_naive();
}

void ParenPart::print_solution(ostream &fo) const
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

    void (ParenPart::*psolve)() =
        (naive ? &ParenPart::solve_naive : &ParenPart::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        ParenPart parenpart(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (parenpart.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        parenpart.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
