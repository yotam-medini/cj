// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <iterator>
#include <map>
#include <vector>
#include <utility>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
typedef vector<ull_t> vull_t;

static unsigned dbg_flags;

class Portion
{
 public:
    Portion(ull_t un, u_t ud)
    {
        u_t g = gcd(un, ud);
        num = un / g;
        denom = ud / g;
    }
    ull_t num;
    u_t denom;
 private:
    u_t gcd(ull_t un, ull_t ud) // Euclid
    {
        // ud is actually u_t
        u_t ret = ud;
        u_t residue = un % ud;
        while (residue > 0)
        {
            u_t big = ret;
            ret = residue;
            residue = big % residue;
        }
        return ret;   
    }
};
bool operator<(const Portion& p0, const Portion& p1)
{
    bool lt = p0.num * p1.denom < p1.num * p0.denom;
    return lt;
}
typedef map<Portion, vu_t> portion2vu_t;

class OversizedPancake
{
 public:
    OversizedPancake(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    bool cuts_can();
    bool can_val_add(ull_t v, u_t need);
    bool can_add(u_t need);
    bool can_val_advance(ull_t v, u_t need, u_t ai, u_t pending);
    bool can_advance(u_t need, u_t ai, u_t pending);
    // non-naive methods
    void init();
    u_t cost(const portion2vu_t::value_type& pc) const;
    u_t N, D;
    vull_t a;
    vull_t sa;
    vu_t pans_cuts;
    u_t solution;
    portion2vu_t portion_cuts;
};

OversizedPancake::OversizedPancake(istream& fi) : solution(0)
{
    fi >> N >> D;
    copy_n(istream_iterator<ull_t>(fi), N, back_inserter(a));
}

void OversizedPancake::solve_naive()
{
    if (N == 1)
    {
        solution = D - 1;
    }
    else if (D > 1)
    {
        sa = a;
        sort(sa.begin(), sa.end());
        pans_cuts = vu_t(size_t(N), 0);
        for (solution = 0; !cuts_can(); ++solution)
        {
            if (dbg_flags & 0x1) { cerr <<"tried solution="<<solution<<'\n'; }
        }
    }
}

bool OversizedPancake::cuts_can()
{
    bool can = false;
    // select pre-size
    for (u_t i = 0, j = 0; (i < N) && !can; i = j)
    {
        const ull_t v = sa[i];
        for (j = i + 1; (j < N) && (sa[j] == v); ++j) {}
        u_t nv = j - i;
        can = (D <= nv);
        can = can || can_val_add(v, D - nv);
    }
    can = can || can_add(D);
    return can;
}

bool OversizedPancake::can_val_add(ull_t v, u_t need)
{
    bool can = can_val_advance(v, need, 0, solution);
    return can;
}

bool OversizedPancake::can_val_advance(ull_t v, u_t need, u_t ai, u_t pending)
{
    bool can = false;
    if (pending == 0)
    {
        u_t made = 0;
        for (u_t i = 0; (i < N) && !can; ++i)
        {
            const u_t pc = pans_cuts[i];
            if (pc > 0)
            {
                u_t q = sa[i] / v;
                u_t cuts = (sa[i] % v == 0 ? q - 1 :  q);
                u_t got = (pc >= cuts ? q : min(q, pc));
                made += got;
                can = (made >= need);
            }
        }
    }
    else
    {
        for (; (ai < N) && sa[ai] == v; ++ai)
        {
            pans_cuts[ai] = 0;
        }
        if (ai < N)
        {
            for (u_t cuts = 0; (cuts <= pending) && !can; ++cuts)
            {
                pans_cuts[ai] = cuts;
                can = can_val_advance(v, need, ai + 1, pending - cuts);
            }
            pans_cuts[ai] = 0;
        }
    }
    return can;
}

bool OversizedPancake::can_add(u_t need)
{
    bool can = can_advance(need, 0, solution);
    return can;
}

bool OversizedPancake::can_advance(u_t need, u_t ai, u_t pending)
{
    bool can = false;
    if (pending == 0)
    {
        for (u_t vi = 0; (vi < N) && !can; ++vi)
        {
            const u_t pc_vi = pans_cuts[vi];
            const ull_t sa_vi = sa[vi];
            const u_t ib = (pc_vi > 0 ? 0 : N);
            u_t made = 0;
            // our rational 'v' == sa_vi / (pc_vi + 1)
            for (u_t i = ib; (i < N) && !can; ++i)
            {
                const u_t pc = pans_cuts[i];
                if (pc > 0)
                {
                    ull_t numerator = sa[i] * (pc_vi + 1);
                    u_t q = numerator / sa_vi;
                    u_t cuts = (numerator % sa_vi == 0 ? q - 1 : q);
                    u_t got = (pc >= cuts ? q : min(q, pc));
                    made += got;
                    can = (made >= need);
                }
            }
        }
    }
    else
    {
        if (ai < N)
        {
            for (u_t cuts = 0; (cuts <= pending) && !can; ++cuts)
            {
                pans_cuts[ai] = cuts;
                can = can_advance(need, ai + 1, pending - cuts);
            }
            pans_cuts[ai] = 0;
        }
    }
    return can;
}

void OversizedPancake::solve()
{
    // solve_naive();
    if (N == 1)
    {
        solution = D - 1;
    }
    else if (D > 1)
    {
        init();
        solution = D; // infinity
        for (const portion2vu_t::value_type& pc: portion_cuts)
        {
            u_t cost_pc = cost(pc);
            if (solution > cost_pc)
            {
                solution = cost_pc;
            }
        }
    }
}

void OversizedPancake::init()
{
    sa = a;
    sort(sa.begin(), sa.end());
    for (ull_t num: sa)
    {
        for (u_t cut = 0; cut < D; ++cut)
        {
            Portion portion(num, cut + 1);
            auto er = portion_cuts.equal_range(portion);
            portion2vu_t::iterator iter = er.first;
            if (er.first == er.second)
            {
                portion2vu_t::value_type v{portion, vu_t()};
                iter = portion_cuts.insert(iter, v);
            }
            vu_t& cuts = iter->second;
            size_t sz = cuts.size();
            size_t nz = (sz <= cut ? cut + 1 - sz : 0);
            cuts.insert(cuts.end(), nz, 0);
            ++(cuts[cut]);
        }
    }
}

u_t OversizedPancake::cost(const portion2vu_t::value_type& pc) const
{
    u_t ret = 0;
    const Portion& p = pc.first;
    const vu_t& cuts = pc.second;
    const u_t cuts_sz = cuts.size();
    u_t ci = 0, got = 0;
    for ( ; (ci < cuts_sz) && (got + (ci + 1)*cuts[ci] <= D); ++ci)
    {
        u_t gain = (ci + 1)*cuts[ci];
        u_t price = ci*cuts[ci];
        got += gain;
        ret += price;
    }
    u_t need = D - got;
    if (ci < cuts_sz)
    {
        u_t n_whole = need / (ci + 1);
        u_t residue = need % (ci + 1);
        ret += ci*n_whole + residue;
        need = 0; // no need anymore?
    }
    else
    {
        // Look for sa[z] larger than p but not integere multiplay.
        u_t pceil = (p.num / p.denom);
        vull_t::const_iterator iter = upper_bound(sa.begin(), sa.end(), pceil);
        for (; (iter != sa.end()) && (need > 0); ++iter)
        {
            const ull_t ai = *iter;
            u_t q = (ai * p.denom) / p.num;
            u_t residue = (ai * p.denom) % p.num;
            if ((residue > 0) || (q > D)) // otherwise already considered above
            {
                u_t add = min(need, q);
                need -= add;
                ret += add;
            }
        }
        if (need > 0)
        {
            ret = D; // failure
        }
    }
    return ret;
}

void OversizedPancake::print_solution(ostream &fo) const
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

    void (OversizedPancake::*psolve)() =
        (naive ? &OversizedPancake::solve_naive : &OversizedPancake::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        OversizedPancake overpan(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (overpan.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        overpan.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
