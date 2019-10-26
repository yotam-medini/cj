// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <numeric>
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
typedef array<ull_t, 2> aull2_t;
typedef vector<aull2_t> vaull2_t;

static unsigned dbg_flags;

class Shift
{
 public:
    Shift(ull_t a=0, ull_t b=0) : ab{a, b}, sort_idx(u_t(-1)) {}
    aull2_t ab;
    u_t sort_idx;
};
typedef vector<Shift> vshift_t;

class Shifts
{
 public:
    Shifts(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    enum { NMAX = 20 };
    static void set_power3()
    {
        if (power3[0] == 0)
        {
            power3[0] = 1;
            for (u_t p = 1; p <= 20; ++p)
            {
                power3[p] = 3 * power3[p - 1];
            }
        }
    }
    void set_shifts();
    ull_t tail_hcount(u_t bi, const aull2_t& hgot);
    static ull_t power3[NMAX + 1];
    u_t n;
    ull_t h;
    vull_t ab[2];
    vshift_t shifts_dec;
    vaull2_t shift_tail;
    ull_t solution;
};

ull_t Shifts::power3[Shifts::NMAX + 1];

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

void Shifts::solve()
{
    set_power3();
    set_shifts();
    solution = tail_hcount(0, aull2_t({0, 0}));
}

void Shifts::set_shifts()
{
    aull2_t ab_sum({0, 0});
    for (u_t i = 0; i < n; ++i)
    {
        shifts_dec.push_back(Shift(ab[0][i], ab[1][i]));
        ab_sum[0] += ab[0][i];
        ab_sum[1] += ab[1][i];
    }
    vu_t idx2[2];
    for (u_t i = 0; i != 2; ++i)
    {
        vu_t& idx = idx2[i];
        idx = vu_t(vu_t::size_type(n), 0);
        iota(idx.begin(), idx.end(), 0);
        sort(idx.begin(), idx.end(),
            [this, i](const u_t i0, const u_t i1)
            {
                return ((shifts_dec[i0].ab[i] > shifts_dec[i1].ab[i]) ||
                     ((shifts_dec[i0].ab[i] == shifts_dec[i1].ab[i]) &&
                      (shifts_dec[i0].ab[1-i] > shifts_dec[i1].ab[1-i])));
            });
    }
    u_t iab = (ab_sum[0] < ab_sum[1] ? 1 : 0);
    u_t ab_sidx2[2] = {0, 0};
    for (u_t dec_idx = 0; dec_idx != n; ++dec_idx, iab = 1 - iab)
    {
        const vu_t& idx = idx2[iab];
        u_t& ab_sidx = ab_sidx2[iab];
        while (shifts_dec[idx[ab_sidx]].sort_idx != u_t(-1))
        {
            ++ab_sidx;
        }
        shifts_dec[idx[ab_sidx]].sort_idx = dec_idx;
    }
    sort(shifts_dec.begin(), shifts_dec.end(),
        [](const Shift& s0, const Shift& s1)
        {
            return s0.sort_idx < s1.sort_idx;
        });
    shift_tail = vaull2_t(vaull2_t::size_type(n + 1), aull2_t({0, 0}));
    for (int si = n; si > 0; )
    {
        --si;
        for (u_t i = 0; i != 2; ++i)
        {
            shift_tail[si][i] = shifts_dec[si].ab[i] + shift_tail[si + 1][i];
        }
    }
}

ull_t Shifts::tail_hcount(u_t bi, const aull2_t& hgot)
{
    ull_t ret = 0;
    if ((h <= shift_tail[bi][0] + hgot[0]) &&
        (h <= shift_tail[bi][1] + hgot[1]))
    {
        if ((hgot[0] >= h) && (hgot[1] >= h))
        {
            ret = power3[n - bi];
        }
        else
        {
            const Shift& shift = shifts_dec[bi];
            for (u_t mask = 1; mask <= 3; ++mask)
            {
                aull2_t sub_hgot(hgot);
                for (u_t iab = 0; iab != 2; ++iab)
                {
                    sub_hgot[iab] += (mask & (1u << iab) ? shift.ab[iab] : 0);
                }
                ret += tail_hcount(bi + 1, sub_hgot);
            }
        }
    }
    return ret;
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
