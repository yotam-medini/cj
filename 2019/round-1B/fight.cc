// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <array>
#include <map>
#include <algorithm>
#include <utility>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<int> vi_t;
typedef vector<vi_t> vvi_t;
typedef vector<ull_t> vull_t;
typedef array<int, 2> i2_t;
// typedef map<int, u_t> i2u_t;

static unsigned dbg_flags;

class NextBound
{
 public:
    NextBound(const vi_t &values=vi_t());
    int prev_bound(int before) const 
    {
        return prev_bound_value(before, get(before));
    }
    int next_bound(int after) const
    {
        return next_bound_value(after, get(after));
    }
    int prev_bound_value(int before, int v) const;
    int next_bound_value(int after, int v) const;
    int get(int i) const { return binmax[0][i]; }
    int prev_bound_naive(int before) const
    {
        return prev_bound_value_naive(before, get(before));
    }
    int next_bound_naive(int after) const
    {
        return next_bound_value_naive(after, get(after));
    }
    int prev_bound_value_naive(int before, int v) const;
    int next_bound_value_naive(int after, int v) const;
 private:
    void fill_bin(vvi_t &bmax);
    int bin_prev_bound_value(const vvi_t &bmax, int before, int v) const;
    vvi_t binmax;
    vvi_t rbinmax;
};

NextBound::NextBound(const vi_t &values)
{
    u_t sz = values.size();
    int log2 = 0;
    while ((1ul << log2) < sz)
    {
        ++log2;
    }
    binmax.reserve(log2);
    binmax.push_back(values);
    fill_bin(binmax);
    rbinmax.reserve(log2);
    vi_t rvalues(values.rbegin(), values.rend());
    rbinmax.push_back(rvalues);
    fill_bin(rbinmax);
}

void NextBound::fill_bin(vvi_t &bmax)
{
    while (bmax.back().size() > 1)
    {
        bmax.push_back(vi_t());
        vi_t &next = bmax.back();
        const vi_t &prev = bmax[bmax.size() - 2];
        u_t prev_sz = prev.size();
        next.reserve(prev_sz / 2);
        for (u_t i = 0, j = 0; j + 1 < prev_sz; ++i, j += 2)
        {
            next.push_back(max(prev[j], prev[j + 1]));
        }
    }    
}

int NextBound::prev_bound_value(int before, int v) const
{
    return bin_prev_bound_value(binmax, before, v);
}

int NextBound::next_bound_value(int after, int v) const
{
    u_t sz = rbinmax[0].size();
    int bi = bin_prev_bound_value(rbinmax, sz - after - 1, v);
    int ret = sz - bi - 1;
    return ret;
}

int NextBound::bin_prev_bound_value(const vvi_t &bmax, int before, int v) const
{
    int ret = -1;
    u_t p2b = u_t(-1), bib = u_t(-1);
    for (int p2 = int(bmax.size()) - 1, bi = 0; p2 >= 0; --p2)
    {
        if ((bi + 1)*(1u << p2) <= unsigned(before))
        {
            if (v <= bmax[p2][bi])
            {
                p2b = p2;
                bib = bi;
            }
            bi = 2*bi + 2;
        }
        else
        {
            bi = 2*bi;
        }
    }
    if (p2b != u_t(-1))
    {
        while (p2b > 0)
        {
            --p2b;
            bib = 2*bib + 1;
            if (bmax[p2b][bib] < v)
            {
                --bib;
            }
            
        }
        ret = bib;
    }
    return ret;
}

int NextBound::prev_bound_value_naive(int before, int v) const
{
    int ret = -1;
    for (int i = before - 1; i >= 0; --i)
    {
        if (v <= get(i))
        {
            ret = i;
            i = 0;
        }
    }
    return ret;
}

int NextBound::next_bound_value_naive(int after, int v) const
{
    int ret = -1;
    int sz = binmax[0].size();
    for (int i = after + 1; i < sz; ++i)
    {
        if (v <= get(i))
        {
            ret = i;
            i = sz;
        }
    }
    return ret;
}

class Fight
{
 public:
    Fight(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    void solve_naive_better();
    bool is_fair(const i2_t &v) const;
    ull_t nfair_at(u_t i, const i2_t &cdi) const;
    ul_t n;
    int k;
    vi_t cd[2];
    NextBound nbcd[2];
    vull_t nfair_till;
    ull_t solution;
};

Fight::Fight(istream& fi) : solution(0)
{
    fi >> n >> k;
    for (u_t cdi = 0; cdi < 2; ++cdi)
    {
        cd[cdi].reserve(n);
        for (ul_t i = 0; i < n; ++i)
        {
            int skill;
            fi >> skill;
            cd[cdi].push_back(skill);
        }
    }
}

void Fight::solve_naive()
{
    ull_t nfair = 0;
    vi_t::const_iterator cbegin = cd[0].begin();
    vi_t::const_iterator dbegin = cd[1].begin();
    for (u_t l = 0; l < n; ++l)
    {
        for (u_t r = l + 1; r <= n; ++r)
        {
            int cmax = *max_element(cbegin + l, cbegin + r);
            int dmax = *max_element(dbegin + l, dbegin + r);
            int delta = (cmax < dmax ? dmax - cmax : cmax - dmax);
            if (delta <= k)
            {
                ++nfair;
            }
        }
    }
    solution = nfair;
}

bool Fight::is_fair(const i2_t &v) const
{
    int delta = (v[0] < v[1] ? v[1] - v[0] : v[0] - v[1]);
    return (delta <= k);
}

static void maximize(int &m, int x)
{
    if (m < x)
    {
        m = x;
    }
}

void Fight::solve_naive_better()
{
    ull_t nfair = 0;
    for (u_t b = 0; b < n; ++b)
    {
        i2_t cdmax({cd[0][b], cd[1][b]});
        for (u_t e = b; e < n; ++e)
        {
            maximize(cdmax[0], cd[0][e]);
            maximize(cdmax[1], cd[1][e]);
            // if (is_fair(cdmax))
            if (abs(cdmax[0] - cdmax[1]) <= k)
            {
                ++nfair;            
            }
        }
    }
    solution = nfair;
}

void Fight::solve()
{
    NextBound nb0(cd[0]);
    NextBound nb1(cd[1]);
    for (u_t i = 0; i < n; ++i)
    {
        ull_t nfair = 0;
        int v0 = cd[0][i];
        int ib = nb0.prev_bound_value(i, v0 + 1) + 1;
        int ie = nb0.next_bound_value(i, v0);
        int delta = v0 - cd[1][i];
        int abs_delta = (delta < 0 ? -delta : delta);
        if (abs_delta <= k)
        {
            int jb = nb1.prev_bound_value(i, v0 + k + 1) + 1;
            int je = nb1.next_bound_value(i, v0 + k + 1);
            int kb = max(ib, jb);
            int ke = min(ie, je);
            nfair = ull_t(i - kb + 1) * ull_t(ke - i);
        }
        else if (delta > k)
        {
            int jle = nb1.prev_bound_value(i, v0 - k), jlb = jle;
            if (jle >= 0)
            {
                if (cd[1][jle] <= v0 + k)
                {
                    jle += 1;
                    jlb = nb1.prev_bound_value(jle, v0 + k + 1) + 1;
                }
                else
                {
                    jlb = ++jle;
                }                
            }
            int jrb = nb1.next_bound_value(i, v0 - k), jre = jrb;
            if (jrb < int(n))
            {
                if (cd[1][jrb] <= v0 + k)
                {
                    jre = nb1.next_bound_value(jrb, v0 + k + 1);
                }
            }
            int klb = max(ib, jlb);
            int kle = max(ib, jle);
            int krb = min(ie, jrb);
            int kre = min(ie, jre);
            ull_t szl = kle - klb;
            ull_t szr = kre - krb;
            ull_t nfl = szl * ull_t(kre - i);
            ull_t nfr = szr * ull_t(i - klb + 1);
            ull_t nflr = szl * szr;
            nfair = nfl + nfr - nflr;
        }
        solution += nfair;
    }
}

void Fight::print_solution(ostream &fo) const
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

    void (Fight::*psolve)() =
        (naive ? &Fight::solve_naive : &Fight::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Fight fight(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (fight.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        fight.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
