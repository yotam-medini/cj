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
    Years(ull_t _year) : year(_year), solution(0) {}
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    ull_t get_solution() const { return solution; }
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
    ull_t build_roar(ull_t roar) const;
    void improve_solution(ull_t y)
    {
        if ((year < y) && ((solution == 0) || (solution > y)))
        {
            solution = y;
        }
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

void Years::solve()
{
    const u_t ndy = n_digits(year);
    for (u_t ndh = 1; 2*ndh <= ndy; ++ndh)
    {
        const ull_t tail = tenp(ndy - ndh);
        const ull_t roar0 = year / tail;
        const ull_t last_digit = roar0 % 10;
        const ull_t roar_max = roar0 + (ull_t(9) - last_digit);
        for (ull_t roar = roar0; roar <= roar_max; ++roar)
        {
            ull_t build = build_roar(roar);
            improve_solution(build);
        }
        for (u_t tp = 0; 2*(ndh + tp) <= ndy + 1; ++tp)
        {
            ull_t build = build_roar((roar0 + 1)*tenp(tp));
            improve_solution(build);
        }
    }
    for (u_t tp = 0; tp <= (ndy + 1)/2; ++tp)
    {
        improve_solution(build_roar(tenp(tp)));
    }
}

ull_t Years::build_roar(ull_t roar) const
{
    bool grown = false;
    ull_t build = roar;
    while ((build <= year) || !grown)
    {
        ++roar;
        u_t nd = n_digits(roar);
        ull_t tp = tenp(nd);
        build = tp * build + roar;
        grown = true;
    }
    return build;
}

void Years::print_solution(ostream &fo) const
{
    fo << ' ' << solution;
}

static int ytest(ull_t y)
{
    int rc = 0;
    Years real(y);
    Years naive(y);
    real.solve();
    naive.solve_naive();
    ull_t s = real.get_solution();
    ull_t s_naive = naive.get_solution();
    if (s != s_naive)
    {
        rc = 1;
        cerr << "Failed: y=" << y << ", solution=" << s <<
            ", naive=" << s_naive << '\n';
    }
    return rc;
}

static int test()
{
    int rc = 0;
    for (ull_t y = 1; (y < 1000000) && (rc == 0); ++y)
    {
        cerr << __func__ << ':' << __LINE__ << ", y=" << y << '\n';
        rc = ytest(y);
    }
    const ull_t t9 = 1000000000;
    const ull_t t10 = 10*t9;
    // const ull_t t18 = t9*t9;
    for (ull_t target = 12, ynext = 34; (target < t10) && (rc == 0);
        target = ynext)
    {
        ull_t ybase = target - 3;
        cerr << __func__ << " ybase=" << ybase << ", target=" << target << '\n';
        for (ull_t y = ybase; (y < ynext) && (rc == 0); ++y)
        {
            rc = ytest(y);
        }
        Years real(target); real.solve();
        ynext = real.get_solution();
        if (ynext <= target + target/100)
        {
            ynext += ynext/2;
        }
    }
    return rc;
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
        else if (opt == string("-test"))
        {
            rc = test();
            if (rc != 0)
            {
                exit(rc);
            }
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
