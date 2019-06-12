// -*-c++-*-
// (GPL)  Yotam Medini  yotam.medini@gmail.com -- Created: 2019/June/08

#include "bbintree.h"

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
    typedef BBinTree<interval_t, std::less<interval_t>, _IntervalTreeCallBack<interval_t> > tree_t;
    typedef typename tree_t::node_t node_t;
    typedef typename tree_t::iterator iterator;
    typedef typename tree_t::node_ptr_t node_ptr_t;
    iterator insert(const _T &l, const _T &r)
    {
        return insert(interval_t(l, r));
    }
    iterator insert(const interval_t& i)
    {
        return _tree.insert(i);
    }
    const node_ptr_t get_root() const { return _tree.get_root(); }
 private:
    tree_t _tree;
};
