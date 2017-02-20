// -*- C++ -*- Implementation of Balanced Binary tree

#include <functional>
// #include <algorithm>

enum class BinTreeBalanceFactor { LEFT = -1, EQ = 0, RIGHT = 1 };

template<typename _T>
class BinTreeNode
{
 public:
    typedef _T  data_t;
    typedef _T* data_ptr_t;
    typedef BinTreeNode<_T> node_t;
    typedef BinTreeNode<_T>* node_ptr_t;
    typedef BinTreeBalanceFactor EBF;

    BinTreeNode(const data_t& v, node_ptr_t p=0) :
        data(v),
        parent(p),
        child{0, 0},
        balnace_factor(EBF::EQ)
    {}
    virtual ~BinTreeNode() 
    {
        delete child[0];
        delete child[1];
    }

    virtual unsigned height() const
    {
        unsigned h0 = child[0] ? child[0]->height() : 0;
        unsigned h1 = child[1] ? child[1]->height() : 0;
        unsigned ret = std::max(h0, h1) + 1;
        return ret;
    }

    static BinTreeBalanceFactor i2bf(unsigned i) 
    { 
        EBF bflr[2] = {EBF::LEFT, EBF::RIGHT};
        return bflr[i]; 
    }

    data_t data;
    node_ptr_t parent;
    node_ptr_t child[2];
    BinTreeBalanceFactor balnace_factor;
};

template<typename _T>
class BinTreeIter
{
 public:
    typedef BinTreeIter<_T> self_t;
    typedef typename BinTreeNode<_T>::node_ptr_t node_ptr_t;
    typedef _T value_type;
    node_ptr_t node_ptr;

    BinTreeIter(node_ptr_t p=0) : node_ptr(p) {}
    virtual ~BinTreeIter() {}

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
    
};

template<typename _T>
inline bool
operator==(const BinTreeIter<_T>& i0, const BinTreeIter<_T>& i1)
{
    return i0.node_ptr == i1.node_ptr;
}

template<typename _T>
inline bool
operator!=(const BinTreeIter<_T>& i0, const BinTreeIter<_T>& i1)
{
    return !(i0 == i1);
}

template<typename _T, typename _Cmp = std::less<_T> >
class BinTree
{
 public:

    typedef _T  data_t;
    typedef _T* data_ptr_t;
    typedef BinTreeNode<_T> node_t;
    typedef BinTreeNode<_T>* node_ptr_t;
    typedef node_ptr_t* node_pp_t;
    typedef _Cmp data_cmp_t;
    typedef BinTreeIter<_T> iterator;
    typedef BinTreeIter<const _T> const_iterator;
    typedef BinTreeBalanceFactor EBF;

    BinTree() : root(0), cmp(_Cmp()) {}
    virtual ~BinTree()
    {
        delete root;
    }

    virtual iterator begin()
    {
        return begin_fr(0);
    }

    virtual iterator rbegin()
    {
        return begin_fr(1);
    }

    virtual iterator end()
    {
        return iterator(0);
    }

    virtual iterator begin_fr(unsigned ci)
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
        iterator ret(p);
        return ret;
    }

    virtual void insert(const data_t& v)
    {
        node_ptr_t parent = 0;
        node_ptr_t p = root;
        int i;
        while (p)
        {
            parent = p;
            i = int(cmp(p->data, v));
            p = p->child[i];
        }
        node_ptr_t nv = new node_t(v, parent);
        if (parent)
        {
            parent->child[i] = nv;
#if 0
            EBF& pbf = parent->balnace_factor;
            if (pbf == node_t::i2bf(1 - i))
            {
                pbf = EBF::EQ;
            }
            else if (pbf == EBF::EQ)
            {
                pbf = node_t::i2bf(i))
            }
#endif
        }
        else
        {
            root = nv;
        }
    }

    virtual unsigned height() const { return (root ? root->height() : 0); }
    
 protected:
    node_ptr_t root;
    data_cmp_t cmp;
};
