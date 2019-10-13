// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <iostream>
#include <fstream>
#include <string>
// #include <set>
// #include <map>
#include <vector>
#include <utility>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;

static unsigned dbg_flags;

class Flatten
{
 public:
    Flatten(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    u_t tail(u_t& changed, u_t& steps,
        u_t si, u_t max_steps, bool fixed_first);
    //void tail_fixed1st(u_t& changed, u_t& steps, u_t si, u_t max_steps);
    u_t n, k;
    u_t solution;
    vu_t a;
    vu_t h;
};

Flatten::Flatten(istream& fi) : solution(0)
{
    fi >> n >> k;
    a.reserve(n);
    for (u_t i = 0; i < n; ++i)
    {
        u_t x;
        fi >> x;
        a.push_back(x);
    }
}

void Flatten::solve_naive()
{
    h = a;
    u_t changed, dum_steps = 0;
    tail(changed, dum_steps, 0, k, false);
    solution = changed;
}

u_t Flatten::tail(u_t& changed, u_t& steps,
    u_t si, u_t max_steps, bool fixed_first)
{
    u_t ret = h[si];
    if (si + 1 >= n)
    {
        changed = 0;
        steps = 0;
    }
    else
    {
        if (h[si] == h[si + 1])
        {
            tail(changed, steps, si + 1, max_steps, true);
            if (max_steps > 0)
            {
                u_t s_changed1, s_steps1;
                tail(s_changed1, s_steps1, si + 1, max_steps - 1, false);
                ++s_changed1;
                if (changed > s_changed1)
                {
                    changed = s_changed1;
                    steps = s_steps1;
                }
            }
        }
        else // h[si] != h[si + 1)
        {
            u_t save = h[si + 1];
            h[si + 1] = h[si];
            tail(changed, steps, si + 1, max_steps, true);
            ++changed; // [si + 1]
            h[si + 1] = save;
            if (max_steps > 0)
            {
                u_t s_changed1, s_steps1;
                u_t h1 = tail(s_changed1, s_steps1, si + 1, max_steps - 1,
                    false);
                s_steps1 += (h[si] != h1 ? 1 : 0);
                if (changed > s_changed1)
                {
                    changed = s_changed1;
                    steps = s_steps1;
                    ret = h1;
                }
            }
            if (!fixed_first)
            {
                save = h[si];
                u_t s_changed1, s_steps1;
                u_t h1 = tail(s_changed1, s_steps1, si + 1, max_steps,
                    false);
                ++s_changed1;
                h[si] = save;
                if (changed > s_changed1)
                {
                    changed = s_changed1;
                    steps = s_steps1;
                    ret = h1;
                }
            }
        }
    }
    return ret;
}

void Flatten::solve()
{
    solve_naive();
}

void Flatten::print_solution(ostream &fo) const
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

    void (Flatten::*psolve)() =
        (naive ? &Flatten::solve_naive : &Flatten::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Flatten flatten(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (flatten.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        flatten.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
