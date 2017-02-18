// -*- C++ -*- Implementation of Balanced Binary tree

#include <functional>

template<typename _T>
class BinTreeNode
{
 public:
    typedef enum { BF_LEFT = -1, BF_EQ = 0, BF_RIGHT = 1 } BF_t;
    typedef _T  data_t;
    typedef _T* data_ptr_t;
    typedef BinTreeNode<_T> node_t;
    typedef BinTreeNode<_T>* node_ptr_t;

    BinTreeNode(const data_t& v, node_ptr_t parent=0) :
        data(v),
        parent(0),
        child{0, 0},
        balnace_factor(BF_EQ)
    {}
    virtual ~BinTreeNode() 
    {
        delete child[0];
        delete child[1];
    }

    data_t data;
    node_ptr_t parent;
    node_ptr_t child[2];
    BF_t balnace_factor;
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
    
};

template<typename _T>
inline bool
operator==(const BinTreeIter<_T>& i0, const BinTreeIter<_T>& i1)
{
    return i0.node_ptr != i1.node_ptr;
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

    BinTree() : root(0), cmp(_Cmp()) {}
    virtual ~BinTree()
    {
        delete root;
    }

    virtual iterator begin()
    {
        return begin_end(0);
    }

    virtual iterator end()
    {
        return begin_end(1);
    }

    virtual iterator begin_end(unsigned ci)
    {
        node_pp_t pp = &root;
        while (*pp)
        {
            pp = &((*pp)->child[ci]);
        }
        iterator ret(*pp);
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
        }
        else
        {
            root = nv;
        }
    }

    
 protected:
    node_ptr_t root;
    data_cmp_t cmp;
};
