// -*-c++-*-
// (GPL)  Yotam Medini  yotam.medini@gmail.com -- Created: 2019/June/08

#include "bbintree.h"

template<typename _T, typename _D>
class IntervalBase
{
 public:
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

template<typename _T, typename _D>
class IntervalTree
{
 public:
    IntervalTree() {}
    typedef Interval<_T, _D> interval_t;
    typedef BBinTree<interval_t> tree_t;
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
