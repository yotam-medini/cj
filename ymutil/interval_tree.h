// -*-c++-*-
// (GPL)  Yotam Medini  yotam.medini@gmail.com -- Created: 2019/June/08

#include "bbintree.h"
#include <vector>

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
        set_rmax_by_children(q, s);
        for (q = (q == p ? q : q->parent); q != p; q = q->parent)
        {
            set_rmax_by_children(q, q);
        }
        set_rmax_by_children(s, p);
    }
    void remove_pre_balance(node_ptr_t p, node_ptr_t c)
    {
        node_ptr_t pp = p->parent;
        if (pp)
        {
            value_t rmax = pp->data.r;
            for (int ci = 0; ci < 2; ++ci)
            {
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
        max_by_pc(pr->data.rmax, pp->child[0]);
        max_by_pc(pr->data.rmax, pp->child[1]);
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
