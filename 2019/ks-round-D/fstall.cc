// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
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
typedef vector<ul_t> vul_t;
typedef vector<ull_t> vull_t;
typedef pair<ull_t, u_t> ullu_t;

static u_t debug_flags = 0;

template<typename _T>
class BBinTreeNode
{
 public:
    typedef _T  data_t;
    typedef _T* data_ptr_t;
    typedef BBinTreeNode<_T> node_t;
    typedef BBinTreeNode<_T>* node_ptr_t;

    BBinTreeNode(const data_t& v, node_ptr_t p=0) :
        data(v),
        parent(p),
        child{0, 0},
        balanced_factor(0)
    {}

    virtual ~BBinTreeNode() {}

    virtual void delete_subtree()
    {
        for (int ci = 0; ci != 2; ++ci)
        {
            node_ptr_t c = child[ci];
            if (c)
            {
                 c->delete_subtree();
                 delete c;
            }
        }
    }

    virtual unsigned depth() const
    {
        unsigned d = 0;
        for (node_ptr_t p = parent; p; p = p->parent, ++d);
        return d;
    }

    virtual unsigned height() const
    {
        unsigned h0, h1;
        child_heights(h0, h1);
        unsigned ret = std::max(h0, h1) + 1;
        return ret;
    }

    virtual bool balanced() const
    {
        unsigned h;
        return balanced_height(h);
    }

    virtual bool valid_bf() const
    {
        int h;
        return valid_bf_height(h);
    }

    virtual void compute_hbf(unsigned& h, unsigned& bf) const
    {
        unsigned h0, h1;
        child_heights(h0, h1);
        h = std::max(h0, h1) + 1;
        int ih0 = h0, ih1 = h1;
        bf = ih1 - ih0;
    }

    virtual void child_heights(unsigned &h0, unsigned& h1) const
    {
        h0 = child[0] ? child[0]->height() : 0;
        h1 = child[1] ? child[1]->height() : 0;
    }

    data_t data;
    node_ptr_t parent;
    node_ptr_t child[2];
    int balanced_factor;

 private:
    bool balanced_height(unsigned& h) const
    {
        bool ret = true;
        unsigned ch[2] = {0, 0};
        for (int ci = 0; ret && ci != 2; ++ci)
        {
            if (child[ci])
            {
                ret = child[ci]->balanced_height(ch[ci]);
            }
        }
        if (ret)
        {
            if (ch[0] > ch[1])
            {
                std::swap(ch[0], ch[1]);
            }
            unsigned abs_bf = ch[1] - ch[0];
            ret = (abs_bf <= 1);
            h = ch[1] + 1;
        }
        return ret;
    }

    bool valid_bf_height(int& h) const
    {
        bool ret = true;
        int ch[2] = {0, 0};
        for (int ci = 0; ret && ci != 2; ++ci)
        {
            if (child[ci])
            {
                ret = child[ci]->valid_bf_height(ch[ci]);
            }
        }
        if (ret)
        {
            ret = (balanced_factor == ch[1] - ch[0]);
            h = std::max(ch[0], ch[1]) + 1;
        }
        return ret;
    }
};

template<typename _T, typename _RT>
class BBinTreeIter
{
 public:
    typedef BBinTreeIter<_T, _RT> self_t;
    typedef typename BBinTreeNode<_T>::node_ptr_t node_ptr_t;
    typedef _RT value_type;

    BBinTreeIter(node_ptr_t p=0) : node_ptr(p) {}
    BBinTreeIter(const BBinTreeIter<_T, _T>& i) : node_ptr(i.node()) {}
    virtual ~BBinTreeIter() {}

    void incr()
    {
        node_ptr_t pnext = node_ptr->child[1];
        if (pnext)
        {
            node_ptr = pnext;
            while ((pnext = node_ptr->child[0]))
            {
                node_ptr = pnext;
            }
        }
        else
        {
            pnext = node_ptr->parent;
            while (pnext && pnext->child[1] == node_ptr)
            {
                node_ptr = pnext;
                pnext = pnext->parent;
            }
            node_ptr = pnext;
        }
    }

    self_t& operator++()
    {
        incr();
        return *this;
    }

    self_t operator++(int)
    {
        self_t ret = *this;
        incr();
        return ret;
    }

    value_type&
    operator*()
    {
        return node_ptr->data;
    }

    const node_ptr_t node() const { return node_ptr; }
    node_ptr_t node() { return node_ptr; }

 private:
    node_ptr_t node_ptr;
};

template<typename _T, typename _RT>
inline bool
operator==(const BBinTreeIter<_T, _RT>& i0, const BBinTreeIter<_T, _RT>& i1)
{
    return i0.node() == i1.node();
}

template<typename _T, typename _RT>
inline bool
operator!=(const BBinTreeIter<_T, _RT>& i0, const BBinTreeIter<_T, _RT>& i1)
{
    return !(i0 == i1);
}

template<typename _T>
class BBinTreeCallBack
{
  public:
    typedef BBinTreeNode<_T>* node_ptr_t;
    virtual void insert_pre_balance(node_ptr_t p) {}
    virtual void remove_replace(node_ptr_t p, node_ptr_t s) {}
    virtual void remove_pre_balance(node_ptr_t p, node_ptr_t c) {}
    virtual void pre_rotate(node_ptr_t p, unsigned ci) {}
};

template<
    typename _T,
    typename _Cmp = std::less<_T>,
    typename _CallBack = BBinTreeCallBack<_T> >
class BBinTree
{
 public:

    typedef _T  data_t;
    typedef _T* data_ptr_t;
    typedef BBinTreeNode<_T> node_t;
    typedef BBinTreeNode<_T>* node_ptr_t;
    typedef node_ptr_t* node_pp_t;
    typedef _Cmp data_cmp_t;
    typedef BBinTreeIter<_T, _T> iterator;
    typedef BBinTreeIter<_T, const _T> const_iterator;
    typedef BBinTree<_T, _Cmp> self_t;
    typedef _CallBack callback_t;

    BBinTree() : root(0), cmp(_Cmp()) {}
    virtual ~BBinTree()
    {
        if (root)
        {
            root->delete_subtree();
            delete root;
        }
    }
    BBinTree(BBinTree&& x) : root(x.root), cmp(x.cmp)
    {
        x.root = 0;
    }
    BBinTree& operator=(BBinTree&& x)
    {
        if (this != &x)
        {
            root = x.root;
            cmp = x.cmp;
            x.root = 0;
        }
        return *this;
    }

    virtual const_iterator begin() const
    {
        return begin_fr(0);
    }

    virtual iterator begin()
    {
        const_iterator ci = begin_fr(0);
        return cit2it(ci);
    }

    virtual const_iterator rbegin() const
    {
        return begin_fr(1);
    }

    virtual iterator rbegin()
    {
        const_iterator ci = begin_fr(1);
        return cit2it(ci);
    }

    virtual const_iterator end() const
    {
        return const_iterator(0);
    }

    virtual iterator end()
    {
        return iterator(0);
    }

    virtual const_iterator begin_fr(unsigned ci) const
    {
        node_ptr_t p = root;
        if (p)
        {
            node_ptr_t next;
            while ((next = p->child[ci]))
            {
                p = next;
            }
        }
        const_iterator ret(p);
        return ret;
    }

    virtual iterator find(const data_t& v)
    {
        return iterator(find_ptr(v));
    }

    virtual iterator insert(const data_t& v)
    {
        node_ptr_t parent = 0;
        node_pp_t pp = &root, t = pp;
        node_ptr_t p = root, s = p;
        int i;
        while (p)
        {
            if (p->balanced_factor != 0)
            {
                t = pp;
                s = p;
            }
            parent = p;
            i = int(cmp(p->data, v));
            pp = &(p->child[i]);
            p = *pp;
        }
        node_ptr_t nv = new node_t(v, parent);
        if (parent)
        {
            int si = int(cmp(s->data, v));
            node_ptr_t chain = s->child[si];
            while (chain) // adjust balanced factor
            {
                int ci = int(cmp(chain->data, v));
                chain->balanced_factor = i2bf(ci);
                chain = chain->child[ci];
            }
            parent->child[i] = nv; // insertion
	    callback.insert_pre_balance(nv);

            // re-balance
            const int sbf = s->balanced_factor;
            const int a = i2bf(si);
            if (sbf == 0) // grown higher
            {
                s->balanced_factor = a;
            }
            else if (sbf == -a) // gotten more balanced
            {
                s->balanced_factor = 0;
            }
            else // rotate
            {
                if (sbf == s->child[si]->balanced_factor)
                {
                    // single rotation
                    p = s->child[si];
                    rotate(s, si);
                    s->balanced_factor = 0;
                }
                else
                {
                    // double rotation
                    node_ptr_t sc = s->child[si];
                    p = sc->child[1 - si];
                    rotate(sc, 1 - si);
                    rotate(s, si);
                    if (p == nv) // a singleton subtree
                    {
                        sc->balanced_factor = 0;
                        s->balanced_factor = 0;
                    }
                    else if (p->balanced_factor == sbf)
                    {
                        sc->balanced_factor = 0;
                        s->balanced_factor = -sbf;
                    }
                    else
                    {
                        sc->balanced_factor = sbf;
                        s->balanced_factor = 0;
                    }
                }
                p->balanced_factor = 0;
                *t = p;
            }
        }
        else
        {
            root = nv; // first inseertion
        }
        return iterator(nv);
    }

    virtual void remove(const data_t& v)
    {
        node_ptr_t p = find_ptr(v); // assume p != 0
        remove(iterator(p));
    }
        
    virtual void remove(iterator iter)
    {
        node_ptr_t p = iter.node();
        const data_t& v = *iter;
        node_pp_t pp = (p->parent
            ? &(p->parent->child[int(p == p->parent->child[1])])
            : &root);
        node_ptr_t a, s;
        int ai = -1; // unset
        if (p->child[0] && p->child[1])
        {
            // find predecessor or successor to swap with
            int swapdir = p->balanced_factor < 0 ? 0 : 1;
            // ai = swapdir;
            s = p->child[swapdir];
            int swapdir1 = 1 - swapdir;
            for (node_ptr_t sc = s->child[swapdir1]; sc;
                s = sc, sc = sc->child[swapdir1])
            {}

            // swap p <-> s
            callback.remove_replace(p, s);
            if (p->parent)
            {
                int pci = int(p == p->parent->child[1]);
                p->parent->child[pci] = s;
            }
            a = s->parent;
            node_ptr_t sc = s->child[swapdir];
            ai = int(a->child[1] == s);
            a->child[ai] = sc;
            s->parent = p->parent;
            if (a == p)
            {
                adopt(s, swapdir1, p->child[swapdir1]);
                a = s;
                ai = swapdir;
            }
            else
            {
                adopt(a, swapdir1, s->child[swapdir]);
                adopt(s, 0, p->child[0]);
                adopt(s, 1, p->child[1]);
            }
            s->balanced_factor = p->balanced_factor;
            *pp = s;
        }
        else
        {
            a = p->parent;
            ai = a ? int(a->child[1] == p) : -1;
            int ci = (p->child[1] != 0); // the only child index if exists
            s = p->child[ci];
            *pp = s;
            if (s)
            {
                s->parent = p->parent;
            }
        }
        callback.remove_pre_balance(p, s);

        // rebalance
        while (a)
        {
            node_ptr_t ap = a->parent;
            int ai_next = ap ? int(ap->child[1] == a) : -1;
            int bfi = i2bf(ai);
            int abf = a->balanced_factor;
            if (abf == 0)
            {
                a->balanced_factor = -bfi;
                a = 0;
            }
            else
            {
                if (abf == bfi)
                {
                    a->balanced_factor = 0;
                }
                else // rebalance
                {
                    //  balanced_factor is effectively +-2
                    int ai1 = 1 - ai;
                    node_ptr_t b = a->child[ai1];
                    int bbf = b->balanced_factor;
                    if (bbf == 0)
                    {
                        rotate(a, ai1);
                        b->balanced_factor = -abf;
                        ap = 0;
                    }
                    else if (abf == bbf)
                    {
                        rotate(a, ai1);
                        a->balanced_factor = 0;
                        b->balanced_factor = 0;
                    }
                    else if (abf == -bbf)
                    {
                        int bi = int(cmp(b->data, v));
                        node_ptr_t c = b->child[bi];
                        int cbf = c->balanced_factor;
                        rotate(b, bi);
                        rotate(a, ai1);
                        a->balanced_factor = (cbf == abf ? -cbf : 0);
                        b->balanced_factor = (cbf == -abf ? abf : 0);
                        c->balanced_factor = 0;
                    }
                }
                a = ap;
            }
            ai = ai_next;
        }

        delete p;
    }

    unsigned size() const
    {
        return subtree_size(root);
    }

    template <typename _SCmp>
    static bool lt(const node_t* p0, const node_t* p1, _SCmp scmp)
    {
         bool ret = p1;
         if (ret && p0)
         {
             bool plt = scmp(p0->data, p1->data);
             if (!(plt || scmp(p1->data, p0->data))) // "=="
             {
                 ret = lt(p0->child[0], p1->child[0], scmp) ||
                     ((!lt(p1->child[0], p0->child[0], scmp)) &&
                     lt(p0->child[1], p1->child[1], scmp));
             }
         }
         return ret;
    }

    static bool lt(const self_t& tree0, const self_t& tree1)
    {
         bool ret = lt(tree0.root, tree1.root, tree0.cmp);
         return ret;
    }

    virtual unsigned height() const { return (root ? root->height() : 0); }
    virtual bool balanced() const { return (root ? root->balanced() : true); }
    virtual bool valid_bf() const { return (root ? root->valid_bf() : true); }
    const node_ptr_t get_root() const { return root; } // for debug

 protected:
    node_ptr_t root;
    data_cmp_t cmp;
    callback_t callback;

 private:
    BBinTree(const BBinTree&) = delete;
    BBinTree& operator=(const BBinTree&) = delete;
    static int i2bf(int i)
    {
        return 2*i - 1; // {-1, 1}[i]
    }
    static void adopt(node_ptr_t p, int ci, node_ptr_t c)
    {
        p->child[ci] = c;
        if (c)
        {
            c->parent = p;
        }
    }

    node_ptr_t find_ptr(const data_t& v)
    {
        node_ptr_t p = root;
        bool found = false;
        while (p && !found)
        {
            const data_t& pd = p->data;
            bool lt = cmp(v, pd);
            bool gt = cmp(pd, v);
            found = !(lt || gt);
            if (!found)
            {
                p = p->child[int(gt)];
            }
        }
        return p;
    }

    void rotate(node_ptr_t p, unsigned ci)
    {
        callback.pre_rotate(p, ci);
        node_ptr_t q = p->child[ci];
        node_ptr_t pp = p->parent;
        if (pp)
        {
            int pi = (pp->child[1] == p);
            pp->child[pi] = q;
        }
        else
        {
            root = q;
        }
        q->parent = p->parent;
        p->parent = q;
        adopt(p, ci, q->child[1 - ci]);
        adopt(q, 1 - ci, p);
    }

    static iterator cit2it(const_iterator& ci)
    {
        return iterator(ci.node());
    }

    unsigned subtree_size(node_ptr_t p) const
    {
        unsigned size = 0;
        if (p)
        {
            size = 1 + subtree_size(p->child[0]) + subtree_size(p->child[1]);
        }
        return size;
    }
          
};

template<typename _T, typename _Cmp = std::less<_T> >
bool
operator<(const BBinTree<_T, _Cmp>& tree0, const BBinTree<_T, _Cmp>& tree1)
{
    return BBinTree<_T, _Cmp>::lt(tree0, tree1);
}

class Data
{
 public:
    typedef long int scalar_t;
    Data(scalar_t _v=0, u_t _xi=0) :
        v(_v), xi(_xi), count(1), vmin(_v), vmax(_v), vtotal(_v) {}
    scalar_t v;
    u_t xi;
    u_t count;
    scalar_t vmin, vmax;
    scalar_t vtotal;
};
bool operator<(const Data& d0, const Data& d1) 
{ 
    bool lt = (d0.v < d1.v) || ((d0.v == d1.v) && (d0.xi < d1.xi));
    return lt;
}
bool operator<=(const Data& d0, const Data& d1) 
{ 
    bool lt = (d0.v < d1.v) || ((d0.v == d1.v) && (d0.xi <= d1.xi));
    return lt;
}

class BBTCB_Stat : public BBinTreeCallBack<Data>
{
 public:
    typedef Data::scalar_t scalar_t;
    void insert_pre_balance(node_ptr_t p)
    {
        if (debug_flags & 0x2) { cerr << __func__ << '\n'; }
        scalar_t v = p->data.v;
        for (node_ptr_t pp = p->parent; pp; p = pp, pp = pp->parent)
        {
            pp->data.count += 1;
            min_by(pp, p);
            max_by(pp, p);
            pp->data.vtotal += v;
        }
    }
    void remove_replace(node_ptr_t p, node_ptr_t s)
    {
        if (debug_flags & 0x2) {
            cerr << __func__ << "p="<<p->data.v << ", s="<<s->data.v << '\n'; }
        s->data.count = p->data.count - 1;
        minmax_by_children(s, p->child);
        s->data.vtotal = p->data.vtotal - p->data.v;
        node_ptr_t pp = s->parent;
        if (pp != p)
        {
            int ci = int(s == pp->child[1]);
            int oci = 1 - ci;
            const node_ptr_t gc2[2] = {pp->child[oci], s->child[oci]};
            init_data_by_children(pp, gc2);
            for (pp = pp->parent; pp != p; pp = pp->parent)
            {
                re_minmax(pp);
                --(pp->data.count);
                pp->data.vtotal -= s->data.v;
            }
        }
    }
    void remove_pre_balance(node_ptr_t p, node_ptr_t c)
    {
        if (debug_flags & 0x2) {
            cerr << __func__ << " p="<<p->data.v << ", c=";
            if (c) {cerr << c->data.v;} cerr << '\n'; }
        node_ptr_t corp = c ? : p;
        scalar_t v = corp->data.v;
        for (node_ptr_t pp = corp->parent; pp; pp = pp->parent)
        {
            init_minmax(pp);
            --(pp->data.count);
            re_minmax(pp);
            if (c && (pp == p->parent))
            {
                v = p->data.v;
            }
            pp->data.vtotal -= v;
        }        
    }
    void pre_rotate(node_ptr_t p, unsigned ci)
    {
        if (debug_flags & 0x2) { cerr << __func__ << '\n'; }
        node_ptr_t c = p->child[ci];
        c->data.count = p->data.count;
        minmax_by(c, p);
        int oci = 1 - ci;
        node_ptr_t oc = p->child[oci];
        node_ptr_t gc2[2] = {c->child[oci], oc};
        init_data_by_children(p, gc2);
        const node_ptr_t cc = c->child[ci];
        c->data.vtotal = c->data.v + p->data.vtotal +
            (cc ? cc->data.vtotal : 0);
    }
 private:
    static void init_data_by_children(node_ptr_t p, const node_ptr_t* c2)
    {
        Data& d = p->data;
        d.count = 1;
        d.vmin = d.vmax = d.vtotal = d.v;
        for (int ci = 0; ci != 2; ++ci)
        {
            const node_ptr_t c = c2[ci];
            if (c)
            {
                d.count += c->data.count;
                minmax_by(p, c);
                d.vtotal += c->data.vtotal;
            }
        }
    }
    static void init_data(node_ptr_t p)
    {
        p->data.count = 1;
        init_minmax(p);
    }
    static void init_minmax(node_ptr_t p)
    {
        Data& d = p->data;
        d.vmin = d.vmax = d.v;
    }
    static void re_minmax(node_ptr_t p)
    {
        Data& data = p->data;
        data.vmin = data.vmax = data.v;
        minmax_by_children(p, p->child);
    }
    static void minmax_by_children(node_ptr_t p, const node_ptr_t* chldren)
    {
        for (int ci = 0; ci != 2; ++ci)
        {
            const node_ptr_t pc = chldren[ci];
            if (pc)
            {
                minmax_by(p, pc);
            }
        }
    }
    static void minmax_by(node_ptr_t p, const node_ptr_t c)
    {
        min_by(p, c);
        max_by(p, c);
    }
    static void min_by(node_ptr_t p, const node_ptr_t c)
    {
        if (p->data.vmin > c->data.vmin)
        {
            p->data.vmin = c->data.vmin;
        }
    }
    static void max_by(node_ptr_t p, const node_ptr_t c)
    {
        if (p->data.vmax < c->data.vmax)
        {
            p->data.vmax = c->data.vmax;
        }
    }
};

typedef BBinTree<Data, std::less<Data>, BBTCB_Stat> bbtstat_t;

class Stall
{
 public:
    Stall(ull_t _x=0, ull_t _c=0, ul_t _i=0):
        x(_x), c(_c), i(_i) {}
    ull_t cost(ull_t wx) const
    {
        ull_t d = (x < wx ? wx - x : x - wx);
        return c + d;
    }
    ull_t x;
    ull_t c;
    u_t i;
};
typedef vector<Stall> vstall_t;

typedef map<ullu_t, Stall> ullu2stall_t;
typedef ullu2stall_t::iterator ullu2stalliter_t;
typedef ullu2stall_t::const_iterator ullu2stallciter_t;
typedef vector<ullu2stalliter_t> vullu2stalliter_t;

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

class SubSum
{
 public:
   SubSum(ull_t b, ull_t x, ull_t o) : 
       bound(b), xbound(x), offset(o), count(0), cost(0) {}
   const ull_t bound;
   const ull_t xbound;
   const ull_t offset;
   u_t count;
   ull_t cost;
};

class Fstall
{
 public:
    Fstall(istream& fi);
    void solve_naive();
    void solve_naive1();
    void solve_naive2();
    void solve();
    void print_solution(ostream&) const;
 private:
    void xstalls_sort();
    ull_t comb_price(const vu_t &comb) const;
    void advance_low(u_t add)
    {
        for (ilow += add; (ilow < n) && stalls[ilow].i >= ipre; ++ilow) {}
        next_low_cost = (ilow < n ? stalls[ilow].cost(curr_wx) : infty);
        min_next_cost = min(next_low_cost, next_high_cost);
    }
    void advance_high(u_t add)
    {
        for (ihigh += add; (ihigh < n) && stalls[ihigh].i <= icurr; ++ihigh) {}
        next_high_cost = (ihigh < n ? stalls[ihigh].cost(curr_wx) : infty);
        min_next_cost = min(next_low_cost, next_high_cost);
    }
    ull_t ware_total_cost(u_t wi) const
    {
        ull_t wx = xstalls[wi].x;
        ull_t cost = xstalls[wi].cost(wx);
        for (u_t i = 0, ns = 0; ns < k; ++i)
        {
            if (stalls[i].i != wi)
            {
                ull_t costi = stalls[i].cost(wx);
                cost += costi;
                ++ns;
            }
        }
        return cost;
    }
    ull_t left_right_cost(u_t wi) const;
    ull_t bt_left_right_cost(u_t xi) const;
    void subsum(
        const bbtstat_t& bts, ull_t bound, ull_t xbound, ull_t offset, 
        u_t& count, ull_t& cost) const;
    void subsum(const bbtstat_t::node_t* p, SubSum& ss) const;
    static const ull_t infty;
    u_t k, n;
    vull_t x;
    vull_t c;
    ull_t solution;
    vstall_t stalls;
    vstall_t xstalls;
    u_t ilow, ihigh, ipre, icurr;
    ull_t curr_wx;
    ull_t next_low_cost;
    ull_t next_high_cost;
    ull_t min_next_cost;
    ullu2stall_t left_stalls;
    ullu2stall_t right_stalls;
    vullu2stalliter_t xiters;

    // non-naive
    bbtstat_t bbt_left, bbt_right;
    vector<bbtstat_t::iterator> vbtiters;
    ull_t single_max_cost;
};
const ull_t Fstall::infty = ull_t(-1);

Fstall::Fstall(istream& fi) : solution(0)
{
    fi >> k >> n;
    ull_t v;
    x.reserve(n);
    c.reserve(n);
    for (u_t i = 0; i < n; ++i)
    {
        fi >> v;
        x.push_back(v);
    }
    for (u_t i = 0; i < n; ++i)
    {
        fi >> v;
        c.push_back(v);
    }
}

void Fstall::solve_naive()
{
    vu_t comb;
    combination_first(comb, n, k + 1);

    ull_t best_price = comb_price(comb);
    while (combination_next(comb, n))
    {
        ull_t price = comb_price(comb);
        if (best_price > price)
        {
            best_price = price;
        }
    }
    solution = best_price;
}

void Fstall::solve_naive1()
{
    stalls.reserve(n);
    stalls.clear();
    for (u_t i = 0; i < n; ++i)
    {
        stalls.push_back(Stall(x[i], c[i]));
    }
    sort(stalls.begin(), stalls.end(),
        [](const Stall& s0, const Stall& s1)
        {
            return s0.x < s1.x;
        });
    for (u_t i = 0; i < n; ++i)
    {
        stalls[i].i = i;
    }
    xstalls = stalls;

    ull_t wx0 = stalls[0].x;
    sort(stalls.begin(), stalls.end(),
        [wx0](const Stall& s0, const Stall& s1)
        {
            return s0.cost(wx0) < s1.cost(wx0);
        });
    ull_t best = ware_total_cost(0);

    vstall_t stalls_next;
    for (ipre = 0, icurr = 1; icurr < n; ipre = icurr++)
    {
        curr_wx = xstalls[icurr].x;
        ull_t pre_cost = xstalls[ipre].cost(curr_wx);
        ull_t curr_cost = xstalls[icurr].cost(curr_wx);
        ull_t min_pre_curr_cost = min(pre_cost, curr_cost);
        ilow = ihigh = 0;
        min_next_cost = infty;
        advance_low(0);
        advance_high(0);
        stalls_next.clear();
        while (stalls_next.size() != n)
        {
            if (min_pre_curr_cost <= min_next_cost)
            {
                if (pre_cost <= curr_cost)
                {
                    stalls_next.push_back(xstalls[ipre]);
                    pre_cost = infty;
                }
                else
                {
                    stalls_next.push_back(xstalls[icurr]);
                    curr_cost = infty;
                }
                min_pre_curr_cost = min(pre_cost, curr_cost);
            }
            else
            {
                if (next_low_cost <= next_high_cost)
                {
                    stalls_next.push_back(stalls[ilow]);
                    advance_low(1);
                }
                else
                {
                    stalls_next.push_back(stalls[ihigh]);
                    advance_high(1);
                }
            }
        }
        swap(stalls, stalls_next);
        ull_t curr_total_cost = ware_total_cost(icurr);
        if (best > curr_total_cost)
        {
            best = curr_total_cost;
        }
    }
    solution = best;
}

void Fstall::solve_naive2()
{
    xstalls_sort();
    const ull_t xmin = xstalls.front().x;
    const ull_t xmax = xstalls.back().x;
    left_stalls.clear();
    right_stalls.clear();
    for (u_t xi = 0; xi < n; ++xi)
    {
        Stall& s = xstalls[xi];
        s.i = xi;
        ull_t xmin_cost = s.cost(xmin);
        ullu2stall_t::value_type v(ullu_t(xmin_cost, xi), s);
        ullu2stalliter_t xiter = right_stalls.insert(right_stalls.end(), v);
        xiters.push_back(xiter);
    }
    ull_t best_cost = infty;
    for (u_t xi = 0; xi < n; ++xi)
    {
        const Stall& s = xstalls[xi];
        ullu2stalliter_t xiter = xiters[xi];
        right_stalls.erase(xiter);
        ull_t xmax_cost = s.cost(xmax);
        ullu2stall_t::value_type v(ullu_t(xmax_cost, xi), s);
        ull_t lr_cost = left_right_cost(xi);
        if (best_cost > lr_cost)
        {
            best_cost = lr_cost;
        }
        xiter = left_stalls.insert(left_stalls.end(), v);
        xiters[xi] = xiter; // not needed
    }
    solution = best_cost;
}

void Fstall::solve()
{
    xstalls_sort();
    const ull_t xmin = xstalls.front().x;
    const ull_t xmax = xstalls.back().x;
    single_max_cost = *(max_element(c.begin(), c.end()));
    single_max_cost += (xmax - xmin);
    for (u_t xi = 0; xi < n; ++xi)
    {
        const Stall& stall = xstalls[xi];
        ull_t v = stall.cost(xmin);
        bbtstat_t::iterator iter = bbt_right.insert(Data(v, xi));
	vbtiters.push_back(iter);
    }
    ull_t best_cost = infty;
    for (u_t xi = 0; xi < n; ++xi)
    {
        const Stall& stall = xstalls[xi];
        bbt_right.remove(vbtiters[xi]);
	ull_t xi_cost = bt_left_right_cost(xi);
	if (best_cost > xi_cost)
	{
	    best_cost = xi_cost;
	}
	ull_t v = stall.cost(xmax);
	bbt_left.insert(Data(v, xi));
    }
    solution = best_cost;
}

void Fstall::xstalls_sort()
{
    xstalls.clear();
    xstalls.reserve(n);
    for (u_t i = 0; i < n; ++i)
    {
        xstalls.push_back(Stall(x[i], c[i]));
    }
    sort(xstalls.begin(), xstalls.end(),
        [](const Stall& s0, const Stall& s1)
        {
            return s0.x < s1.x;
        });
}

ull_t Fstall::comb_price(const vu_t &comb) const
{
    ull_t base = 0;
    for (u_t pi: comb)
    {
        base += c[pi];
    }
    ull_t smin_distance(-1);
    for (u_t p: comb)
    {
        ull_t xp = x[p];
        ull_t sdist = 0;
        for (u_t q: comb)
        {
            ull_t xq = x[q];
            ull_t delta = (xp < xq ? xq - xp : xp - xq);
            sdist += delta;
        }
        if (smin_distance > sdist)
        {
            smin_distance = sdist;
        }
    }
    ull_t ret = base + smin_distance;
    return ret;
}

ull_t Fstall::left_right_cost(u_t wi) const
{
    const Stall& ware = xstalls[wi];
    u_t ret = ware.c;
    ull_t wx = ware.x;
    ullu2stallciter_t liter = left_stalls.begin();
    ullu2stallciter_t riter = right_stalls.begin();
    const ullu2stallciter_t el = left_stalls.end(), er = right_stalls.end();
    u_t si = 0;
    for ( ; (si < k) && (liter != el) && (riter != er); ++si)
    {
        ull_t lcost = (*liter).second.cost(wx);
        ull_t rcost = (*riter).second.cost(wx);
        if (lcost < rcost)
        {
            ret += lcost;
            ++liter;
        }
        else
        {
            ret += rcost;
            ++riter;
        }
    }
    for ( ; (si < k) && (liter != el); ++si, ++liter)
    {
        ull_t lcost = (*liter).second.cost(wx);
        ret += lcost;
    }
    for ( ; (si < k) && (riter != er); ++si, ++riter)
    {
        ull_t rcost = (*riter).second.cost(wx);
        ret += rcost;
    }
    return ret;
}

ull_t Fstall::bt_left_right_cost(u_t xi) const
{
    const Stall& ware = xstalls[xi];
    const ull_t xmin = xstalls.front().x;
    const ull_t xmax = xstalls.back().x;
    ull_t cost = 0;
    ull_t low = 0, high = single_max_cost;
    u_t xilow = 0, xihigh = n - 1, ximid = xihigh;
    bool bisect = true;
    while (bisect)
    {
        if (low > high) { cerr << "ERROR @ " << __LINE__ << '\n'; exit(1); }
        u_t lcount = 0, rcount = 0;
        ull_t lcost = 0, rcost = 0;
        ull_t mid = (low + high)/2;
        if (low == mid)
        {
            low = mid = high;
            ximid = (xilow + xihigh + 1)/2;
        }
        subsum(bbt_left,  mid, ximid, xmax - ware.x, lcount, lcost);
        subsum(bbt_right, mid, ximid, ware.x - xmin, rcount, rcost);
        u_t lr_count = lcount + rcount;
        if (lr_count < k)
        {
            if (low != high)
            {
                low = mid;
            }
            else
            {
                xilow = ximid + 1;
            }
        }
        else if (lr_count == k)
        {
            bisect = false;
            cost = lcost + rcost;
        }
        else
        {
            if (low != high)
            {
                high = mid;
            }
            else
            {
                xihigh = ximid - 1;
            }
        }
    }
    cost += xstalls[xi].c;
    return cost;
}

void Fstall::subsum(
    const bbtstat_t& bts, ull_t bound, ull_t xbound, ull_t offset, 
    u_t& count, ull_t& cost) const
{
    SubSum ss(bound, xbound, offset);
    subsum(bts.get_root(), ss);
    count = ss.count;
    cost = ss.cost;
}

void Fstall::subsum(const bbtstat_t::node_t *p, SubSum& ss) const
{
    if (p)
    {
        bbtstat_t::node_t *pc0 = p->child[0];
        const Data bound(ss.bound + ss.offset, ss.xbound);
        if (p->data <= bound)
        {
            ++ss.count;
            ss.cost += (p->data.v - ss.offset);
            if (pc0)
            {
                const Data& d0 = pc0->data;
                ss.count += d0.count;
                ss.cost += (d0.vtotal - d0.count * ss.offset);
            }
            subsum(p->child[1], ss);
        }
        else
        {
            subsum(pc0, ss);
        }
    }
}

void Fstall::print_solution(ostream &fo) const
{
    fo << ' ' << solution;
}

int main(int argc, char ** argv)
{
    const string dash("-");

    bool naive = false;
    bool naive1 = false;
    bool naive2 = false;
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
        else if (opt == string("-naive1"))
        {
            naive1 = true;
        }
        else if (opt == string("-naive2"))
        {
            naive2 = true;
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

    void (Fstall::*psolve)() =
        (naive ? &Fstall::solve_naive
             : (naive1 ? &Fstall::solve_naive1
             : (naive2 ? &Fstall::solve_naive2 : &Fstall::solve)));
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Fstall fstall(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (fstall.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        fstall.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
