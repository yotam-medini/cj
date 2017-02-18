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

    BinTreeNode(const data_t& v) :
        data(v),
        child{0, 0}
    {}
    virtual ~BinTreeNode() 
    {
        delete child[0];
        delete child[1];
    }

    data_t data;
    node_ptr_t child[2];
};

template<typename _T, typename _Cmp = std::less<_T> >
class BinTree
{
 public:

    typedef _T  data_t;
    typedef _T* data_ptr_t;
    typedef BinTreeNode<_T> node_t;
    typedef BinTreeNode<_T>* node_ptr_t;
    typedef _Cmp data_cmp_t;

    BinTree() : root(0), cmp(_Cmp()) {}
    virtual ~BinTree()
    {
        delete root;
    }

    bool virtual insert(const data_t& v)
    {
        node_ptr_t* w = search(&root, v);
        return w;
    }

    node_ptr_t* search(node_ptr_t* pp, const data_t &v)
    {
        node_ptr_t* ret = pp;
        node_ptr_t p = *pp;
        if (p)
        {
            if (cmp(v, p->data))
            {
                ret = search(&p->child[0], v);
            }
            else if (cmp(p->data, v))
            {
                ret = search(&p->child[1], v);
            }
        }
        return ret;
    }    
    
 protected:
    node_ptr_t root;
    data_cmp_t cmp;
};
