// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
typedef vector<ull_t> vull_t;

static unsigned dbg_flags;

class Shifts
{
 public:
    Shifts(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    u_t n;
    ull_t h;
    vull_t ab[2];
    ull_t solution;
};

Shifts::Shifts(istream& fi) : solution(0)
{
    fi >> n >> h;
    for (u_t iab = 0; iab != 2; ++iab)
    {
        ab[iab].reserve(n);
        ull_t x;
        for (u_t i = 0; i < n; ++i)
        {
            fi >> x;
            ab[iab].push_back(x);
        }
    }
}

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

void Shifts::solve_naive()
{
    vu_t bound(vu_t::size_type(n), 3);
    for (vu_t tuple = vu_t(vu_t::size_type(n), 0); !tuple.empty();
        tuple_next(tuple, bound))
    {
        ull_t points[2];
        points[0] = points[1] = 0;
        for (ull_t ti = 0; ti < n; ++ti)
        {
            u_t mask = tuple[ti] + 1;
            if (mask & 1)
            {
                points[0] += ab[0][ti];
            }
            if (mask & 2)
            {
                points[1] += ab[1][ti];
            }
        }
        if ((points[0] >= h) && (points[1] >= h))
        {
            ++solution;
        }
    }
}

#if 0
void Shifts::solve_naive()
{
    ull_t happy2[2];
    for (u_t iab = 0; iab != 2; ++iab)
    {
        ull_t happy = 0;
        const vull_t& c = ab[iab];
        for (ull_t bits = 0, bitse = (1ull << n); bits < bitse; ++bits)
        {
            ull_t points = 0;
            for (u_t si = 0; si < n; ++si)
            {
                if (bits & (1ull << si))
                {
                    points += c[si];
                }
            }
            if (points >= h)
            {
                ++happy;
            }
        }
        happy2[iab] = happy;
    }
    solution = happy2[0] * happy2[1];
}
#endif

void Shifts::solve()
{
    solve_naive();
}

void Shifts::print_solution(ostream &fo) const
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

    void (Shifts::*psolve)() =
        (naive ? &Shifts::solve_naive : &Shifts::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Shifts shifts(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (shifts.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        shifts.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
