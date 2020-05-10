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

template<typename T> class SkipListListNode; // fwd decl
template <typename T>
class SkipListListLink
{
 public:
    typedef SkipListListNode<T>* nodep_t;
    SkipListListLink(nodep_t nxt=0, u_t l=0) : next(nxt), length(l) {}
    nodep_t next;
    u_t length;
};

template<typename T>
class SkipListListLinks
{
 public:
    enum {LEVEL_MAX = 32};
    SkipListListLink<T> link[LEVEL_MAX];
    typedef SkipListListNode<T> node_t;
    // const node_t* next(u_t level) const { return link[level].next; }
    // node_t* next(u_t level) { return link[level].next; }
};

template<typename T>
class SkipListListNode
{
 public:
    typedef T data_t;
    SkipListListNode<T>(const data_t& d) : data(d) {}
    data_t data;
    SkipListListLinks<T> links;
};

template<typename T>
class SkipListList
{
 public:
    typedef T data_t;
    SkipListList<T>() : sz(0), levels(0) {}
    u_t size() const { return sz; }
    void insert(u_t i, const data_t& d);
    void remove(u_t i);
    const data_t* get(u_t i) const;
    void debug_print(std::ostream&) const;
 private:
    typedef SkipListListLink<T> link_t;
    typedef SkipListListLinks<T> links_t;
    typedef SkipListListNode<T> node_t;
    typedef node_t* nodep_t;
    enum {LEVEL_MAX = links_t::LEVEL_MAX};
    links_t head;
    u_t sz;
    u_t levels;
};

template <typename T>
void SkipListList<T>::insert(u_t i, const data_t& d)
{
    static u_t calls = 0;
    ++sz;
    u_t new_level = 0, seed = ++calls;
    while (seed && new_level + 1 < LEVEL_MAX)
    {
        new_level += (seed % 2 == 0 ? 1 : 0);
        seed /= 2;
    }
    if (levels < new_level + 1)
    {
        levels = new_level + 1;
    }
    nodep_t node = new node_t(d);
    int j = 0;
    links_t* p = &head;
    for (u_t level = levels; level > 0; )
    {
        --level;
        for (; p->link[level].next && (j + p->link[level].length < i + 1);
            p = &p->link[level].next->links)
        {
            j += p->link[level].length;
        }
        link_t& pll = p->link[level];
        ++pll.length;
        if (level <= new_level)
        {
            node->links.link[level].next = pll.next;
            pll.next = node;
            u_t delta = i - j + 1;
            node->links.link[level].length = pll.length - delta;
            pll.length = delta;
        }
    }
}

template <typename T>
void SkipListList<T>::remove(u_t i)
{
    u_t j = 0;
    links_t* p = &head;
    nodep_t x = 0;
    for (u_t level = levels; level > 0; )
    {
        --level;
        for (; p->link[level].next && (j + p->link[level].length < i + 1);
            p = &p->link[level].next->links)
        {
            j += p->link[level].length;
        }
        link_t& pll = p->link[level];
        --(pll.length);
        if (pll.next && (j + pll.length == i))
        {
            x = pll.next; // needed only for level=0
            pll.length += pll.next->links.link[level].length;
            pll.next = pll.next->links.link[level].next;
        }
    }
    delete x;
    --sz;
    for (; (levels > 0) && (head.link[levels - 1].next == 0); --levels) {}
}

template <typename T>
const typename SkipListList<T>::data_t* SkipListList<T>::get(u_t i) const
{
    u_t j = 0;
    const links_t* p = &head;
    for (u_t level = levels; level > 0; )
    {
        --level;
        for (; p->link[level].next && (j + p->link[level].length < i + 1);
            p = &p->link[level].next->links)
        {
            j += p->link[level].length;
        }
    }
    nodep_t x = p->link[0].next;
    data_t* pd = &x->data;
    return pd;
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
    void delete_add_candidates(u_t low, u_t high);
    u_t skip_find(u_t si) const; // assuming success
    ull_t ii_common(u_t i0, u_t i1) const;
    u_t n, k;
    vs_t ss;
    ull_t solution;
    // naive
    vvu_t groups;
    Available available;
    // non-naive
    typedef au2_to_u_t cand_t;
    cand_t cand; // (val, low) -> high

    typedef SkipListList<u_t> skiplistu_t;
    skiplistu_t ss_idx;    
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
            cand_t::reverse_iterator ibest = cand.rbegin();
            cand_t::iterator ifwd_best((++ibest).base());
            const cand_t::value_type& vt = *ifwd_best;
            u_t v = vt.first[0];
            u_t low = vt.first[1];
            u_t high = vt.second;
            solution += v;
            delete_add_candidates(low, high);
        }
    }
}

void Bundling::init_cand()
{
    for (u_t low = 0, high = k - 1; high < n; ++low, ++high)
    {
        u_t v = ii_common(low, high);
        cand_t::key_type key{v, low};
        cand_t::value_type vt(key, high);
        cand.insert(cand.end(), vt);
    }
    for (u_t i = 0; i < n; ++i)
    {
        ss_idx.insert(i, i);
    }
}

void Bundling::delete_add_candidates(u_t low, u_t high)
{
    const u_t km1 = k - 1;
    u_t iskip_low = skip_find(low);
    u_t iskip_high = skip_find(high);
    if (iskip_high - iskip_low != km1)
    {
        cerr << "ERROR: low="<<low << ", high="<<high <<
            ", iskip_low="<<iskip_low << ", iskip_high="<<iskip_high << '\n';
    }
    u_t sz_old = ss_idx.size();
    u_t iskipb = (iskip_low < km1 ? 0 : iskip_low - km1);
    u_t iskipe = min(sz_old - (k - 1), iskip_high + 1);
    for (u_t iskip = iskipb; iskip < iskipe; ++iskip)
    {
        u_t ilow = *ss_idx.get(iskip);
        u_t ihigh = *ss_idx.get(iskip + km1);
        u_t v = ii_common(ilow, ihigh);
        cand_t::key_type key{v, ilow};
        cand.erase(key);
    }
    for (u_t lskip = iskipb, hskip = lskip + k + km1; 
        (lskip < iskip_low) && (hskip < sz_old);
        ++lskip, ++hskip)
    {
        u_t ilow = *ss_idx.get(lskip);
        u_t ihigh = *ss_idx.get(hskip);
        u_t v = ii_common(ilow, ihigh);
        cand_t::key_type key{v, ilow};
        cand_t::value_type vt(key, ihigh);
        cand.insert(cand.end(), vt);
    }
    for (u_t dc = 0; dc < k; ++dc) // remove [iskip_low .. iskip_high]
    {
        ss_idx.remove(iskip_low);
    }
}

u_t Bundling::skip_find(u_t si) const
{
    u_t ret = n; // undef
    u_t low = 0, high = ss_idx.size() - 1;
    if (*ss_idx.get(0) == si)
    {
        ret = 0;
    }
    else if (*ss_idx.get(high) == si)
    {
        ret = high;
    }
    while (ret == n)
    {
        u_t mid = (low + high)/2;
        u_t g = *ss_idx.get(mid);
        if (g == si)
        {
            ret = mid;
        }
        else if (g < si)
        {
            low = mid + 1;
        }
        else // si < g
        {
            high = mid - 1;
        }
    }
    return ret;
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
