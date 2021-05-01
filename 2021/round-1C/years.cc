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

static unsigned dbg_flags;

class Years
{
 public:
    Years(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    bool is_roaring(ull_t y) const;
    u_t n_digits(ull_t x) const;
    ull_t tenp(u_t p) const
    {
        ull_t n = 1;
        for (; p > 0; --p)
        {
            n *= 10;
        }
        return n;
    }
    ull_t year;
    ull_t solution;
};

Years::Years(istream& fi) : solution(0)
{
    fi >> year;
}

void Years::solve_naive()
{
    ull_t y = year + 1;
    for (; !is_roaring(y); ++y) {}
    solution = y;
}

bool Years::is_roaring(ull_t y) const
{
    bool roaring = false;
    const u_t ndy = n_digits(y);
    for (u_t ndh = 1; (2*ndh <= ndy) && !roaring; ++ndh)
    {
        ull_t tail = tenp(ndy - ndh);
        ull_t roar = y / tail;
        ull_t build = roar;
        while (build < y)
        {
            ++roar;
            u_t nd = n_digits(roar);
            ull_t tp = tenp(nd);
            build = tp * build + roar;
        }
        roaring = (build == y);
    }
    return roaring;
}

u_t Years::n_digits(ull_t x) const
{
    u_t n = 0;
    ull_t tenp = 1;
    for (; x >= tenp; tenp *= 10)
    {
        ++n;
    }
    return n;
}

#if 0
bool Years::is_roaring(ull_t y) const
{
    u_t n_digits = 0;
    ull_t tenp = 1;
    for (; y > tenp; tenp *= 10)
    {
        ++n_digits;
    }
    if (dbg_flags & 0x1) { cerr << "y="<<y << ", n_digits="<<n_digits << '\n'; }
    bool roaring = false;
    if (n_digits >= 2)
    {
        ull_t half = (n_digits + 1) / 2;
        ull_t tenlow = 1;
        for (u_t p = 0; p < half; ++p)
        {
            tenlow *= 10;
        }
        if (dbg_flags & 0x1) {
            cerr << "year="<<year << ", tenlow="<<tenlow <<'\n';}
        ull_t left = y / tenlow;
        ull_t right = y % tenlow;
        if (10*right >= tenlow)
        {
            roaring = left + 1 == right;
        }
    }
    return roaring;
}
#endif

void Years::solve()
{
     solve_naive();
}

void Years::print_solution(ostream &fo) const
{
    fo << ' ' << solution;
}

int main(int argc, char ** argv)
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

    void (Years::*psolve)() =
        (naive ? &Years::solve_naive : &Years::solve);
    if (solve_ver == 1) { psolve = &Years::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Years years(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (years.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        years.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
