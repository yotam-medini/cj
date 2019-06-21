// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <iostream>
#include <fstream>
#include <string>
#include <array>
#include <set>
#include <algorithm>
#include <list>
#include <map>
#include <vector>
#include <utility>
#include <numeric>

#include <cstdlib>

#include "ymutil/interval_tree.h"

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef pair<u_t, u_t> uu_t;
typedef set<u_t> setu_t;
typedef vector<u_t> vu_t;
typedef vector<uu_t> vuu_t;
typedef set<uu_t> setuu_t;
typedef vector<bool> vb_t;
typedef map<u_t, u_t> u2u_t;
typedef map<u_t, setu_t> u2setu_t;
typedef array<u_t, 2> u2_t;
typedef set<u2_t> setu2_t;
typedef map<u2_t, u_t> u22u_t;
typedef IntervalTree<u_t, u_t> itree_t;
typedef itree_t::interval_t interval_t;
typedef itree_t::vinterval_t vinterval_t;

static unsigned dbg_flags;

void permutation_first(vu_t &p, u_t n)
{
    p = vu_t(n);
    iota(p.begin(), p.end(), 0);
}

bool permutation_next(vu_t &p)
{
    u_t n = p.size();

    // find the last 'in order'
    int j = n - 1 - 1;
    while ((j >= 0) && (p[j] > p[j + 1]))
    {
        --j;
    }
    bool ret = j >= 0;
    if (ret)
    {
        // Find last > p[j]. Must find since p[j] < p[j+1]
        u_t l;
        for (l = n - 1; p[j] >= p[l]; --l);
        swap(p[j], p[l]);
        reverse(p.begin() + j + 1, p.end());
    }
    return ret;
}

class IndexValue
{
 public:
    class VIter
    {
     public:
        VIter(u_t _v, list<u_t>::iterator _iter) : v(_v), iter(_iter) {}
        u_t v;
        list<u_t>::iterator iter;
    };
    typedef map<u_t, VIter> u2viter_t;
    typedef map<u_t, list<u_t>, greater<u_t>> u2listu_t;
#if 0
    void add(u_t i, u_t v)
    {
        auto er = v2i.equal_range(v);
        u2listu_t::iterator iter = er.first;
        if (er.first == er.second)
        {
            iter = v2i.insert(iter, u2listu_t::value_type(v, list<u_t>()));
        }
        list<u_t> &l = (*iter).second;
        l.push_front(i);
        VIter viter(v, l.begin());
        u2viter_t::value_type vv(i, viter);
        i2v.insert(i2v.end(), vv);
    }
#endif
    void del(u_t i)
    {
        u2viter_t::iterator iter = i2v.find(i);
        VIter &vi = (*iter).second;
        u2listu_t::iterator v2i_iter =  v2i.find(vi.v);
        list<u_t> &l = (*v2i_iter).second;
        l.erase(vi.iter);
        if (l.empty())
        {
            v2i.erase(v2i_iter);
        }
        i2v.erase(iter);
    }
    void add_update(u_t i, u_t v)
    {
        auto v2i_er = v2i.equal_range(v);
        u2listu_t::iterator v2i_iter = v2i_er.first;
        if (v2i_er.first == v2i_er.second)
        {
            u2listu_t::value_type vv(v, list<u_t>());
            v2i_iter = v2i.insert(v2i_iter, vv);
        }
        list<u_t> &l = (*v2i_iter).second;

        auto i2v_er = i2v.equal_range(i);
        u2viter_t::iterator i2v_iter = i2v_er.first;
        if (i2v_er.first == i2v_er.second)
        { // add
            l.push_front(i);
            VIter viter(v, l.begin());
            u2viter_t::value_type vv(i, viter);
            i2v.insert(i2v.end(), vv);
        }
        else 
        { // update
            VIter &viter = (*i2v_iter).second;
            u2listu_t::iterator old_iter =  v2i.find(viter.v);
            list<u_t> &old_l = (*old_iter).second;
            old_l.erase(viter.iter);
            if (old_l.empty())
            {
                v2i.erase(old_iter);
            }
            l.push_front(i);
            viter = VIter(v, l.begin());
        }
    }
    u_t value(u_t i) const
    {
       auto er = i2v.equal_range(i);
       u_t v = (er.first == er.second) ? 0 : (*er.first).second.v;
       return v;
    }
    void increase(u_t i, u_t by=1)
    {
        add_update(i, value(i) + by);
    }
    u2viter_t i2v;
    u2listu_t v2i;
};

class Contention
{
 public:
    Contention(istream& fi);
    void solve_naive();
    // void solve_better();
    void solve();
    void print_solution(ostream&) const;
 private:
    // void dsort();
    // void compute_segs();
    // bool can(u_t k) const;
    bool distinct() const
    { 
        setuu_t set_bookings(bookings.begin(), bookings.end());
        bool eq_size = set_bookings.size() == q;
        return eq_size;
    }
    u_t n, q;
    vuu_t bookings;
    u_t solution;
#if 0
    vu_t dorder;
    vu_t ep; // endpoints : start points + last end
    vu_t seg_count; //
    u2setu_t use2segi;
#endif
    u22u_t seg_count;
    u22u_t seg_owner;
    itree_t itree;
    // vu_t allocations;
    IndexValue allocation;
};

Contention::Contention(istream& fi) : solution(0)
{
    fi >> n >> q;
    bookings.reserve(q);
    for (u_t i = 0; i < q; ++i)
    {
        u_t l, r;
        fi >> l >> r;
        bookings.push_back(uu_t(l - 1, r)); // b,e
    }
}

void Contention::solve_naive()
{
    vu_t order;
    permutation_first(order, q);
    do
    {
        u_t can = n;
        vb_t booked(vb_t::size_type(n), false);
        for (u_t i = 0; i < q; ++i)
        {
            uu_t lr = bookings[order[i]];
            u_t l = lr.first, r = lr.second;
            u_t avail = 0;
            for (u_t k = l; k < r; ++k)
            {
                if (!booked[k])
                {
                    ++avail;
                    booked[k] = true;
                }
            }
            if (can > avail)
            {
                can = avail;
            }
        }
        if (solution < can)
        {
            solution = can;
        }
    } while (permutation_next(order));
}

void Contention::solve()
{
    if (distinct())
    {
        setu_t pts;
        for (const uu_t &b: bookings)
        {
            pts.insert(pts.end(), b.first);
            pts.insert(pts.end(), b.second);
        }
        setu_t::const_iterator pts_ci = pts.begin();
        u_t pt_last = *pts_ci;
        for (++pts_ci; pts_ci != pts.end(); ++pts_ci)
        {
            u_t pt = *pts_ci;
            u22u_t::value_type v(u2_t({pt_last, pt}), 0);
            seg_count.insert(seg_count.end(), v);
            pt_last = pt;
        }
        vector<itree_t::iterator> bookings_iterators;
        bookings_iterators.reserve(q);
        for (u_t bi = 0; bi < q; ++bi)
        {
            const uu_t &b = bookings[bi];
            const u2_t key({b.first, b.first});
            auto er = seg_count.equal_range(key);
            for (u22u_t::iterator pts_i = er.first;
                (pts_i != seg_count.end()) && ((*pts_i).first[0] < b.second);
                ++pts_i)
            {
                ++((*pts_i).second);
            }
            itree_t::iterator i = itree.insert(interval_t(b.first, b.second));
            (*i).d = bi;
            bookings_iterators.push_back(i);
        }
        // allocations = vu_t(vu_t::size_type(q), 0);
        for (auto const &x: seg_count)
        {
            if (x.second == 1)
            {
                vinterval_t vi;
                const u2_t &seg = x.first;
                interval_t interval_seg(seg[0], seg[1]);
                itree.search(vi, interval_seg);
                if (vi.size() != 1)
                {
                    cerr << __LINE__ << ": Erroor\n";
                    exit(1);
                }
                const interval_t &interval = vi[0];
                u_t bi = interval.d;
                u_t seats = seg[1] - seg[0];
                allocation.increase(bi, seats);
            }
        }

        solution = (*allocation.v2i.begin()).first;
        u_t processed = 0;
        while ((processed < q) && (solution > 0) && !allocation.i2v.empty())
        {
            auto const &viter = *(allocation.v2i.begin());
            u_t single_usage = viter.first;
            if (solution > single_usage)
            {
                solution = single_usage;
            }
            const list<u_t> &l = viter.second;
            u_t bi = l.front();
            allocation.del(bi);
            ++processed;

            const uu_t &booking = bookings[bi];
            itree.remove(bookings_iterators[bi]);
            u2_t seg_booking({booking.first, booking.first});
            auto er = seg_count.equal_range(seg_booking);
            for (u22u_t::iterator segi = er.first; 
                (segi != seg_count.end()) && 
                    ((*segi).first[0] < booking.second);
                ++segi)
            {
                if ((--(*segi).second) == 1)
                {
                    const u2_t &seg = (*segi).first;
                    interval_t iseg(seg[0], seg[1]);
                    vinterval_t vi;
                    itree.search(vi, iseg);
                    if (vi.size() != 1)
                    {
                        cerr << __LINE__ << ":Error\n";
                        exit(1);
                    }
                    u_t ubi = vi[0].d;
                    u_t seats = seg[1] - seg[0];
                    allocation.increase(ubi, seats);
                }
            } 
        }
        if (processed < q)
        {
            solution = 0;
        }
    }
}

#if 0
void Contention::solve_better()
{
   dsort();
   compute_segs();
   const uu_t &bsmall = bookings[dorder.back()];
   u_t l = 0, h = bsmall.second + 1 - bsmall.first; // l=possible h=impossible
   while (l + 1 < h)
   {
       if (dbg_flags & 0x1) { cerr << "l="<<l << ", h="<<h << "\n"; }
       u_t mid = (l + h)/2;
       if (can(mid))
       {
           l = mid;
       }
       else
       {
           h = mid;
       }
   }
   solution = l;
}

void Contention::dsort()
{
   dorder = vu_t(vu_t::size_type(q), 0);
   iota(dorder.begin(), dorder.end(), 0);
   sort(dorder.begin(), dorder.end(),
      [this](const u_t &i0, const u_t &i1)
      {
          const uu_t &b0 = bookings[i0];
          const uu_t &b1 = bookings[i1];
          u_t d0 = b0.second - b0.first;
          u_t d1 = b1.second - b1.first;
          bool lt = (d0 > d1) || ((d0 == d1) && (i0 < i1));
          return lt;
      });
}

void Contention::compute_segs()
{
    setu_t pts;
    for (const uu_t &b: bookings)
    {
        pts.insert(pts.end(), b.first);
        pts.insert(pts.end(), b.second);
    }
    ep.insert(ep.end(), pts.begin(), pts.end());
    sort(ep.begin(), ep.end());
    seg_count = vu_t(vu_t::size_type(ep.size() - 1), 0);
    for (const uu_t b: bookings)
    {
        u_t ib = lower_bound(ep.begin(), ep.end(), b.first) - ep.begin();
        u_t ie = lower_bound(ep.begin(), ep.end(), b.second) - ep.begin();
        for (u_t i = ib; i < ie; ++i)
        {
            ++seg_count[i];
        }
    }
    for (const uu_t b: bookings)
    {
        u_t ib = lower_bound(ep.begin(), ep.end(), b.first) - ep.begin();
        u_t ie = lower_bound(ep.begin(), ep.end(), b.second) - ep.begin();
        for (u_t i = ib; i < ie; ++i)
        {
            u_t use_count = seg_count[i];
            auto er = use2segi.equal_range(use_count);
            u2setu_t::iterator where = er.first;
            if (er.first == er.second)
            {
                u2setu_t::value_type v(use_count, setu_t());
                where = use2segi.insert(er.first, v);
            }
            (*where).second.insert(setu_t::value_type(i));
        }
    }
}

bool Contention::can(u_t k) const
{
    vu_t seg_use(seg_count);
    setu_t dis;
    for (u_t i = 0; i < q; ++i)
    {
        dis.insert(dis.end(), i);
    }
    u2setu_t u2si(use2segi); // copy
    bool possible = true;

    while (possible && !dis.empty())
    {
        bool progress = false;
        for (setu_t::iterator i = dis.begin(), i1 = i, e = dis.end();
            i != e; i = i1)
        {
            ++i1;
            u_t di = *i;
            u_t bi = dorder[di];
            const uu_t &b = bookings[bi];
            u_t ib = lower_bound(ep.begin(), ep.end(), b.first) - ep.begin();
            u_t ie = lower_bound(ep.begin(), ep.end(), b.second) - ep.begin();
            u_t n_seats = 0;
            const u2setu_t::iterator set1i = u2si.find(1);
            if (set1i != u2si.end())
            {
                const setu_t &set1 = (*set1i).second;
                for (setu_t::const_iterator
                    sii = set1.lower_bound(ib),
                    sie = set1.lower_bound(ie);
                    (sii != sie) && (n_seats < k); ++sii)
                {
                    u_t si = *sii;
                    u_t seg_size = ep[si + 1] - ep[si];
                    n_seats += seg_size;
                }
            }
            if (n_seats >= k)
            {
                progress = true;
                for (u_t si = ib; si < ie; ++si)
                {
                    // --seg_use[si];
                    u_t use = seg_use[si]--;
                    u2setu_t::iterator usei = u2si.find(use);
                    if (usei == u2si.end())
                    {
                        cerr << __LINE__ << " software error\n";
                        exit(1);
                    }
                    setu_t &old = (*usei).second;
                    old.erase(setu_t::value_type(si));
                    if (old.empty())
                    {
                        u2si.erase(usei);
                    }
                    if (use > 1)
                    {
                        --use;
                        auto er = u2si.equal_range(use);
                        u2setu_t::iterator ni = er.first;
                        if (er.first == er.second)
                        {
                            u2setu_t::value_type v(use, setu_t());
                            ni = u2si.insert(er.first, v);
                        }
                        setu_t &nset = (*ni).second;
                        nset.insert(nset.end(), si);
                    }
                }
                dis.erase(i);
            }
        }
        possible = progress;
    }
    return possible;
}

void Contention::solve()
{
    compute_segs();
    vuu_t::iterator bmaxi = max_element(bookings.begin(), bookings.end(),
        [](const uu_t &b0, const uu_t &b1)
        { return (b0.second - b0.first) < (b1.second - b1.first); });
    const uu_t bmax = *bmaxi;
    solution = bmax.second - bmax.first;
    setu_t dis;
    for (u_t i = 0; i < q; ++i)
    {
        dis.insert(dis.end(), i);
    }
    while (!use2segi.empty())
    {
        const u2setu_t::const_iterator s1i = use2segi.find(1);
        if (s1i == use2segi.end())
        {
            solution = 0;
            use2segi.clear();
        }
        else
        {
            // greedy
            const setu_t &set1 = (*s1i).second;
            for (u_t si: set1)
            {
                const uu_t seg(ep[si], ep[si + 1]);
            }
        }
    }
}
#endif

void Contention::print_solution(ostream &fo) const
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

    void (Contention::*psolve)() =
        (naive ? &Contention::solve_naive : &Contention::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Contention problem(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (problem.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        problem.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
