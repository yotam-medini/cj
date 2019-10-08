// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <utility>
// #include <algorithm>
#include <array>
#include <numeric>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef long long ll_t;
typedef array<ul_t, 2> aul2_t;
typedef vector<aul2_t> vaul2_t;
typedef vector<ul_t> vul_t;

static unsigned dbg_flags;

class Codeeat
{
 public:
    Codeeat(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    void solve_naive1();
    void solve_s1();
    void solve_zero(u_t i, u_t ce);
    void solve_s2_nonzero();
    u_t s, d;
    vaul2_t ce_slots;
    vaul2_t ab_days;
    vector<bool> solution;
    // a2_t ce_slots[2];
};

Codeeat::Codeeat(istream& fi) 
{
    fi >> d >> s;
    ce_slots.reserve(s);
    ab_days.reserve(d);
    for (u_t si = 0; si < s; ++si)
    {
        ul_t c, e;
        fi >> c >> e;
        ce_slots.push_back(aul2_t({c, e}));
    }
    for (u_t di = 0; di < d; ++di)
    {
        ul_t a, b;
        fi >> a >> b;
        ab_days.push_back(aul2_t({a, b}));
    }
}

void Codeeat::solve_naive()
{
    // assume s<=2 and Ci,Ei <= 7
    const ull_t denom = 24*7;
    ull_t a1 = ce_slots[0][0], b1 = ce_slots[0][1];
    ull_t a2 = ce_slots[1][0], b2 = ce_slots[1][1];
    for (u_t di = 0; di < d; ++di)
    {
        const aul2_t& ab_day = ab_days[di];
        ull_t A = ab_day[0], B = ab_day[1];
        ull_t Ad = A * denom, Bd = B * denom;
        bool yes = false;
        for (ull_t alpha1 = 0; (alpha1 <= denom) && !yes; ++alpha1)
        {
            ull_t beta1 = denom - alpha1;
            for (ull_t alpha2 = 0; (alpha2 <= denom) && !yes; ++alpha2)
            {
                ull_t beta2 = denom - alpha2;
                ull_t sa = alpha1 * a1 + alpha2 * a2;
                ull_t sb = beta1 * b1 + beta2 * b2;
                yes = (sa >= Ad) && (sb >= Bd);
            }
        }
        solution.push_back(yes);
    }
}

void Codeeat::solve_naive1()
{
    // assuming s <= 2
    if (s == 1)
    {
        solve_s1();
    }
    else
    {
        u_t zi = 2, zce = 2;
        for (u_t i = 0; i < s; ++i)
        {
            for (u_t ce = 0; ce < 2; ++ce)
            {
                if (ce_slots[i][ce] == 0)
                {
                   zi = i;  zce = ce;
                }
            }
        }
        if (zi < 2)
        {
            solve_zero(zi, zce);
        }
        else
        {
            solve_s2_nonzero();
        }
    }
}

void Codeeat::solve_s1()
{
    const ull_t c = ce_slots[0][0];
    const ull_t e = ce_slots[0][1];
    for (u_t di = 0; di < d; ++di)
    {
        const aul2_t& ab_day = ab_days[di];
        // yes = (cslot[0][0] / ab_days[0]) + (cslot[0][1] / ab_days[2]) <= 1
        ull_t a = ab_day[0];
        ull_t b = ab_day[1];
        bool yes = (c * b + e * a) <= (a * b);
        solution.push_back(yes);
    }
}

void Codeeat::solve_zero(u_t i, u_t ce)
{
    // similar to solve_s1
    const ull_t c = ce_slots[1 - i][0];
    const ull_t e = ce_slots[1 - i][1];
    for (u_t di = 0; di < d; ++di)
    {
        aul2_t ab_day = ab_days[di];
        ab_day[1 - ce] -= min(ce_slots[i][1 - ce], ab_day[1 - ce]);
        // yes = (ab_days[0] / cslot[0][0]) + (ab_days[1] / cslot[0][1]) <= 1
        ull_t A = ab_day[0];
        ull_t B = ab_day[1];
        // yes =  A/c + B/e <= 1
        bool yes = false;
        if (c == 0)
        {
            yes = (A == 0) && (B <= e);
        }
        else if (e == 0)
        {
            yes = (B == 0) && (A <= c);
        }
        else
        {
            yes = (A * e + B * c) <= c * e;
        }
        solution.push_back(yes);
    }
}

void Codeeat::solve_s2_nonzero()
{
    // x the fraction of ce_slots[0][0]
    ll_t c1 = ce_slots[0][0], e1 = ce_slots[0][1];
    ll_t c2 = ce_slots[1][0], e2 = ce_slots[1][1];
    ll_t c = c1 + c2, e = e1 + e2;
    ll_t det = c1*e2 - c2*e1;
    for (u_t di = 0; di < d; ++di)
    {
        const aul2_t& ab_day = ab_days[di];
        ll_t A = ab_day[0], B = ab_day[1];
        bool yes = (A <= c) && (B <= e);
        if (yes)
        {
            yes = false;
            for (u_t i = 0; (i != 2) && (!yes); ++i)
            {
                for (u_t j = 0; (j != 2) && (!yes); ++j)
                {
                    if (ce_slots[i][j] >= ab_day[j])
                    {
                        ull_t g = ce_slots[i][j];
                        ull_t fj = ce_slots[i][1 - j];
                        ull_t fo = ce_slots[1 - i][1 - j];
                        ull_t lhs = g*(fj + fo);
                        ull_t rhs = ab_day[j]*fj + ab_day[1 - j]*g;
                        yes = lhs >= rhs;
                    }
                }
            }
            if (!yes)
            {
                ll_t rhs = A*e2 + (B - (e1 + e2))*c2;
                if (det == 0)
                {
                    yes = (rhs <= 0);
                }
                else if (det < 0)
                {
                    yes = (rhs <= 0); // && ...
                    // rhs/det >= 1 - (B - e2)/e1
                    // e1*rhs <= det*(e1 + e2 - B)
                    yes = yes && (e1*rhs <= det*(e1 + e2 - B));
                }
                else // det > 0
                {
                    yes = (rhs <= 0); // || ...
                    if (!yes)
                    {
                        yes = rhs <= det; // rhs/det <= 1
                        // yes = (A*e2 + B*c2 <= c1*e2 + c2*e2); // rhs <= 0
                        // 1 - (B - e2)/e1 >= a1 >= rhs/det 
                        // rhs*e1 + det*B <= det*(e1 + e1)
                        yes = yes && (rhs*e1 + det*B <= det*(e1 + e2));
                    }
                }
            }
        }
        solution.push_back(yes);
    }
}


void Codeeat::solve()
{
    solve_naive1();
}

void Codeeat::print_solution(ostream &fo) const
{
    fo << ' ';
    for (bool b: solution)
    {
        fo << (b ? 'Y' : 'N');
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

    void (Codeeat::*psolve)() =
        (naive ? &Codeeat::solve_naive : &Codeeat::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Codeeat codeeat(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (codeeat.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        codeeat.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
