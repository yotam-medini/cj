// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <vector>
#include <utility>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef set<u_t> setu_t;
typedef vector<u_t> vu_t;

static unsigned dbg_flags;

// caller should initialize t = vu_t(bound.size(), 0)
void tuple_next(vu_t& t, const vu_t& bound)
{
    u_t i, sz = bound.size();
    bool done = false;
    for (i = 0; (i < sz) && !done; ++i)
    {
        ++t[i];
        done = (t[i] < bound[i]);
        if (!done)
        {
            fill_n(t.begin(), i + 1, 0);
        }
    }
    if (!done)
    {
        t.clear();
    }
}

class Flatten
{
 public:
    Flatten(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    void tail(u_t& changed, setu_t& heads, u_t si, u_t max_steps);
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
    setu_t heights(a.begin(), a.end());
    u_t nh = heights.size();
    vu_t i2h(heights.begin(), heights.end());
    const vu_t bounds(vu_t::size_type(n), nh);
    h = vu_t(vu_t::size_type(n), u_t(-1));
    solution = u_t(-1);
    for (vu_t t = vu_t(vu_t::size_type(n), 0); !t.empty();
        tuple_next(t, bounds))
    {
        for (u_t i = 0; i < n; ++i)
        {
            h[i] = i2h[t[i]];
        }
        u_t n_steps = 0;
        u_t n_changes = (h[0] != a[0] ? 1 : 0);
        for (u_t i = 1; i < n; ++i)
        {
            n_steps += (h[i - 1] != h[i] ? 1 : 0);
            n_changes += (h[i] != a[i] ? 1 : 0);
        }
        if ((n_steps <= k) && (solution > n_changes))
        {
            solution = n_changes;
        }
    }
}

void Flatten::solve()
{
    h = a;
    setu_t heads_ignored;
    u_t changed;
    tail(changed, heads_ignored, 0, k);
    solution = changed;
}

void Flatten::tail(u_t& changed, setu_t& heads, u_t si, u_t max_steps)
{
    setu_t heads_default;
    heads_default.insert(heads_default.end(), h[si]);
    changed = 0;
    heads = heads_default;
    if (si + 1 < n)
    {
        u_t changed_sub;
        setu_t heads_sub;
        tail(changed_sub, heads_sub, si + 1, max_steps);
        changed = changed_sub;
        if (heads_sub.find(h[si]) == heads_sub.end())
        {
            ++changed;
            swap(heads, heads_sub);
        }
        if (max_steps > 0)
        {
            tail(changed_sub, heads_sub, si + 1, max_steps - 1);
            if (changed > changed_sub)
            {
                changed = changed_sub;
                heads = heads_default;
            }
            else if (changed == changed_sub)
            {
                heads.insert(heads.end(), h[si]);
            }
        }
        if (h[si] != h[si + 1])
        {
            u_t save;
            save = h[si + 1];
            h[si + 1] = h[si];
            tail(changed_sub, heads_sub, si + 1, max_steps);
            h[si + 1] = save;
            ++changed_sub;
            if (heads_sub.find(h[si]) != heads_sub.end())
            {
                if (changed > changed_sub)
                {
                    changed = changed_sub;
                    heads = heads_default;
                }
                else if (changed == changed_sub)
                {
                    heads.insert(heads.end(), h[si]);
                }
            }
            else
            {
                ++changed_sub;
                if (changed > changed_sub)
                {
                    changed = changed_sub;
                    swap(heads, heads_sub);
                }
                else if (changed == changed_sub)
                {
                    heads.insert(heads_sub.begin(), heads_sub.end());
                }
            }
        }
    }
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
