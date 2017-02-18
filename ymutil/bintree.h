// -*- C++ -*- Implementation of Balanced Binary tree

#include <functional>

template<typename _T>
class BinTreeNode
{
 public:
    typedef _T  data_t;
    typedef _T* data_ptr_t;
    typedef BinTreeNode<_T> node_t;
    typedef BinTreeNode<_T>* node_ptr_t;

    BinTreeNode(const data_t& v, node_ptr_t parent=0) :
        data(v),
        parent(0),
        child{0, 0}
    {}
    virtual ~BinTreeNode() 
    {
        delete child[0];
        delete child[1];
    }

    data_t data;
    node_ptr_t parent;
    node_ptr_t child[2];
};

template<typename _T>
class BinTreeNodeIter
{
    typedef BinTreeNodeIter<_T> self_t;
    typedef typename BinTreeNode<_T>::node_ptr_t node_ptr_t;
    typedef _T value_type;
    node_ptr_t node_ptr;

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

    BinTree() : root(0), cmp(_Cmp()) {}
    virtual ~BinTree()
    {
        delete root;
    }

    bool virtual insert(const data_t& v)
    {
        bool ret = insert_in(0, root, v);
        return ret;
    }

    bool virtual insert_in(node_ptr_t parent, node_ptr_t& subroot,
        const data_t& v)
    {
        bool ret = false;
        if (subroot)
        {
            if (cmp(v, subroot->data))
            {
                ret = insert_in(subroot, subroot->child[0], v);
            }
            else if (cmp(subroot->data, v))
            {
                ret = insert_in(subroot, subroot->child[1], v);
            }
        }
        else
        {
            ret = true;
            subroot = new node_t(v, parent);
        }
        return ret;
    }

    
 protected:
    node_ptr_t root;
    data_cmp_t cmp;
};
