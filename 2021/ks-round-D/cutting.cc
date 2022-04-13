// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <numeric>
#include <queue>
#include <string>
#include <utility>
#include <vector>
// #include <iterator>
// #include <map>
// #include <set>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef array<ull_t, 2> aull2_t;
typedef vector<aull2_t> vaull2_t;
typedef vector<ull_t> vull_t;

static unsigned dbg_flags;

class Event
{
 public:
   Event(ull_t _p=0, u_t _be=0) : p(_p), be(_be) {}
   ull_t p;
   u_t be;
};
typedef vector<Event> vev_t;

bool operator<(const Event& ev0, const Event& ev1)
{
    bool lt = (ev0.p < ev1.p) ||
        ((ev0.p == ev1.p) && (ev0.be < ev1.be));
    return lt;
}

class Cutting
{
 public:
    Cutting(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    ull_t N, C;
    vaull2_t LR;
    ull_t solution;
};

Cutting::Cutting(istream& fi) : solution(0)
{
    fi >> N >> C;
    LR.reserve(N);
    for (ull_t i = 0; i < N; ++i)
    {
        aull2_t lr;
        fi >> lr[0] >> lr[1];
        LR.push_back(lr);
    }
}

void Cutting::solve_naive()
{
    ull_t minL = LR[0][0], maxR = LR[0][1];
    for (const aull2_t& lr: LR)
    {
        minL = min(minL, lr[0]);
        maxR = max(maxR, lr[1]);
    }
    vull_t cuts; cuts.reserve(maxR - minL);
    for (ull_t x = minL + 1; x < maxR; ++x)
    {
        ull_t c = 0;
        for (const aull2_t& lr: LR)
        {
            if ((lr[0] < x) && (x < lr[1]))
            {
                ++c;
            }
        }
        cuts.push_back(c);
    }
    sort(cuts.begin(), cuts.end());
    const ull_t max_cuts = min<ull_t>(C, cuts.size());
    solution = accumulate(cuts.end() - max_cuts, cuts.end(), N);
}

void Cutting::solve()
{
    vev_t events;
    events.reserve(2*N);
    for (const aull2_t& lr: LR)
    {
         if (lr[1] - lr[0] > 1)
         {
             events.push_back(Event(lr[0] + 1, 0));
             events.push_back(Event(lr[1], 1));
         }
    }
    sort(events.begin(), events.end());
    priority_queue<aull2_t> q;
    ull_t active = 0;
    ull_t last = events.front().p;
    for (const Event& ev: events)
    {
        u_t possible_cuts = ev.p - last;
        if (ev.be == 0)
        {
            if ((active > 0) && (possible_cuts > 0))
            {
                q.push(aull2_t{active, ev.p - last});
            }
            ++active;
        }
        else
        {
            if ((active > 0) && (possible_cuts > 0))
            q.push(aull2_t{active, ev.p - last});
            --active;
        }
        last = ev.p;
    }
    solution = N;
    for (ull_t cuts_used = 0; (cuts_used < C) && !q.empty(); )
    {
        const aull2_t& ic = q.top();
        ull_t intervals = ic[0];
        ull_t possible_cuts = ic[1];
        q.pop();
        ull_t take = min(possible_cuts, C - cuts_used);
        solution += intervals * take;
        cuts_used += take;
    }
}

void Cutting::print_solution(ostream &fo) const
{
    fo << ' ' << solution;
}

static int real_main(int argc, char ** argv)
{
    const string dash("-");

    bool naive = false;
    bool tellg = false;
    u_t solve_ver = 0;
    int rc = 0, ai;

    for (ai = 1; (rc == 0) && (ai < argc) && (argv[ai][0] == '-') &&
        argv[ai][1] != '\0'; ++ai)
    {
        const string opt(argv[ai]);
        if (opt == string("-naive"))
        {
            naive = true;
        }
        else if (opt == string("-solve1"))
        {
            solve_ver = 1;
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

    void (Cutting::*psolve)() =
        (naive ? &Cutting::solve_naive : &Cutting::solve);
    if (solve_ver == 1) { psolve = &Cutting::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Cutting cutting(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (cutting.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        cutting.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}

static int test_specific(int argc, char ** argv)
{
    int rc = 0;
    return rc;
}

static int test_random(int argc, char ** argv)
{
    int rc = 0;
    int ai = 0;
    u_t n_tests = strtoul(argv[ai++], 0, 0);
    for (u_t ti = 0; (rc == 0) && (ti < n_tests); ++ti)
    {
        cerr << "Tested: " << ti << '/' << n_tests << '\n';
    }
    return rc;
}

static int test(int argc, char ** argv)
{
    int rc = ((argc > 1) && (string(argv[1]) == string("specific"))
        ? test_specific(argc - 1, argv + 1)
        : test_random(argc, argv));
    return rc;
}

int main(int argc, char **argv)
{
    int rc = ((argc > 1) && (string(argv[1]) == string("test"))
        ? test(argc - 2, argv + 2)
        : real_main(argc, argv));
    return rc;
}
