// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <iostream>
#include <fstream>
#include <string>
#include <utility>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;

static unsigned dbg_flags;

class EvenDigits
{
 public:
    EvenDigits(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    bool has_odd(ull_t x) const;
    ull_t target_down(ull_t x, u_t pos, u_t dig) const;
    ull_t target_up(u_t pos, u_t dig) const;
    ull_t pwr10(u_t p) const
    {
        ull_t ret = 1;
        while (p--)
        {
            ret *= 10;
        }
        return ret;
    }
    ul_t n;
    ull_t solution;
};

EvenDigits::EvenDigits(istream& fi) : solution(0)
{
    fi >> n;
}

void EvenDigits::solve_naive()
{
    ull_t n_down = 0, n_up = 0;
    for (ull_t nn = n; has_odd(nn); --nn, ++n_down) {}
    for (ull_t nn = n; has_odd(nn); ++nn, ++n_up) {}
    solution = min(n_down, n_up);
}

void EvenDigits::solve()
{
    // get most significant odd digit
    int odd_pos = -1;
    u_t odd_dig = 0;
    u_t pos = 0;
    for (ull_t nn = n; nn != 0; nn /= 10, ++pos)
    {
        ul_t dig = (nn % 10);
        if ((dig % 2) != 0)
        {
            odd_pos = pos;
            odd_dig = dig;
        }
    }
    if (odd_pos == -1)
    {
        solution = 0;
    }
    else
    {
        ul_t tdown = target_down(n, odd_pos, odd_dig);
        ul_t tup = target_up(odd_pos, odd_dig);
        ull_t step_down = n - tdown;
        ull_t step_up = tup - n;
        solution = min(step_down, step_up);
    }
}

bool EvenDigits::has_odd(ull_t x) const
{
     while ((x > 0) && (((x % 10) % 2) == 0))
     {
         x /= 10;
     }
     return (x != 0);
}

ull_t EvenDigits::target_down(ull_t x, u_t pos, u_t dig) const
{
    ull_t ret = 0;
    if (dig == 1)
    {
        if (pos > 0)
        {
            for (u_t p = 0; p < pos; ++p)
            {
                ret = 10*ret + 8;
            }
        }
    }
    else
    {
        for (u_t p = 0; p < pos; ++p)
        {
            ret = 10*ret + 8;
        }
        ret += (dig - 1)*pwr10(pos);
    }
    ret += (n - (n % pwr10(pos + 1)));
    return ret;
}

ull_t EvenDigits::target_up(u_t pos, u_t dig) const
{
    ull_t ret = 0;
    if (dig == 9)
    {
        ret = 2;
        for (u_t p = 0; p <= pos + 1; ++p)
        {
            ret *= 10;
        }
    }
    else
    {
        ret = (dig + 1)*pwr10(pos);
    }
    ret += (n - (n % pwr10(pos + 1)));
    return ret;
}

void EvenDigits::print_solution(ostream &fo) const
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

    void (EvenDigits::*psolve)() =
        (naive ? &EvenDigits::solve_naive : &EvenDigits::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        EvenDigits evenDigits(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (evenDigits.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        evenDigits.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
