// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <set>
#include <map>
#include <vector>
#include <deque>
#include <utility>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
typedef map<u_t, u_t> u2u_t;
typedef set<u_t> setu_t;
typedef vector<u2u_t> vu2u_t;
typedef deque<u_t> dqu_t;
typedef map<u_t, dqu_t> u2dqu_t;
typedef map<u_t, u_t> u2u_t;
// typedef pair<u_t, u_t> uu_t;
// typedef map<uu_t, u_t> uu2u_t;
typedef map<u_t, vu_t> u2vu_t;

enum { AMAX = 10000 };

static unsigned dbg_flags;

class Diverse
{
 public:
    Diverse(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    u_t lr_count(u_t l, u_t r) const;
    u_t reduce();
    u_t alternative(u_t &alt_pos);
    void compute_alt_used(u_t pos);
    // u_t till_count(u_t &l, u_t r);
    u_t n, s;
    u_t solution;
    vu_t trinkets;
    vu2u_t count_till;
    // uu2u_t memo;
    vu_t trinkets_reduced;
    u2vu_t tpos;
    u2dqu_t used;
    u2dqu_t alt_used;
};

Diverse::Diverse(istream& fi) : solution(0)
{
    fi >> n >> s;
    trinkets.reserve(n);
    for (u_t i = 0; i < n; ++i)
    {
        u_t t;
        fi >> t;
        trinkets.push_back(t);
    }
}

void Diverse::solve_naive()
{
    u2u_t count;
    count_till.reserve(n + 1);
    count_till.push_back(count);
    for (u_t i = 0; i < n; ++i)
    {
        u_t t = trinkets[i];
        auto er = count.equal_range(t);
        if (er.first == er.second)
        {
            u2u_t::value_type v(t, 1);
            count.insert(er.first, v);
        }
        else
        {
            ++(*er.first).second;
        }
        count_till.push_back(count); 
    }

    for (u_t l = 0; l < n; ++l)
    {
        for (u_t r = l + 1; r <= n; ++r)
        {
            u_t candid = lr_count(l, r);
            if (solution < candid)
            {
                solution = candid;
            }
        }
    }
}

u_t Diverse::lr_count(u_t l, u_t r) const
{
    u_t ret = 0;
    const u2u_t& rcount = count_till[r];
    const u2u_t& lcount = count_till[l];
    for (u2u_t::const_iterator i = rcount.begin(), e = rcount.end();
        i != e; ++i)
    {
        const u2u_t::value_type &v = *i;
        u_t nr = v.second;
        u2u_t::const_iterator li = lcount.find(v.first);
        u_t nl = (li == lcount.end() ? 0 : (*li).second);
        u_t nn = nr - nl;
        if (nn <= s)
        {
            ret += nn;
        }
    }
    return ret;
}   

void maximize(u_t &v, u_t by)
{
    if (v < by)
    {
        v = by;
    }
}

void Diverse::solve()
{
    u_t rsz = reduce();
    for (u_t l = 0; l < n; ++l)
    {
        vu_t counters(vu_t::size_type(rsz), 0);
        u_t candidate = 0;
        for (u_t r = l; r < n; ++r)
        {
            u_t t = trinkets_reduced[r];
            u_t ct = counters[t];
            if (ct < s)
            {
                ++candidate;
                maximize(solution, candidate);
            }
            else if (ct == s)
            {
                candidate -= s;
            }
            counters[t] = ct + 1;
        }
    }
}

#if 0
void Diverse::solve()
{
    u_t curr = 0;
    u_t pos = 0;
    for (u_t till = 0; till < n; ++till)
    {
        u_t t = trinkets[till];
        auto er = used.equal_range(t);
        u2dqu_t::iterator i = er.first;
        if (er.first == er.second)
        {
            u2dqu_t::value_type v(t, dqu_t());
            i = used.insert(used.end(), v);
        }
        dqu_t &at = (*i).second;
        u_t nt = at.size();
        at.push_back(till);
        if (nt < s)
        {
            ++curr;
            maximize(solution, curr);
        }
        else
        {
            curr = ((nt == s) ? curr - s : curr);
            u_t pos_alt = 0;
            u_t curr_alt = alternative(pos_alt);
            if ((curr < curr_alt) || ((curr == curr_alt) && (pos < pos_alt)))
            {
                curr = curr_alt;
                pos = pos_alt;
                maximize(solution, curr);
                compute_alt_used(pos);
                swap(used, alt_used);
            }
        }
    }
}
#endif

u_t Diverse::reduce()
{
    u2u_t t2i;
    trinkets_reduced.clear();
    trinkets_reduced.reserve(trinkets.size());
    for (u_t t: trinkets)
    {
        auto er = t2i.equal_range(t);
        int i;
        if (er.first == er.second)
        {
            i = t2i.size();
            t2i.insert(er.first, u2u_t::value_type(t, i));
        }
        else
        {
            i = (*er.first).second;
        }
        trinkets_reduced.push_back(i);
    }
    return t2i.size();
}


u_t Diverse::alternative(u_t &alt_pos)
{
    u_t best_value = 0;
    u_t best_pos = 0;
    const u2dqu_t::const_iterator ub = used.begin(), ue = used.end();
    for (u2dqu_t::const_iterator ci = ub; ci != ue; ++ci)
    {
        const dqu_t &take_positions = (*ci).second;
        u_t sz = take_positions.size();
        if (sz > s)
        {
            u_t value = 0;
            u_t pos = take_positions[sz - s - 1] + 1;
            for (u2dqu_t::const_iterator cj = ub; cj != ue; ++cj)
            {
                const dqu_t &apos = (*cj).second;
                dqu_t::const_iterator lb =
                    lower_bound(apos.begin(), apos.end(), pos);
                dqu_t tail(lb, apos.end());
                u_t add = tail.size();
                if (add <= s)
                {
                    value += add;
                }
            }
            if ((best_pos < value) ||
                ((best_value == value) && (best_pos < pos)))
            {
                best_value = value;
                best_pos = pos;
            }
        }
    }
    alt_pos = best_pos;
    return best_value;
}

void Diverse::compute_alt_used(u_t pos)
{
    alt_used.clear();
    for (u2dqu_t::const_iterator ci = used.begin(); ci != used.end(); ++ci)
    {
        u_t a = (*ci).first;
        const dqu_t &apos = (*ci).second;
        dqu_t::const_iterator lb = lower_bound(apos.begin(), apos.end(), pos);
        dqu_t tail(lb, apos.end());
        u_t sz = tail.size();
        if (sz > 0)
        {
            u2dqu_t::value_type v(a, dqu_t(lb, apos.end()));
            alt_used.insert(alt_used.end(), v);
        }
    }
}

#if 0
u_t Diverse::alternative(u_t t)
{
    u_t value = 0;
    u2dqu_t::iterator i = used.find(t);
    if (i == used.end())
    {
        cerr << "fatal: " << __LINE__ << '\n';  exit(1);
    }
    dqu_t &at = (*i).second;
    // We want a tail of at of size s
    u_t atsz = at.size();
    if (atsz <= s)
    {
        cerr << "fatal: " << __LINE__ << '\n';  exit(1);
    }
    u_t pos = at[atsz - s - 1] + 1;

    alt_used.clear();
    for (u2dqu_t::const_iterator ci = used.begin(); ci != used.end(); ++ci)
    {
        u_t a = (*ci).first;
        const dqu_t &apos = (*ci).second;
        dqu_t::const_iterator lb = lower_bound(apos.begin(), apos.end(), pos);
        dqu_t tail(lb, apos.end());
        u_t add = tail.size();
        if (add > 0)
        {
            u2dqu_t::value_type v(a, dqu_t(lb, apos.end()));
            alt_used.insert(alt_used.end(), v);
            if (add <= s)
            {
                value += add;
            }
        }
    }
    return value;
}
#endif

void Diverse::print_solution(ostream &fo) const
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

    void (Diverse::*psolve)() =
        (naive ? &Diverse::solve_naive : &Diverse::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Diverse diverse(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (diverse.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        diverse.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
