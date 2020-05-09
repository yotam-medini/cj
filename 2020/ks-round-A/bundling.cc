// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <numeric>
#include <set>
#include <string>
#include <utility>
#include <array>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<string> vs_t;
typedef vector<u_t> vu_t;
typedef vector<bool> vb_t;
typedef vector<vu_t> vvu_t;
typedef set<u_t> setu_t;
typedef array<u_t, 2> au2_t;
typedef map<au2_t, u_t> au2_to_u_t;

static unsigned dbg_flags;

void combination_first(vu_t &c, u_t n, u_t k)
{
    c = vu_t(k);
    iota(c.begin(), c.end(), 0);
}

bool combination_next(vu_t &c, u_t n)
{
    u_t j = 0, k = c.size();

    // sentinels (Knuth) (Why the second (0) ??!)
    c.push_back(n);  c.push_back(0);

    while ((j < k) && (c[j] + 1 == c[j + 1]))
    {
        c[j] = j;
        ++j;
    }
    bool ret = j < k;
    if (ret)
    {
        ++(c[j]);
    }

    c.pop_back(); c.pop_back(); // the sentinels

    return ret;
}

struct Available
{
 public:
    vu_t a;
    setu_t s;
};

class Bundling
{
 public:
    Bundling(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    void gen_groups();
    void next_group(u_t gi);
    ull_t check_groups() const;
    ull_t group_compute(const vu_t& group) const;
    void init_cand();
    ull_t ii_common(u_t i0, u_t i1) const;
    u_t n, k;
    vs_t ss;
    ull_t solution;
    // naive
    vvu_t groups;
    Available available;
    // non-naive
    vb_t used;
    u2_au2_t cand; // (val, low) -> high
};

Bundling::Bundling(istream& fi) : solution(0)
{
    fi >> n >> k;
    ss.reserve(n);
    for (u_t si = 0; si < n; ++si)
    {
        string s;
        fi >> s;
        ss.push_back(s);
    }
}

void Bundling::solve_naive()
{
    sort(ss.begin(), ss.end());
    gen_groups();
}

void Bundling::gen_groups()
{
    const u_t ng = n / k;
    const vu_t group_undef(size_t(k), n);
    for (u_t i = 0; i != n; ++i)
    {
        available.a.push_back(i);
        available.s.insert(available.s.end(), i);
    }
    groups = vvu_t(size_t(ng), group_undef);
    next_group(0);
}

void Bundling::next_group(u_t gi)
{
    if (k*gi == n)
    {
        ull_t v = check_groups();
        if (solution < v)
        {
            solution = v;
        }
    }
    else
    {
        vu_t& aa = available.a; // abbreviation
        vu_t& group = groups[gi];

        u_t i0 = *available.s.begin();
        group[0] = i0;
        available.s.erase(available.s.begin());
        u_t aai0 = find(aa.begin(), aa.end(), i0) - aa.begin();
        aa[aai0] = aa.back();
        aa.pop_back();

        vu_t comb;
        bool combing = true;        
        for (combination_first(comb, aa.size(), k - 1); combing; )
        {
            for (u_t ki = k - 1; ki > 0; --ki)
            {
                u_t aai = comb[ki - 1];
                u_t i = aa[aai];
                group[ki] = i;
                aa[aai] = aa.back();
                aa.pop_back();
                available.s.erase(i);
            }
            next_group(gi + 1);
            // restore
            for (u_t ki = 1; ki < k; ++ki)
            {
                u_t aai = comb[ki - 1];
                aa.push_back(aa[aai]);
                aa[aai] = group[ki];
                available.s.insert(available.s.end(), group[ki]);
                group[ki] = n; // undef
            }
            combing = combination_next(comb, aa.size());
        }
        aa.push_back(aa[aai0]);
        aa[aai0] = group[0];
        available.s.insert(available.s.end(), group[0]);
        group[0] = n; // undef
    }
}

ull_t Bundling::check_groups() const
{
    ull_t v = 0;
    for (const vu_t& group: groups)
    {
        ull_t vg = group_compute(group);
        v += vg;
    }
    return v;
}

ull_t Bundling::group_compute(const vu_t& group) const
{
    bool common = true;
    u_t si;
    for (si = 0; common; ++si)
    {
        const string& s0 = ss[group[0]];
        common = si < s0.size();
        char c = (common ? s0[si] : '?');
        for (u_t ki = 1; common && (ki < k); ++ki)
        {
            const string& s = ss[group[ki]];
            common = (si < s.size()) && (s[si] == c);
        }
    }
    --si;
    return si;
}

void Bundling::solve()
{
    sort(ss.begin(), ss.end());
    if (n == k)
    {
        solution = ii_common(0, n - 1);
    }
    else if (k == 1)
    {
        for (const string& s: ss)
        {
            solution += s.size();
        }
    }
    else
    {
        init_cand();
        while (!cand.empty())
        {
            au2_to_u_t::reverse_iterator ibest = cand.rbegin();
            au2_to_u_t::iterator ifwd_best(ibest + 1);
            const au2_to_u_t::value_type& vt = *ifwd_best;
            u_t v = vt.first[0];
            u_t low = vt.first[1];
            u_t high = vt.second;
            solution += v;
            delete_candidate(low, high);
            add_candidate(low, high);
        }
    }
}

ull_t Bundling::init_cand()
{
    used.insert(size_t(n), false);
    for (u_t low = 0, high = k - 1; high < n; ++low, ++high)
    {
        u_t v = ii_common(low, high);
        au2_to_u_t::key_type key(v, low);
        au2_to_u_t::value_type vt(key, high);
        cand.insert(cand, vt);
    }
}

ull_t Bundling::ii_common(u_t i0, u_t i1) const
{
    const string& s0 = ss[i0];
    const string& s1 = ss[i1];
    ull_t minlen = min(s0.size(), s1.size());
    ull_t ret = 0;
    for ( ; (ret < minlen) && (s0[ret] == s1[ret]); ++ret) {}
    return ret;
}

void Bundling::print_solution(ostream &fo) const
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

    void (Bundling::*psolve)() =
        (naive ? &Bundling::solve_naive : &Bundling::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Bundling bundling(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (bundling.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        bundling.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
