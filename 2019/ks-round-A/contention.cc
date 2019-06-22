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

#include <functional>

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


template<typename _T, typename _D>
class IntervalBase
{
 public:
    typedef _T value_t;
    IntervalBase(const _T &_l=9, const _T &_r=0, const _D &_d=_D()) :
        l(_l), r(_r), d(_d) 
        {}
    _T l, r;
    _D d;
};

template<typename _T, typename _D>
bool operator<(const IntervalBase<_T, _D> &i0, const IntervalBase<_T, _D> &i1)
{
    bool lt = (i0.l < i1.l) || ((i0.l == i1.l) && (i0.r < i1.r));
    return lt;
}

template<typename _T, typename _D>
bool operator==(const IntervalBase<_T, _D> &i0, const IntervalBase<_T, _D> &i1)
{
    bool eq = (i0.l == i1.l) && (i0.r == i1.r);
    return eq;
}

template<typename _T, typename _D>
class Interval: public IntervalBase<_T, _D>
{
 public:
    Interval<_T, _D>(const _T &_l=0, const _T &_r=0, const _D &_d=_D()) :
        IntervalBase<_T, _D>(_l, _r),
        rmax(_r)
    {}
    _T rmax;
};

template<typename _Interval>
class _IntervalTreeCallBack : public BBinTreeCallBack<_Interval>
{
 public:
    // typedef typename BBinTree<_T>::node_ptr_t node_ptr_t;
    // typedef Interval<_T, _D> interval_t;
    typedef typename _Interval::value_t value_t;
    typedef typename BBinTreeNode<_Interval>::data_t data_t;
    typedef typename BBinTreeNode<_Interval>::node_ptr_t node_ptr_t;
    void insert_pre_balance(node_ptr_t p)
    {
        const value_t rmax = p->data.rmax;
        for (node_ptr_t pp = p->parent; pp && (pp->data.rmax < rmax); 
            p = pp, pp = pp->parent)
        {
            pp->data.rmax = rmax;
        }
    }
    void remove_replace(node_ptr_t p, node_ptr_t s)
    {
        node_ptr_t q = s->parent;
        int ci = int(q->child[1] == s);
        int cic = 1 - ci;
        if (p == q)
        {
            max_by_pc(s->data.rmax, p->child[cic]);
        }
        else
        {
            q->data.rmax = q->data.r;
            max_by_pc(q->data.rmax, q->child[cic]);
            max_by_children(q->data.rmax, s);
            for (q = q->parent; q != p; q = q->parent)
            {
                set_rmax_by_children(q, q);
            }
            set_rmax_by_children(s, p);
        }
    }
    void remove_pre_balance(node_ptr_t p, node_ptr_t c)
    {
        node_ptr_t pp = p->parent;
        if (pp)
        {
            value_t rmax = pp->data.r;
            for (int ci = 0; ci < 2; ++ci)
            {
                max_by_pc(rmax, pp->child[ci]);
                max_by_pc(rmax, p->child[ci]);
            }
            bool reducing = (pp->data.rmax > rmax);
            while (reducing && pp)
            {
                int ci = int(pp->child[1] == p);
                int cic = 1 - ci;
                max_by_val(rmax, pp->data.r);
                max_by_pc(rmax, pp->child[cic]);
                reducing = (pp->data.rmax > rmax);
                if (reducing)
                {
                    pp->data.rmax = rmax;
                    p = pp;
                    pp = pp->parent;
                }
            }
        }
    }
    void pre_rotate(node_ptr_t p, unsigned ci)
    {
        node_ptr_t pc = p->child[ci];
        _Interval& pinterval = p->data;
        _Interval& cinterval = pc->data;
        int cic = 1 - ci;
        value_t prmax = pinterval.r;
        max_by_pc(prmax, p->child[cic]);
        max_by_pc(prmax, pc->child[cic]);
        value_t crmax = cinterval.r;
        max_by_val(crmax, prmax);
        max_by_pc(crmax, pc->child[ci]);
        pinterval.rmax = prmax;
        cinterval.rmax = crmax;
    }
 private:
    void set_rmax_by_children(node_ptr_t pr, node_ptr_t pp)
    {
        pr->data.rmax = pr->data.r;
        max_by_children(pr->data.rmax, pp);
    }
    void max_by_children(value_t  &rmax, node_ptr_t p)
    {
        max_by_pc(rmax, p->child[0]);
        max_by_pc(rmax, p->child[1]);
    }
    void max_by_pc(value_t& rmax, node_ptr_t pc)
    {
        if (pc)
        {  
            max_by_val(rmax, pc->data.rmax);
        }
    }
    void max_by_val(value_t& rmax, value_t v)
    {
        if (rmax < v)
        {
            rmax = v;
        }
    }
};

template<typename _T, typename _D>
class IntervalTree
{
 public:
    IntervalTree() {}
    typedef Interval<_T, _D> interval_t;
    typedef typename interval_t::value_t value_t;
    typedef BBinTree<interval_t, std::less<interval_t>, _IntervalTreeCallBack<interval_t> > tree_t;
    typedef typename tree_t::node_t node_t;
    typedef typename tree_t::iterator iterator;
    typedef typename tree_t::node_ptr_t node_ptr_t;
    typedef std::vector<interval_t> vinterval_t;
    iterator insert(const _T &l, const _T &r)
    {
        return insert(interval_t(l, r));
    }
    iterator insert(const interval_t& i)
    {
        return _tree.insert(i);
    }
    void remove(iterator iter)
    {
        _tree.remove(iter);
    }
    void search(vinterval_t& found, const interval_t& x) const
    {
        sub_search(get_root(), found, x);
    }
    static node_ptr_t node_ptr(iterator iter) { return iter.node(); }
    const node_ptr_t get_root() const { return _tree.get_root(); }
 private:
    void sub_search(node_ptr_t p, vinterval_t& found, const interval_t& x) const
    {
        if (p)
        {
            const interval_t& y = p->data;
            // if (!((x.r <= y.l) || (x.l >= y.r)))
            // if ((y.l < x.r) && (x.l < y.rmax))
            if (x.l < y.rmax)
            {
                if ((y.l < x.r) && (x.l < y.r))
                {
                    found.push_back(y);
                }
                sub_search(p->child[0], found, x);
                if (y.l < x.r)
                {
                    sub_search(p->child[1], found, x);
                }
            }
        }
    }
    tree_t _tree;
};



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
typedef array<u_t, 2> u2_t;
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

class SegCount
{
 public:
    SegCount(const u2_t &_seg={0, 0}, u_t _count=0) : 
        seg(_seg), count(_count) {}
    u2_t seg;
    u_t count;
};
typedef vector<SegCount> vsegcount_t;

bool operator<(const SegCount &sc0, const SegCount &sc1)
{
    return sc0.seg < sc1.seg;
}

class Contention
{
 public:
    Contention(istream& fi);
    void solve_naive();
    // void solve_better();
    void solve();
    void print_solution(ostream&) const;
 private:
    bool distinct() const
    { 
        setuu_t set_bookings(bookings.begin(), bookings.end());
        bool eq_size = set_bookings.size() == q;
        return eq_size;
    }
    void set_seg_count();
    u_t n, q;
    vuu_t bookings;
    u_t solution;
    vsegcount_t seg_count;
    itree_t itree;
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
        set_seg_count();
        vector<itree_t::iterator> bookings_iterators;
        bookings_iterators.reserve(q);
        for (u_t bi = 0; bi < q; ++bi)
        {
            const uu_t &b = bookings[bi];
            itree_t::iterator i = itree.insert(interval_t(b.first, b.second));
            (*i).d = bi;
            bookings_iterators.push_back(i);
        }
        for (auto const &x: seg_count)
        {
            if (x.count == 1)
            {
                vinterval_t vi;
                const u2_t &seg = x.seg;
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
            SegCount seg_booking({booking.first, booking.first});
            auto er = equal_range(seg_count.begin(), seg_count.end(), 
                seg_booking);
            for (vsegcount_t::iterator segi = er.first; 
                (segi != seg_count.end()) && ((*segi).seg[0] < booking.second);
                ++segi)
            {
                if ((--(*segi).count) == 1)
                {
                    const u2_t &seg = (*segi).seg;
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

void Contention::set_seg_count()
{
    setu_t pts;
    vector<pair<u_t, int>> pt_delta;
    pt_delta.reserve(bookings.size());
    for (const uu_t &b: bookings)
    {
        pts.insert(pts.end(), b.first);
        pts.insert(pts.end(), b.second);
        pt_delta.push_back(uu_t(b.first, +1));
        pt_delta.push_back(uu_t(b.second, -1));
    }
    sort(pt_delta.begin(), pt_delta.end());
    seg_count.reserve(pts.size());
    

    vector<pair<u_t, int>>::const_iterator pdi = pt_delta.begin();
    setu_t::const_iterator pts_ci = pts.begin();
    u_t pt_last = *pts_ci;
    u_t count = 0;
    for (++pts_ci; pts_ci != pts.end(); ++pts_ci)
    {
        u_t pt = *pts_ci;
        for (; (pdi != pt_delta.end() && pdi->first == pt_last); ++pdi)
        {
            count += pdi->second;
        }
        
        seg_count.push_back(SegCount(u2_t({pt_last, pt}), count));
        pt_last = pt;
    }

}

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
