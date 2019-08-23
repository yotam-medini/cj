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
typedef vector<u_t> vu_t;
typedef set<u_t> setu_t;

static unsigned dbg_flags;

class Xwhat
{
 public:
    Xwhat(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    bool xor_even(u_t x);
    u_t max_even() const;
    u_t n, q;
    vu_t a;
    vu_t p;
    vu_t v;
    setu_t odds;
    vu_t solution;
};

Xwhat::Xwhat(istream& fi)
{
    fi >> n >> q;
    a.reserve(n);
    p.reserve(q);
    v.reserve(q);
    u_t x;
    for (u_t i = 0; i < n; ++i)
    {
        fi >> x;
        a.push_back(x);
    }
    for (u_t i = 0; i < q; ++i)
    {
        fi >> x;
        p.push_back(x);
        fi >> x;
        v.push_back(x);
    }
}

void Xwhat::solve_naive()
{
    vu_t as(a);
    for (u_t i = 0; i < q; ++i)
    {
        as[p[i]] = v[i];
        u_t maxlen = 0;
        for (u_t b = 0; b < n; ++b)
        {
            for (u_t e = b + 1; e < n + 1; ++e)
            {
                u_t x = 0;
                for (u_t j = b; j < e; ++j)
                {
                    x ^= as[j];
                }
                if (xor_even(x) && (maxlen < (e - b)))
                {
                    maxlen = e - b;
                }
            }
        }
        solution.push_back(maxlen);
    }
}

bool Xwhat::xor_even(u_t x)
{
    u_t nbits = 0;
    for (u_t i = 0; i <= 10; ++i)
    {
        if (x & (1u << i))
        {
            ++nbits;
        }
    }
    bool ret = ((nbits % 2) == 0);
    return ret;
}


void Xwhat::solve()
{
    for (u_t i = 0; i < n; ++i)
    {
        const u_t x = a[i];
        if (!xor_even(x))
        {
            odds.insert(odds.end(), i);
        }
    }
    u_t curr_max = max_even();
    for (u_t j = 0; j < q; ++j)
    {
        u_t pj = p[j];
        u_t vj = v[j];
        auto er = odds.equal_range(pj);
        bool old_odd = (er.first != er.second);
        bool new_odd = !xor_even(vj);
        if (new_odd != old_odd)
        {
            setu_t::iterator odd_iter = er.first;
            if (new_odd)
            {
                odds.insert(odd_iter, pj);
            }
            else
            {
                odds.erase(odd_iter);
            }
            curr_max = max_even();
        }
        solution.push_back(curr_max);
    }
}

u_t Xwhat::max_even() const
{
    u_t ret = n;
    u_t n_odds = odds.size();
    if ((n_odds % 2) != 0)
    {
        u_t odd_min = *(odds.begin());
        u_t odd_max = *(odds.rbegin());
        u_t min_left = odd_min;
        u_t min_right = n - (odd_min + 1);
        u_t max_left = odd_max;
        u_t max_right = n - (odd_max + 1);
        ret = max(max(min_left, min_right), max(max_left, max_right));
    }
    return ret;
}

void Xwhat::print_solution(ostream &fo) const
{
    for (u_t i = 0; i < q; ++i)
    {
        fo << ' ' << solution[i];
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

    void (Xwhat::*psolve)() =
        (naive ? &Xwhat::solve_naive : &Xwhat::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Xwhat xwhat(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (xwhat.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        xwhat.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
