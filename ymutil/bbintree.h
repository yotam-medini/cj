// -*- C++ -*- Implementation of Balanced Binary tree
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2017/03/05

#if !defined(_BBTreeH)
#define _BBTreeH 1

#include <functional>
#include <algorithm>

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

template<typename _T, typename _Cmp = std::less<_T> >
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
        node_ptr_t a;
        int ai = -1; // unset
        if (p->child[0] && p->child[1])
        {
            // find predecessor or successor to swap with
            int swapdir = p->balanced_factor  < 0 ? 0 : 1;
            // ai = swapdir;
            node_ptr_t s = p->child[swapdir];
            int swapdir1 = 1 - swapdir;
            for (node_ptr_t sc = s->child[swapdir1]; sc;
                s = sc, sc = sc->child[swapdir1])
            {}

            // swap p <-> s
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
            *pp = p->child[ci];
            if (p->child[ci])
            {
                p->child[ci]->parent = p->parent;
            }
        }

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

 protected:
    node_ptr_t root;
    data_cmp_t cmp;

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
};

template<typename _T, typename _Cmp = std::less<_T> >
bool
operator<(const BBinTree<_T, _Cmp>& tree0, const BBinTree<_T, _Cmp>& tree1)
{
    return BBinTree<_T, _Cmp>::lt(tree0, tree1);
}

#endif /* _BBTreeH */
