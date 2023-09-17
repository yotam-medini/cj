#include <algorithm>
#include <initializer_list>
#include <iostream>
#include <memory>
#include <string>
#include <utility>

enum _tree_color { RED = false, BLACK = true };

static unsigned long debug_flags = 0;

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

class _Child
{
 public:
    _Child(
        _PersistentRBTreeNodeBase *_p=nullptr,
        int _side = 0) :
        p(_p), side(_side)
        {}
    _PersistentRBTreeNodeBase *p;
    int side; // 0, 1
};

class PersistentRBTreePath
{
 public:
    PersistentRBTreePath(_PersistentRBTreeNodeBase *_root=nullptr) :
        root(_root) {}
    _PersistentRBTreeNodeBase *root;
    std::vector<_Child> path;
};

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
            pkv{new std::pair{_key, _value}},
            parent{_parent}, 
            child{_left, _right}
    {}
    void delete_non_nil(self_t* nil)
    {
        for (int ci: {0, 1})
        {
            if (child[ci] != nil)
            {
                child[ci]->delete_non_nil(nil);
                delete child[ci];
            }
        }
    }
    std::shared_ptr<std::pair<K, V>> pkv;
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
    ~PersistentRBTree()
    {
        if (root != nil)
        {
            root->delete_non_nil(nil);
            delete root;
        }
    }
    size_t size() const { return _size; }
    void insert(const key_type& key, const value_type& value)
    {
        std::vector<pointer> path; path.push_back(nil);
        pointer z = new node_t(key, value, RED, nil, nil, nil);
        pointer x = root; // roots.empty() ? nil : roots.back();
        pointer y = nil;
        while (x != nil)
        {
            y = x;
            int side = int(x->pkv->first < key);
            path.push_back(x);
            x = x->child[side];
        }
        z->parent = y;
        if (y == nil)
        {
            root = z;
        }
        else
        {
            int side = int(y->pkv->first < key);
            y->child[side] = z;
        }
        path.push_back(z);
        insert_fixup(path);
        ++_size;
    }
    void erase(const key_type& key)
    {
        std::vector<pointer> path; path.push_back(nil);
        pointer p = root;
        while ((p != nil) && (p->pkv->first != key))
        {
            path.push_back(p);
            p = p->child[int(p->pkv->first < key)];
        }
        if (p != nil)
        {
            path.push_back(p);
            erase(path);
            delete p;
            --_size;
        }
    }
    cpointer find(const key_type& key) const
    {
        cpointer cp = root;
        while ((cp != nil) && (cp->pkv->first != key))
        {
            cp = cp->child[int(cp->pkv->first < key)];
        }
        return (cp == nil ? nullptr : cp);
    }
    pointer find(const key_type& key)
    {
        pointer p = root;
        while ((p != nil) && (p->pkv->first != key))
        {
            p = p->child[int(p->pkv->first < key)];
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
    void insert_fixup(std::vector<pointer>& path)
    {
        size_t pi = path.size() - 1;
        while (path[pi - 1]->color == RED)
        {
            pointer z = path[pi];
            pointer zp = path[pi - 1];
            pointer zpp = path[pi - 2];
            // y == z.p.p.left
            const int side = int(zp == zpp->child[1]);
            const int oside = 1 - side;
            pointer y = zpp->child[oside];
            if (y->color == RED)
            {
                zp->color = BLACK;
                y->color = BLACK;
                zpp->color = RED;
                pi -= 2;
            }
            else
            {
                if (z == zp->child[oside])
                {
                    z = zp;
                    zp = zpp;
                    rotate(&zp, zp, z, side);
                    std::swap(path[pi - 1], path[pi]);
                    z = path[pi];
                    zp = path[pi - 1];
                }
                zp->color = BLACK;
                zpp->color = RED;
                pointer parent = (pi >= 3) ? path[pi - 3] : nil;
                pointer *p_parent = (pi >= 3) ? &path[pi - 3] : nullptr;
                rotate(p_parent, parent, zpp, oside);
                path[pi - 2] = path[pi - 1];
                path[pi - 1] = path[pi];
                --pi;
                // path[pi] = path[pi + 1]; // buggy
            }
        }
        root->color = BLACK;
    }
    void erase(std::vector<pointer>& path)
    {
        size_t pi = path.size();
        pointer z = path[pi - 1];
        pointer zp = path[pi - 2];
        pointer x = nullptr;
        pointer y = z;
        _tree_color y_original_color = y->color;
        const int inull = ((z->child[0] == nil)
            ? 0
            : ((z->child[1] == nil) ? 1 : 2));
        if (inull != 2) // at least one nil child
        {
            const int iother = 1 - inull;
            x = z->child[iother];
            transplant(zp, z, x);
            path.back() = x;
        }
        else // 2 non-nil children
        {
            // path.pop_back();   
            std::vector<pointer> unused_path;
            y = minimum(z->child[1], path);
            y_original_color = y->color;
            x = y->child[1];
            if (y != z->child[1])
            {
                transplant(path.back(), y, y->child[1]);
                y->child[1] = z->child[1];
                y->child[1]->parent = y;
            }
            else
            {
                // path.back() = y;
                x->parent = y;
            }
            path[pi - 1] = y; // where z was
            path.push_back(x);
            transplant(zp, z, y);
            y->child[0] = z->child[0];
            y->child[0]->parent = y;
            y->color = z->color;
        }
        if (y_original_color == BLACK)
        {
            delete_fixup(path);
        }
    }
    void transplant(pointer uparent, pointer u, pointer v)
    {
        if (uparent == nil) // <==> (u == root)
        {
            root = v;
        }
        else
        {
            uparent->child[int(u == uparent->child[1])] = v;
        }
        v->parent = uparent;
    }
    void delete_fixup(std::vector<pointer>& path)
    {
        pointer x = path.back();
        for (size_t pi = path.size() - 1;
            ((x  != root) && (x->color == BLACK)); --pi)
        {
            pointer xp = path[pi - 1];
            pointer parent = (pi >= 2) ? path[pi - 2] : nil;
            if (xp != x->parent) {
                std::cerr << __FILE__ << ':' << __LINE__ << " bug\n";
            }
            const int ichild = int(x == xp->child[1]);
            const int iother = 1 - ichild;
            pointer w = xp->child[iother];
            if (w->color == RED)
            {
                w->color = BLACK;
                xp->color = RED;
                rotate(&parent, parent, xp, ichild);
                path[pi - 1] = parent;
                w = xp->child[iother];
            }
            if ((w->child[0]->color == BLACK) && (w->child[1]->color == BLACK))
            {
                w->color = RED;
                x = xp;
            }
            else
            {
                if (w->child[iother]->color == BLACK)
                {
                    w->child[ichild]->color = BLACK;
                    w->color = RED;
                    if (xp != w->parent) {
                        std::cerr << __LINE__ << ": error\n";
                        exit(1);
                    }
                    rotate(nullptr, xp, w, iother);
                    w = xp->child[iother];
                }
                w->color = xp->color;
                xp->color = BLACK;
                w->child[iother]->color = BLACK;
                rotate(nullptr, parent, xp, ichild);
                x = root;
            }
        }
        x->color = BLACK;
    }
    void rotate(
        pointer *p_new_parent,
        pointer curr_parent,
        pointer x,
        const int side)
    {
        const int oside = 1 - side;
        pointer y = x->child[oside];
        x->child[oside] = y->child[side];
        if (y->child[side] != nil)
        {
            y->child[side]->parent = x;
        }
        y->parent = curr_parent;
        if (curr_parent == nil)
        {
            root = y;
        }
        else
        {
            const int update_side =
                (x == curr_parent->child[side]) ? side : oside;
            curr_parent->child[update_side] = y;
        }
        y->child[side] = x;
        x->parent = y;
        if (p_new_parent)
        {
            *p_new_parent = y;
        }
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
    pointer minimum(pointer x, std::vector<pointer>& path)
    { 
        return extremum<0>(x, path);
    }
    pointer maximum(pointer x, std::vector<pointer>& path)
    {
        return extremum<1>(x, path);
    }
    template<int ci>
    pointer extremum(pointer x, std::vector<pointer>& path)
    {
        for (pointer next = x->child[ci]; (next != nil);
            path.push_back(x), x = next, next = next->child[ci])
        { }
        return x;
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
                ", key=" << x->pkv->first << ", v=" << x->pkv->second << '\n';
            print(os, x->child[1], depth + 1);
        }
    }
    void dbgln_print_with_path(int ln, const std::vector<pointer>& path) const {
        if (debug_flags & 0x1) {
           std::cerr << ln << ":\n"; print_with_path(path); std::cerr << '\n';
        }
    }
    void print_with_path(const std::vector<pointer>& path) const {
        for (size_t i = 0; i < path.size(); ++i) {
            pointer p = path[i];
            std::cerr << "path[" << i << "]= " << "RB"[int(p->color)] << ' ' <<
                p->pkv->first << '\n'; }
        print();
    }

    node_t sentinerl; // const
    pointer nil;
    size_t _size;
    pointer root;
    std::vector<pointer> roots;
    std::vector<std::unique_ptr<pointer>> nodes_;
    std::vector<std::unique_ptr<std::pair<key_type, value_type>>> kvs_;
};

#if defined(TEST_PERSISTENTRBTREE)

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
        else if (cp && (cp->pkv->second != citer->second))
        {
            cerr << "found values inconsistent, qk=" << qk << 
                ", RB: " << cp->pkv->second << ", map: " << citer->second <<'\n';
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
        prb_i2i.insert(k, v);
        if (i2i.size() != prb_i2i.size())
        {
            cerr << "Not same size\n";
            rc = 1;
        }
        else if (prb_i2i.is_red_black())
        {
            rc = test_queries(i2i, prb_i2i, sz);
        }
        else
        {
            cerr << "Failed (after insert) is_red_black test\n";
            prb_i2i.print();
            rc = 1;
        }
        if (rc != 0)
        {
            cerr << "commands";
            for (int i = 0; i <= pi; ++i) { cerr << " i " << perm[i]; }
            cerr << '\n';
        }
    }
    for (int pi = 0; (rc == 0) && (pi < sz); ++pi)
    {
        const int k = del_perm[pi];
        i2i.erase(k);
        prb_i2i.erase(k);
        if (i2i.size() != prb_i2i.size())
        {
            cerr << "Not same size\n";
            rc = 1;
        }
        else if (prb_i2i.is_red_black())
        {
            rc = test_queries(i2i, prb_i2i, sz);
        }
        else
        {
            cerr << "Failed (after erase) is_red_black test\n";
            rc = 1;
        }
        if (rc != 0)
        {
            prb_i2i.print();
            cerr << "commands";
            for (int i: perm) { cerr << " i " << i; }
            for (int i = 0; i <= pi; ++i) { cerr << " e " << del_perm[i]; }
            cerr << '\n';
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
    unsigned long long  n_tests = 0;
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
                if ((n_tests & (n_tests - 1)) == 0) {
                    cerr << __func__ << ", testedd=" << n_tests << '\n';
                }
                ++n_tests;
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

int test_commands(int argc, char **argv)
{
    int rc = 0, n = -1;
    i2i_t i2i;
    prb_i2i_t prb_i2i;
    int n_max = 0;
    for (int ai = 0; (rc == 0) && (ai < argc); ++ai)
    {
        char cmd = argv[ai][0];
        switch (cmd)
        {
         case 'i':
             n = strtoul(argv[++ai], nullptr, 0);
             i2i.insert(i2i.end(), i2i_t::value_type{n, n*n});
             prb_i2i.insert(n, n*n);
             n_max = max(n_max, n);
             break;
         case 'e':
             n = strtoul(argv[++ai], nullptr, 0);
             i2i.erase(n);
             prb_i2i.erase(n);
             break;
         case 'p':
             prb_i2i.print(); cerr << '\n';
             break;
         default:
             rc = 1;
        }
        if ((rc == 0) && !prb_i2i.is_red_black())
        {
            cerr << "is_red_black failed, " << cmd << ' ' << n << '\n';
            rc = 1;
        }
        if (rc == 0)
        {
            rc = test_queries(i2i, prb_i2i, n_max + 1);
        }
        if (rc != 0)
        {
            prb_i2i.print();
        }
    }
    return rc;
}

int main(int argc, char **argv)
{
    int rc = 0, ai = 1, skip = 2;; 
    test_map();
    if ((argc > 3) && string(argv[1]) == string("-debug"))
    {
        debug_flags = strtoul(argv[2], nullptr, 0);
        ai = 3;
        skip = 4;
    }
    const string cmd{1 < argc ? argv[ai] : ""};
    if (cmd == string("permutate"))
    {
        rc = test_permutate(argc - skip, argv + skip);
    }
    else if (cmd == string("specific"))
    {
        rc = test_specific(argc - skip, argv + skip);
    }
    else if (cmd == string("commands"))
    {
        rc = test_commands(argc - skip, argv + skip);
    }
    else
    {
        cerr << "unsupported cmd: " << cmd << '\n';
        rc = 1;
    }
    return rc;
}

#endif
