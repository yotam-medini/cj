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
// typedef vector<ul_t> vul_t;

static unsigned dbg_flags;

class No9
{
 public:
    No9(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    static void compute_lead_power_mod_count();
 private:
    ull_t n_legal_till(ull_t till) const;
    enum {PwrMax = 18};
    static ull_t lead_power_mod_count[9][18][9];
    bool is_legal(ull_t x) const;
    ull_t f, l;
    ull_t solution;
};

// [L][p][m] Number of non 9-digit numbers n upto L*(10^p) % 9, such that n % 9 == m.  1<=L<=8, [L=0] unused
ull_t No9::lead_power_mod_count[9][18][9];

void No9::compute_lead_power_mod_count()
{
    for (u_t l = 0; l < 9; ++l)
    {
        for (u_t p = 0; p < PwrMax; ++p)
        {
            for (u_t m = 0; m < PwrMax; ++m)
            {
                lead_power_mod_count[l][p][m] = 0;
            }
        }
    }
}

No9::No9(istream& fi) : solution(0)
{
    fi >> f >> l;
}

void No9::solve_naive()
{
    for (ull_t x = f; x <= l; ++x)
    {
        if (is_legal(x))
        {
            ++solution;
        }
    }
}

void No9::solve()
{
    ull_t n_till_low = n_legal_till(f);
    ull_t n_till_high = n_legal_till(l + 1);
    solution = n_till_high - n_till_low;
}

bool No9::is_legal(ull_t x) const
{
    bool legal = (x % 9) != 0;
    while (legal && (x > 0))
    {
        legal = ((x % 10) != 9);
        x /= 10;
    }
    return legal;
}

ull_t No9::n_legal_till(ull_t till) const
{
    ull_t n = 0;
    ull_t t = till;

    // leave just higest 9 digit
    int pos9 = -1;
    for (u_t pos = 0; t != 0; ++pos, t /= 10)
    {
        u_t digit = t % 10;
	if (digit == 9)
	{
	    pos9 = pos;
	}
    }
    t = till;
    if (pos9 != -1)
    {
        for (int pos = 0; pos < pos9; ++pos, t /= 10) {}
        for (int pos = 0; pos < pos9; ++pos, t *= 10) {}
    }

    ull_t bp = 1;
    while (t != 0)
    {
        ull_t digit = t % 10;
        if (bp == 1)
        {
	    u_t next_digit = (t / 10) % 10;
            if ((digit > 0) && (next_digit != 9))
            {
                n = digit;
                ull_t m9 = (till - digit) % 9;
                if ((m9 == 0 || (9 - m9 < digit)))
                {
                    --n;
                }
            }
            bp = 8;
        }
        else
        {
            n += digit * bp;
            bp *= 9;
        }
        t /= 10;
    }
    return n;
}

void No9::print_solution(ostream &fo) const
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

    if (!naive)
    {
       No9::compute_lead_power_mod_count();
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

    void (No9::*psolve)() =
        (naive ? &No9::solve_naive : &No9::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        No9 no9(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (no9.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        no9.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
