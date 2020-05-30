// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef long long ll_t;
typedef vector<ll_t> vll_t;
typedef vector<vll_t> vvll_t;
typedef map<u_t, u_t> u2u_t;

static unsigned dbg_flags;

class OP
{
 public:
    OP(char _cmd=' ', int _v0=0, int _v1=0) :
        cmd(_cmd), v0(_v0), v1(_v1)
        {}
    char cmd;
    int v0, v1;
};
typedef vector<OP> vop_t;

class Candies
{
 public:
    Candies(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    void init();
    void update(u_t idx, u_t val);
    void sums_till(ll_t& sum, ll_t& asum, u_t till) const;
    u_t n, q;
    vll_t candies;
    vvll_t bin_sums;
    vvll_t bin_asums;
#if 0
    vll_t scandies;
    vll_t sacandies;
    u2u_t updates;
#endif
    ll_t solution;
    vop_t ops;
};

Candies::Candies(istream& fi) : solution(0)
{
    fi >> n >> q;
    copy_n(istream_iterator<ll_t>(fi), n, back_inserter(candies));
    ops.reserve(q);
    for (u_t qi = 0; qi < q; ++qi)
    {
        string scmd;
        int v0, v1;
        fi >> scmd >> v0 >> v1;
        char cmd = scmd[0];
        ops.push_back(OP(cmd, v0, v1));
    }
}

void Candies::solve_naive()
{
    for (const OP& op: ops)
    {
        if (op.cmd == 'Q')
        {
            const u_t l = op.v0;
            const u_t r = op.v1;
            int a = 1;
            int sign = 1;
            ll_t s = 0;
            for (u_t i = l - 1; i <= r - 1; ++i, ++a, sign = -sign)
            {
                ll_t factor = sign*a;
                ll_t add = factor * candies[i];
                s += add;
            }
            solution += s;
        }
        else if (op.cmd == 'U')
        {
            u_t idx = op.v0 - 1;
            int new_val = op.v1;
            candies[idx] = new_val;
        }
        else
        {
            cerr << "RRROR:" << __LINE__ << '\n';
            exit(1);
        }
    }
}

void Candies::solve()
{
    init();
    for (const OP& op: ops)
    {
        if (op.cmd == 'Q')
        {
            const u_t l = op.v0 - 1;
            const u_t r = op.v1 - 1;
            ll_t sum_till_l, sum_till_r;
            ll_t asum_till_l, asum_till_r;
            sums_till(sum_till_l, asum_till_l, l);
            sums_till(sum_till_r, asum_till_r, r + 1);
            ll_t ds = sum_till_r - sum_till_l;
            ll_t das = asum_till_r - asum_till_l;
            ll_t s = das - l*ds;
            if (l % 2 != 0)
            {
                s = -s;
            }
            solution += s;
        }
        else if (op.cmd == 'U')
        {
            update(op.v0 - 1, op.v1);
        }
        else
        {
            cerr << "RRROR:" << __LINE__ << '\n';
            exit(1);
        }
    }
}

void Candies::init()
{
    bin_sums.push_back(vll_t());
    bin_asums.push_back(vll_t());
    vll_t& bin_sum0 = bin_sums.back();
    vll_t& bin_asum0 = bin_asums.back();
    bin_sum0.reserve(n);
    bin_asum0.reserve(n);
    {
        int a = 1;
        int sign = 1;
        for (ll_t candy: candies)
        {
            ll_t scandy = sign*candy;
            ll_t sacandy = a*scandy;
            bin_sum0.push_back(scandy);
            bin_asum0.push_back(sacandy);
            ++a;
            sign = -sign;
        }
    }

    for (u_t bsz_next = 2; bsz_next <= n; bsz_next *=2)
    {
        const vll_t& bin_sum = bin_sums.back();
        const vll_t& bin_asum = bin_asums.back();
        const u_t sz = bin_sum.size(), sz_next = sz/2;
        vll_t bin_sum_next;
        vll_t bin_asum_next;
        bin_sum_next.reserve(sz_next);
        bin_asum_next.reserve(sz_next);
        for (u_t inext = 0, i = 0; inext < sz_next; ++inext, i += 2)
        {
            bin_sum_next.push_back(bin_sum[i] + bin_sum[i + 1]);            
            bin_asum_next.push_back(bin_asum[i] + bin_asum[i + 1]);            
        }
        bin_sums.push_back(bin_sum_next);
        bin_asums.push_back(bin_asum_next);
    }
}

void Candies::sums_till(ll_t& sum, ll_t& asum, u_t till) const
{
    sum = asum = 0;
    for (u_t h = 0; till > 0; ++h, till /= 2)
    {
        if (till % 2 != 0)
        {
            sum += bin_sums[h][till - 1];
            asum += bin_asums[h][till - 1];
        }
    }
}

void Candies::update(u_t idx, u_t val)
{
    const int iidx = idx;
    const int ival = val;
    const ll_t old_val = bin_sums[0][idx];
    const ll_t new_val = (idx % 2 == 0 ? ival : -ival);
    const ll_t old_aval = bin_asums[0][idx];
    const ll_t new_aval = (iidx + 1)*(idx % 2 == 0 ? ival : -ival);
    const ll_t delta = new_val - old_val;
    const ll_t adelta = new_aval - old_aval;
    const u_t height = bin_sums.size();
    for (u_t h = 0; h < height; ++h, idx /= 2)
    {
        if (idx < bin_sums[h].size())
        {
            bin_sums[h][idx] += delta;
            bin_asums[h][idx] += adelta;
        }
    }
}

#if 0
void Candies::solve()
{
    scandies.reserve(n);
    sacandies.reserve(n);
    {
        int a = 1;
        int sign = 1;
        ll_t sum = 0;
        ll_t asum = 0;
        for (ll_t candy: candies)
        {
            ll_t scandy = sign*candy;
            ll_t sacandy = a*scandy;
            sum += scandy;
            asum += sacandy;
            scandies.push_back(sum);
            sacandies.push_back(asum);
            ++a;
            sign = -sign;
        }
    }
    for (const OP& op: ops)
    {
        if (op.cmd == 'Q')
        {
            const u_t l = op.v0 - 1;
            const u_t r = op.v1 - 1;
            ll_t ds = scandies[r] - (l == 0 ? 0 : scandies[l - 1]);
            ll_t das = sacandies[r] - (l == 0 ? 0 : sacandies[l - 1]);
            ll_t s = das - l*ds;
            if (l % 2 != 0)
            {
                s = -s;
            }
            u2u_t::const_iterator b = updates.lower_bound(l), iter = b;
            u2u_t::const_iterator e = updates.lower_bound(r + 1);
            ll_t update_delta = 0;
            for (; iter != e; ++iter)
            {
                u_t idx = iter->first;
                u_t pos = idx - l;
                int sign = (pos % 2 == 0 ? 1 : -1);
                int uv = iter->second;
                int vdelta = uv - candies[idx];
                int delta = sign*(pos + 1)*vdelta;
                update_delta += delta;
            }
            s += update_delta;
            solution += s;
        }
        else if (op.cmd == 'U')
        {
            update(op.v0 - 1, op.v1);
        }
        else
        {
            cerr << "RRROR:" << __LINE__ << '\n';
            exit(1);
        }
    }
}

void Candies::update(u_t idx, u_t val)
{
    auto er = updates.equal_range(idx);
    u2u_t::iterator iter = er.first;
    if (er.first == er.second)
    {
        updates.insert(iter, u2u_t::value_type(idx, val));
    }
    else
    {
        iter->second = val;
    }
}
#endif

void Candies::print_solution(ostream &fo) const
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

    void (Candies::*psolve)() =
        (naive ? &Candies::solve_naive : &Candies::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Candies candies(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (candies.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        candies.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
