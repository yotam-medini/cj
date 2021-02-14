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

static ull_t five_power(u_t p)
{
    static vull_t r;
    if (r.empty())
    {
        r.push_back(1);
    }
    while (r.size() <= p)
    {
        static const ull_t five = 5;
        r.push_back(five * r.back());
    }
    return r[p];
}

static ull_t five_powers_sum(u_t p)
{
    static vull_t r;
    if (r.empty())
    {
        r.push_back(1); // kind of exception
        r.push_back(5);
    }
    while (r.size() <= p)
    {
        r.push_back(five_power(r.size()) + r.back());
    }
    return r[p];
}

class LRBoring
{
 public:
    LRBoring(ull_t _L, ull_t _R) : L(_L), R(_R) {}
    ull_t solve_naive();
    ull_t solve();
 private:
    void digitize(vu_t& digits, ull_t x) const;
    bool is_boring(ull_t x) const;
    ull_t n_boring_till(ull_t till) const;
    ull_t L, R;
};

ull_t LRBoring::solve_naive()
{
    ull_t solution = 0;
    for (ull_t x = L; x <= R; ++x)
    {
        if (is_boring(x))
        {
            ++solution;
        }
    }
    return solution;
}

ull_t LRBoring::solve()
{
    ull_t nr = n_boring_till(R + 1);
    ull_t nl = n_boring_till(L);
    ull_t solution = nr - nl;
    return solution;
}

bool LRBoring::is_boring(ull_t x) const
{
    vu_t digits;
    digitize(digits, x);
    bool isb = true;
    for (size_t i = 0, e = digits.size(); isb && (i < e); ++i)
    {
        const bool odd_pos = (i % 2 == 0);
        isb = (digits[i] % 2 == 1) == odd_pos;
    }
    return isb;
}

void LRBoring:: digitize(vu_t& digits, ull_t x) const
{
    vu_t rdigits;
    while (x > 0)
    {
        rdigits.push_back(x % 10);
        x /= 10;
    }
    digits = vu_t(rdigits.rbegin(), rdigits.rend());
}

ull_t LRBoring::n_boring_till(ull_t till) const
{
    vu_t digits;
    digitize(digits, till);
    u_t nd = digits.size();
    ull_t n = (nd > 1 ? five_powers_sum(nd - 1) : 0);
    for (u_t i = 0; i < nd; ++i)
    {
        // # boring digits in position i
        const u_t digit = digits[i];
        const ull_t fp = five_power(nd - i - 1);
        if (i % 2 == 0) // odd place
        {
            ull_t k = digit / 2;
            n += k * fp;
            if (digit % 2 == 0)
            {
                i = nd; // exit loop
            }
        }
        else // even place
        {
            ull_t k = (digit + 1) / 2;
            n += k * fp;
            if (digit % 2 == 1)
            {
                i = nd; // exit loop
            }
        }
    }
    return n;
}

static int test_lrboring(ull_t lrmin, ull_t lrmax)
{
    int rc = 0;
    for (ull_t L = lrmin; (rc == 0) && (L <= lrmax); ++L)
    {
        for (ull_t R = lrmax; (rc == 0) && (L <= R); --R)
        {
            ull_t n_naive = LRBoring(L, R).solve_naive();
            ull_t n = LRBoring(L, R).solve();
            if (n_naive != n)
            {
                cerr << __func__ << " failed: " << L << ' ' << R << '\n';
                cerr << "n_naive=" << n_naive << ", n=" << n << '\n';
                rc = 1;
            }
        }
    }
    return rc;
}

class Boring
{
 public:
    Boring(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    ull_t L, R;
    ull_t solution;
};

Boring::Boring(istream& fi) : solution(0)
{
    fi >> L >> R;
}

void Boring::solve_naive()
{
    solution = LRBoring(L, R).solve_naive();
}

void Boring::solve()
{
    solution = LRBoring(L, R).solve();
}

void Boring::print_solution(ostream &fo) const
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

        if (opt == string("-lrtest"))
        {
            ull_t L = stol(argv[ai + 1]);
            ull_t R = stol(argv[ai + 2]);
            rc = test_lrboring(L, R);
            return rc;
        }

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

    void (Boring::*psolve)() =
        (naive ? &Boring::solve_naive : &Boring::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Boring boring(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (boring.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        boring.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
