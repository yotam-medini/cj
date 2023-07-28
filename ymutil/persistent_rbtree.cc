#include <algorithm>
#include <initializer_list>
#include <iosfwd>

enum _tree_color { RED = false, BLACK = true };

class _PersistentRBTreeNodeBase
{
 public:
    _PersistentRBTreeNodeBase(
        _tree_color _color=BLACK,
        _PersistentRBTreeNodeBase* _parent=nullptr,
        _PersistentRBTreeNodeBase* _left=nullptr,
        _PersistentRBTreeNodeBase* _right=nullptr) :
            color(_color), parent(_parent), child{_left, _right} {}
    typedef const _PersistentRBTreeNodeBase* cpointer;
    _tree_color color; // red=false, black=true
    _PersistentRBTreeNodeBase *parent;
    _PersistentRBTreeNodeBase *child[2]; // left, right

    size_t height() const
    { 
        const size_t h0 = child[0]->height();
        const size_t h1 = child[1]->height();
        size_t h =  1 + std::max(h0, h1);
        return h;
    }
    bool is_balanced() const
    {
        return is_balanced(unused_height);
    }
 private:
    inline static size_t unused_height;
    bool is_balanced(size_t& h) const
    {
        bool balanced = true;
        size_t hc[2]{0, 0};
        for (int i: {0, 1})
        {
            cpointer c = child[i];
            balanced = balanced && (c ? c->is_balanced(hc[i]) : true);
        }
        if (balanced)
        {
            if (hc[0] > hc[1]) { std::swap(hc[0], hc[1]); }
            h = hc[1];
            balanced = (h <= 2*hc[0]);
        }
        return balanced;
    }
};


// template <typename K, typename V> class PersistentRBTree; // forward

template <typename K, typename V>
class _PersistentRBTreeNode : public _PersistentRBTreeNodeBase
{
 public:
    typedef _PersistentRBTreeNode<K, V> self_t;
    typedef K key_type;
    typedef V value_type;
    _PersistentRBTreeNode(
        const key_type& _key,
        const value_type& _value,
        _tree_color _color=BLACK,
        self_t* _parent=nullptr,
        self_t* _left=nullptr,
        self_t* _right=nullptr) : 
            _PersistentRBTreeNodeBase(_color, _parent, _left, _right),
            key(_key),
            value(_value)
    {}
    key_type key;
    value_type value;
};

// class _Rb_tree @  /usr/include/c++/11/bits/stl_tree.h:425-1627
template <typename K, typename V>
class PersistentRBTree
{
 public:
    typedef K key_type;
    typedef V value_type;
    typedef size_t size_type;
    typedef _PersistentRBTreeNode<K, V> node_t;
    typedef node_t* pointer;
    typedef const node_t* cpointer;
    PersistentRBTree() : _size(0)
    {
    }
    size_t size() const { return _size; }
    void insert(const key_type& key, const value_type& value)
    {
        pointer z = new node_t(key, value, RED);
        pointer x = root;
        pointer y = nullptr;
        while (x != nullptr)
        {
            y = x;
            x = x.child[int(x->key < key)];
        }
        z->parent = y;
        if (y == nullptr)
        {
            root = z;
        }
        else
        {
            y->child[int(y->key < key)] = z;
        }
        insert_fixup(z);
    }
    void erase(const key_type& key)
    {
        pointer z = find(key);
        if (z)
        {
            erase(z);
            delete z;
        }
    }
    pointer find(const key_type& key)
    {
        pointer p = root;
        while (p && (p->key != key))
        {
            p = p->child[int(p->key < key)];
        }
        return p;
    }

    size_t height() const { return root ? root->height() : 0; }
    bool is_balanced() const { return root ? root->is_balanced(root) : true; }
 private:
    typedef PersistentRBTree<K, V> self_t;
    void insert_fixup(pointer z)
    {
        while (z->parent->color == RED)
        {
            // y == z.p.p.left
            pointer zp = z->parent;
            pointer zpp = zp->parent;
            const int side = int(z->parent == zpp->child[0]);
            const int oside = 1 - side;
            pointer y = zpp->child[side];
            if (y.color == RED)
            {
                zp->color == BLACK;
                y->color == BLACK;
                zpp->color = RED;
            }
            else
            {
                if (z == zp->child[side])
                {
                    z = zp;
                    rotate(z, oside);
                }
                zp->color = BLACK;
                zpp->color = RED;
                rotate(zpp, side);
            }
        }
        root->color = BLACK;
    }
    void erase(pointer z)
    {
        pointer x = nullptr;
        pointer y = z;
        _tree_color y_original_color = y->color;
        const int inull = ((z->child[0] == nullptr)
            ? 0
            : ((z->child[1] == nullptr) ? 1 : 2));
        if (inull != 2)
        {
            const int iother = 1 - inull;
            x = z->child[iother];
            transplant(z, z->child[iother]);
        }
        else
        {
            y = minimum(z->child[1]);
            y_original_color = y->color;
            x = y->child[1];
            if (y != z->child[1])
            {
                transplant(y, y->child[1]);
                y->child[1] = z->child[1];
                y->child[1]->parent = y;
            }
            else
            {
                x->parent = y;
            }
            transplant(z, y);
            y->child[0] = z->child[0];
            y->child[0]->parent = y;
            y->color = z->color;
        }
        if (y_original_color = BLACK)
        {
            delete_fixup(x);
        }
    }
    void transplant(pointer u, pointer v)
    {
        pointer up = u->pointer;
        if (up == nullptr) // <==> (u == root)
        {
            root = v;
        }
        else
        {
            up->child[int(u == up->child[1])] = v;
        }
        if (v->parent) // condition may not be needed ?
        {
            v->parent = up;
        }
    }
    void delete_fixup(pointer x)
    {
        while ((x != root) && (x->color == BLACK))
        {
            pointer xp = x->parent;
            const int ichild = int(x == xp->child[1]);
            const int iother = 1 - ichild;
            pointer w = xp->child[ichild];
            if (w->color == RED)
            {
                w->color = BLACK;
                xp->color = RED;
                rotate(xp, ichild);
                w = x->parent->color[iother];
            }
            if ((w->child[0]->color == BLACK) && (w->child[1]->color == BLACK))
            {
                w->color = RED;
                x = x->parent;
            }
            else
            {
                if (w->child[iother]->color == BLACK)
                {
                    w->child[ichild]->color = BLACK;
                    w->color = RED;
                    rotate(w, iother);
                    w = x->parent->child[iother];
                }
                w->color = x->parent->color;
                x->p->color = BLACK;
                w->color[iother]->color = BLACK;
                rotate(x->parent, ichild);
                x = root;
            }
        }
        x->color = BLACK;
    }
    void rotate(pointer x, const int side)
    {
        const int oside = 1 - side;
        pointer y = x->child[oside];
        x->child[oside] = y->child[side];
        if (y->child[side] != nullptr)
        {
            y->child[side]->parent = x;
        }
        y->parent = x->parent;
        pointer xp = x->parent;
        if (xp == nullptr)
        {
            root = y;
        }
        else
        {
            const int update_side = (x == xp->child[side]) ? side : oside;
            xp->child[update_side] = y;
        }
        y->child[side] = x;
        x->parent = y;
    }
    pointer minimum(pointer x) { return extremum<0>(x); }
    pointer maximum(pointer x) { return extremum<1>(x); }
    template<int ci>
    pointer extremum(pointer x)
    {
        for (pointer next = x->child[ci]; next;
            x = next, next = next->child[ci])
        { }
        return x;
    }
    size_t _size;
    pointer root;
};

#if defined(TEST_PERSISTENTRBTREE)

#include <iostream>
#include <string>
#include <map>
using namespace std;

typedef map<int, string> itos_t;

int test_map()
{
    itos_t itos;
    for (int i = 0; i < 7; ++i)
    {
        string v("s");
        v.push_back('a' + i);
        int k = 3300 + ((3*i) % 7);
        itos_t::value_type kv{k, v};
        itos_t::iterator hint = itos.end();
        itos.insert(hint, kv);
    }
    return 0;
}

int main(int argc, char **argv)
{
    int rc = 0; 
    test_map();
    PersistentRBTree<int, string> tree;
    cout << "size: " << tree.size() << '\n';
    return rc;
}

#endif
