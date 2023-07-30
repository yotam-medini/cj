#include <algorithm>
#include <initializer_list>
#include <iostream>
#include <string>

enum _tree_color { RED = false, BLACK = true };

class _PersistentRBTreeNodeBase
{
 public:
    _PersistentRBTreeNodeBase(_tree_color _color=BLACK) : color(_color) {}
    virtual ~_PersistentRBTreeNodeBase() {}
    typedef const _PersistentRBTreeNodeBase* cbpointer;
    _tree_color color; // red=false, black=true

    size_t height() const
    { 
        const size_t h0 = bchild(0)->height();
        const size_t h1 = bchild(1)->height();
        size_t h =  1 + std::max(h0, h1);
        return h;
    }
    bool is_balanced() const
    {
        return is_balanced(unused_height);
    }
    bool is_red_black() const
    {
        return is_mono_black_count(unused_black_count);
    }
    virtual cbpointer bparent() const = 0;
    virtual cbpointer bchild(int ci) const = 0;
 private:
    inline static size_t unused_height;
    inline static size_t unused_black_count;
    bool is_balanced(size_t& h) const
    {
        bool balanced = true;
        size_t hc[2]{0, 0};
        for (int i: {0, 1})
        {
            cbpointer c = bchild(i);
            balanced = balanced && (c ? c->is_balanced(hc[i]) : true);
        }
        if (balanced)
        {
            if (hc[0] > hc[1]) { std::swap(hc[0], hc[1]); }
            balanced = (hc[1] <= 2*hc[0]);
            h = hc[1] + 1;
        }
        return balanced;
    }
    bool is_mono_black_count(size_t& black_count) const
    {
        bool mono = true;
        size_t child_black_count[2]{0, 0};
        for (int ci = 0; mono && (ci < 2); ++ci)
        {
            cbpointer cbp = bchild(ci);
            mono = (!cbp) || cbp->is_mono_black_count(child_black_count[ci]);
        }
        if (mono)
        {
            mono = (child_black_count[0] == child_black_count[1]);
            if (mono)
            {
                black_count = child_black_count[0] + int(color == BLACK);
            }
        }
        return mono;
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
            _PersistentRBTreeNodeBase(_color),
            key{_key},
            value{_value},
            parent{_parent}, 
            child{_left, _right}
    {}
    key_type key;
    value_type value;
    cbpointer bparent() const { return parent; }
    cbpointer bchild(int ci) const { return child[ci]; }
    self_t *parent;
    self_t *child[2]; // left, right
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
    PersistentRBTree() :
        sentinerl(key_type(), value_type(), BLACK, nullptr, nullptr, nullptr),
        nil(&sentinerl) ,
        _size{0},
        root{nil}
    {
    }
    size_t size() const { return _size; }
    void insert(const key_type& key, const value_type& value)
    {
        pointer z = new node_t(key, value, RED, nil, nil, nil);
        pointer x = root;
        pointer y = nil;
        while (x != nil)
        {
            y = x;
            x = x->child[int(x->key < key)];
        }
        z->parent = y;
        if (y == nil)
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
    cpointer find(const key_type& key) const
    {
        cpointer cp = root;
        while ((cp != nil) && (cp->key != key))
        {
            cp = cp->child[int(cp->key < key)];
        }
        return (cp == nil ? nullptr : cp);
    }
    pointer find(const key_type& key)
    {
        pointer p = root;
        while ((p != nil) && (p->key != key))
        {
            p = p->child[int(p->key < key)];
        }
        return (p == nil ? nullptr : p);
    }

    size_t height() const { return root != nil ? root->height() : 0; }
    bool is_balanced() const
    {
        return root != nil ? root->is_balanced(root) : true;
    }
    bool is_red_black() const
    { 
        return root == nil ? true : root->is_balanced() && root->is_red_black();
    }
    void print(std::ostream& os=std::cerr) const
    {
        print(os, root, 0);
    }
 private:
    typedef PersistentRBTree<K, V> self_t;
    void insert_fixup(pointer z)
    {
        while (z->parent->color == RED)
        {
            // y == z.p.p.left
            const int side = int(z->parent == z->parent->parent->child[1]);
            const int oside = 1 - side;
            pointer y = z->parent->parent->child[oside];
            if (y->color == RED)
            {
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
            }
            else
            {
                if (z == z->parent->child[oside])
                {
                    z = z->parent;
                    rotate(z, side);
                }
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                rotate(z->parent->parent, oside);
            }
        }
        root->color = BLACK;
    }
    void erase(pointer z)
    {
        pointer x = nullptr;
        pointer y = z;
        _tree_color y_original_color = y->color;
        const int inull = ((z->child[0] == nil)
            ? 0
            : ((z->child[1] == nil) ? 1 : 2));
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
        if (y_original_color == BLACK)
        {
            delete_fixup(x);
        }
    }
    void transplant(pointer u, pointer v)
    {
        pointer up = u->parent;
        if (up == nil) // <==> (u == root)
        {
            root = v;
        }
        else
        {
            up->child[int(u == up->child[1])] = v;
        }
        v->parent = up;
    }
    void delete_fixup(pointer x)
    {
        while ((x != root) && (x->color == BLACK))
        {
            pointer xp = x->parent;
            const int ichild = int(x == xp->child[1]);
            const int iother = 1 - ichild;
            pointer w = xp->child[iother];
            if (w->color == RED)
            {
                w->color = BLACK;
                xp->color = RED;
                rotate(xp, ichild);
                w = x->parent->child[iother];
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
                x->parent->color = BLACK;
                w->child[iother]->color = BLACK;
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
        if (y->child[side] != nil)
        {
            y->child[side]->parent = x;
        }
        y->parent = x->parent;
        pointer xp = x->parent;
        if (xp == nil)
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
        for (pointer next = x->child[ci]; (next != nil);
            x = next, next = next->child[ci])
        { }
        return x;
    }
    void print(std::ostream& os, cpointer x, size_t depth) const
    {
        if (x != nil)
        {
            print(os, x->child[0], depth + 1);
            os << std::string(depth, ' ') <<
                "RB"[int(x->color)] <<
                ", key=" << x->key << ", v=" << x->value << '\n';
            print(os, x->child[1], depth + 1);
        }
    }

    node_t sentinerl; // const
    pointer nil;
    size_t _size;
    pointer root;
};

#if defined(TEST_PERSISTENTRBTREE)

// #include <iostream>
// #include <string>
#include <map>
#include <vector>
using namespace std;

typedef map<int, string> itos_t;
using vi_t = vector<int>;
using i2i_t = map<int, int>;
using prb_i2i_t = PersistentRBTree<int, int>;

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

int test_queries(const i2i_t& ci2i, const prb_i2i_t& cprb_i2i, int sz)
{
    int rc = 0;
    for (int qk = -1; (rc == 0) && (qk <= sz); ++qk)
    {
        i2i_t::const_iterator citer = ci2i.find(qk);
        prb_i2i_t::cpointer cp = cprb_i2i.find(qk);
        if ((citer == ci2i.end()) != (cp == nullptr))
        {
            cerr << "find-found inconsistent, qk=" << qk << '\n';
            rc = 1;
        }
        else if (cp && (cp->value != citer->second))
        {
            cerr << "found values inconsistent, qk=" << qk << 
                ", RB: " << cp->value << ", map: " << citer->second << '\n';
            rc = 1;
        }
    }
    return rc;
}

int test_permutation(const vi_t& perm, const vi_t& del_perm)
{
    int rc = 0;
    const int sz = perm.size();
    i2i_t i2i;
    prb_i2i_t prb_i2i;
    for (int pi = 0; (rc == 0) && (pi < sz); ++pi)
    {
        const int k = perm[pi], v = k*k;
        i2i.insert(i2i.end(), i2i_t::value_type{k, v});
prb_i2i.print();
        prb_i2i.insert(k, v);
        if (prb_i2i.is_red_black())
        {
            rc = test_queries(i2i, prb_i2i, sz);
        }
        else
        {
            cerr << "Failed (after insert) is_red_black test\n";
prb_i2i.print();
            rc = 1;
        }
    }
    for (int pi = 0; (rc == 0) && (pi < sz); ++pi)
    {
cerr << "pi=" << pi << '\n'; prb_i2i.print();
        const int k = del_perm[pi];
        i2i.erase(k);
        prb_i2i.erase(k);
        if (prb_i2i.is_red_black())
        {
            test_queries(i2i, prb_i2i, sz);
        }
        else
        {
            cerr << "Failed (after erase) is_red_black test\n";
        }
    }
    return rc;
}

int test_permutate(int argc, char **argv)
{
    int rc = 0;
    int ai = 0;
    size_t perm_min = strtoul(argv[ai++], nullptr, 0);
    size_t perm_max = strtoul(argv[ai++], nullptr, 0);
    for (size_t perm_size = perm_min; (rc == 0) && (perm_size <= perm_max);
        ++perm_size)
    {
        vi_t perm;
        while (perm.size() < perm_size) { perm.push_back(perm.size()); }
        const vi_t cperm(perm);
        for (bool more = true; (rc == 0) && more;
            more = next_permutation(perm.begin(), perm.end()))
        {
            vi_t del_perm(cperm);
            for (bool dmore = true; (rc == 0) && dmore;
                dmore = next_permutation(del_perm.begin(), del_perm.end()))
            {
                rc = test_permutation(perm, del_perm);
            }
        }
    }
    return rc;
}

int test_specific(int argc, char **argv)
{
    const size_t sz = argc / 2;
    vi_t perm, del_perm;
    int ai = 0;
    for ( ; perm.size() < sz; ++ai)
    {
        perm.push_back(strtoul(argv[ai], nullptr, 0));
    }
    for ( ; del_perm.size() < sz; ++ai)
    {
        del_perm.push_back(strtoul(argv[ai], nullptr, 0));
    }
    int rc = test_permutation(perm, del_perm);
    return rc;
}

int main(int argc, char **argv)
{
    int rc = 0; 
    test_map();
    const string cmd{1 < argc ? argv[1] : ""};
    if (cmd == string("permutate"))
    {
        rc = test_permutate(argc - 2, argv + 2);
    }
    else if (cmd == string("specific"))
    {
        rc = test_specific(argc - 2, argv + 2);
    }
    else
    {
        cerr << "unsupported cmd: " << cmd << '\n';
        rc = 1;
    }
    return rc;
}

#endif
