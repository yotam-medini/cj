// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <utility>
#include <algorithm>
#include <array>
#include <numeric>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef long long ll_t;
typedef array<ul_t, 2> aul2_t;
typedef vector<u_t> vu_t;
typedef vector<aul2_t> vaul2_t;
typedef vector<ul_t> vul_t;

static unsigned dbg_flags;

u_t gcd(u_t m, u_t n)
{
   while (n)
   {
      u_t t = n;
      n = m % n;
      m = t;
   }
   return m;
}

u_t lcm(u_t m, u_t n)
{
    return (m*n)/gcd(m, n);
}

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

class Codeeat
{
 public:
    Codeeat(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    void solve_naive0();
    void solve_naive1();
    void solve_s1();
    void solve_zero(u_t i, u_t ce);
    void solve_s2_nonzero();
    bool frac_lt(const aul2_t& x0, const aul2_t& x1) const;
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

void Codeeat::solve_naive0()
{
    // assume s<=2 and Ci,Ei <= 7
    ull_t a1 = ce_slots[0][0], b1 = ce_slots[0][1];
    ull_t a2 = ce_slots[1][0], b2 = ce_slots[1][1];
    ul_t tdenom = lcm(a1, b1);
    tdenom = lcm(tdenom, b1);
    tdenom = lcm(tdenom, b2);
    const ull_t denom = tdenom;
    for (u_t di = 0; di < d; ++di)
    {
        const aul2_t& ab_day = ab_days[di];
        ull_t A = ab_day[0], B = ab_day[1];
        ull_t Ad = A * denom, Bd = B * denom;
        bool yes = false;
        bool yes1 = false;
        for (ull_t alpha1 = 0; (alpha1 <= denom); ++alpha1)
        {
            ull_t beta1 = denom - alpha1;
            for (ull_t alpha2 = 0; (alpha2 <= denom); ++alpha2)
            {
                ull_t beta2 = denom - alpha2;
                ull_t sa = alpha1 * a1 + alpha2 * a2;
                ull_t sb = beta1 * b1 + beta2 * b2;
                bool tyes = (sa >= Ad) && (sb >= Bd);
                yes = yes || tyes;
                yes1 = yes1 || (tyes && (
                   (alpha1 == 0) || (alpha1 == denom) ||
                   (alpha2 == 0) || (alpha2 == denom)));
            }
        }
        if (yes && !yes1)
        {
            cerr << "Yes without 1.0 coefficient\n";
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

void Codeeat::solve_naive()
{
    vu_t bound;
    for (u_t si = 0; si < s; ++si)
    {
        ul_t c = ce_slots[si][0];
        ul_t e = ce_slots[si][1];
        bound.push_back(lcm(c, e) + 1);
    }    
    for (u_t di = 0; di < d; ++di)
    {
        const aul2_t& ab_day = ab_days[di];
        bool yes = false;
        vu_t coefs(vu_t::size_type(s), 0);
        for (; !(coefs.empty() || yes); tuple_next(coefs, bound))
        {
            ul_t sa = 0, sb = 0;
            for (u_t si = 0; si < s; ++si)
            {
               ul_t c = ce_slots[si][0];
               ul_t e = ce_slots[si][1];
               u_t denom = bound[si] - 1;
               sa += (coefs[si] * c) / denom;
               sb += ((denom - coefs[si]) * e) / denom;
            }
            yes = (ab_day[0] <= sa) && (ab_day[1] <= sb);
        }
        solution.push_back(yes);
    }
}

void Codeeat::solve_s1()
{
    const ull_t c = ce_slots[0][0];
    const ull_t e = ce_slots[0][1];
    const ull_t cpe = c*e;
    for (u_t di = 0; di < d; ++di)
    {
        const aul2_t& ab_day = ab_days[di];
        // yes = (ab_days[0] / cslot[0][0]) + (ab_days[1] / cslot[0][1]) <= 1
        ull_t a = ab_day[0];
        ull_t b = ab_day[1];
        bool yes = (a*e + b*c) <= cpe;
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
    if (s == 1)
    {
        solve_s1();
    }
    else
    {
        vaul2_t ce_sorted(ce_slots);
        sort(ce_sorted.begin(), ce_sorted.end(),
            [this](const aul2_t& x0, const aul2_t& x1)
            {
                return this->frac_lt(x0, x1);
            });
        vul_t einc(vul_t::size_type(s), 0);
        vul_t cdec(vul_t::size_type(s), 0); 
        einc[0] = ce_sorted[0][1];
        cdec[s - 1] = ce_sorted[s - 1][0];
        for (u_t si = 1; si < s; ++si)
        {
            einc[si] = einc[si - 1] + ce_sorted[si][1];
            cdec[s - si - 1] = cdec[s - si] +  ce_sorted[s - si - 1][0];
        }
        for (u_t di = 0; di < d; ++di)
        {
            aul2_t needed(ab_days[di]);
            aul2_t ce_left({cdec.front(), einc.back()});
            bool yes = false;
            auto ei_lb = lower_bound(einc.begin(), einc.end(), needed[1]);
            auto ci_lb = lower_bound(cdec.rbegin(), cdec.rend(), needed[0]);
            u_t ib = ei_lb - einc.begin();
            u_t ie = s - (ci_lb - cdec.rbegin());
            aul2_t reduce({ie < s ? cdec[ie] : 0, ib > 0 ? einc[ib - 1] : 0});
            for (u_t i = 0; i != 2; ++i)
            {
                needed[i] -= reduce[i];
                ce_left[i] -= reduce[i];
            }
            while ((ib < ie) && !yes)
            {
                bool lt = frac_lt(needed, ce_left);
                u_t cei = (lt ? ib++ : --ie);
                const aul2_t& ce = ce_sorted[cei];
                u_t j = (lt ? 1 : 0);
                if (needed[j] >= ce[j])
                {
                    needed[j] -= ce[j];
                }
                else // ce[j] > 0
                {
                    ul_t r = needed[j];
                    needed[j] = 0;
                    // r/ce[j] + ro/ce[1-j] = 1
                    ul_t ro = (ce[1 - j]*(ce[j] - r)) / ce[j];
                    needed[1 - j] -= min(needed[1 - j], ro);
                }
                ce_left[0] -= ce[0];
                ce_left[1] -= ce[1];
                yes = (needed[0] + needed[1] == 0);
            }
            solution.push_back(yes);
        }
    }
}

bool Codeeat::frac_lt(const aul2_t& x0, const aul2_t& x1) const
{
    ull_t lhs = x0[0] * x1[1];
    ull_t rhs = x0[1] * x1[0];
    bool lt = (lhs < rhs) || 
        ((lhs == rhs) && (
            ((lhs == 0) && (x0[0] + x1[1] < x0[1] + x1[0])) ||
            ((lhs != 0) && (x0[0] < x1[0]))));
    return lt;
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
