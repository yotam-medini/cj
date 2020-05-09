#include <iostream>
typedef unsigned u_t;

template<typename T> class SkipListListNode; // fwd decl
template <typename T>
class SkipListListLink
{
 public:
    SkipListListLink<T>() : next(0) {}
    typedef SkipListListNode<T>* nodep_t;
    nodep_t next;
    u_t length;
};

template<typename T>
class SkipListListLinks
{
 public:
    enum {LEVEL_MAX = 32};
    SkipListListLink<T> link[LEVEL_MAX];
    typedef SkipListListNode<T> node_t;
    // const node_t* next(u_t level) const { return link[level].next; }
    // node_t* next(u_t level) { return link[level].next; }
};

template<typename T>
class SkipListListNode
{
 public:
    typedef T data_t;
    SkipListListNode<T>(const data_t& d) : data(d) {}
    data_t data;
    SkipListListLinks<T> links;
};

template<typename T>
class SkipListList
{
 public:
    typedef T data_t;
    SkipListList<T>() : sz(0), levels(0) {}
    u_t size() const { return sz; }
    void insert(u_t i, const data_t& d);
    void remove(u_t i);
    const data_t* get(u_t i) const;
    void debug_print(std::ostream&) const;
 private:
    typedef SkipListListLink<T> link_t;
    typedef SkipListListLinks<T> links_t;
    typedef SkipListListNode<T> node_t;
    typedef node_t* nodep_t;
    enum {LEVEL_MAX = links_t::LEVEL_MAX};
    links_t head;
    u_t sz;
    u_t levels;
};

template <typename T>
void SkipListList<T>::insert(u_t i, const data_t& d)
{
    ++sz;
    u_t new_level = 0, seed = i;
    while (seed && new_level + 1 < LEVEL_MAX)
    {
        new_level += (seed % 2 == 0 ? 1 : 0);
        seed /= 2;
    }
    if (levels < new_level + 1)
    {
        levels = new_level + 1;
    }
    nodep_t node = new node_t(d);
    int j = 0;
    links_t* p = &head;
    for (u_t level = levels; level > 0; )
    {
        --level;
        for (; p->link[level].next && (j + p->link[level].length < i + 1);
            p = &p->link[level].next->links)
        {
            j += p->link[level].length;
        }
        link_t& pll = p->link[level];
        ++pll.length;
        if (level <= new_level)
        {
            node->links.link[level].next = pll.next;
            pll.next = node;
            u_t delta = i - j + 1;
            node->links.link[level].length = pll.length - delta;
            pll.length = delta;
        }
    }
}

template <typename T>
void SkipListList<T>::remove(u_t i)
{
    u_t j = 0;
    links_t* p = &head;
    nodep_t x = 0;
    for (u_t level = levels; level > 0; )
    {
        --level;
        for (; p->link[level].next && (j + p->link[level].length < i + 1);
            p = &p->link[level].next->links)
        {
            j += p->link[level].length;
        }
        link_t& pll = p->link[level];
        --(pll.length);
        if (pll.next && (j + pll.length == i))
        {
            x = pll.next; // needed only for level=0
            pll.length += pll.next->links.link[level].length;
            pll.length += pll.length;
            pll.next = pll.next->links.link[level].next;
        }
    }
    delete x;
    for (; (levels > 0) && (head.link[levels - 1].next == 0); --levels) {}
}

template <typename T>
const typename SkipListList<T>::data_t* SkipListList<T>::get(u_t i) const
{
    u_t j = 0;
    const links_t* p = &head;
    for (u_t level = levels; level > 0; )
    {
        --level;
        for (; p->link[level].next && (j + p->link[level].length < i + 1);
            p = &p->link[level].next->links)
        {
            j += p->link[level].length;
        }
    }
    nodep_t x = p->link[0].next;
    data_t* pd = &x->data;
    return pd;
}

template <typename T>
void SkipListList<T>::debug_print(std::ostream& os) const
{
    os << "{ SkipListList\n";
    for (u_t level = levels; level > 0; )
    {
        --level;
        os << "  [Level " << level << "]: {\n  ";
        u_t counter = 0;
        for (const link_t* p = &head.link[level]; p->next && (counter < 0x40);
            p = &p->next->links.link[level], ++counter)
        {
            os << "  [L=" << p->length << "] " << p->next->data;
        }
        os << (counter == 0x40 ? " ...\n" : "\n") << "  }\n";
    }
    os << "}\n";
}

#include <iostream>
#include <list>
#include <vector>
#include <string>
using namespace std;
typedef SkipListList<u_t> skplu_t;
typedef list<u_t> lu_t;
class Op
{
 public:
    Op(char _cmd, u_t _v0, u_t _v1=0) : cmd(_cmd), v0(_v0), v1(_v1) {}
    char cmd;
    u_t v0;
    u_t v1;
};
typedef vector<Op> vop_t;

bool test(const vop_t& ops)
{
    skplu_t skplu;
    lu_t listu;
    bool ok = true;
    for (u_t i = 0; (i < ops.size()) && ok; ++i)
    {
        const Op& op = ops[i];
        switch (op.cmd)
        {
         case 'i':
            skplu.insert(op.v0, op.v1);
            {
                lu_t::iterator iter = listu.begin();
                for (; (iter != listu.end()) && (op.v0 < *iter); ++iter) {}
                listu.insert(iter, op.v1);
            }
            break;
         case 'r':
            skplu.remove(op.v0);
            {
                lu_t::iterator iter = listu.begin();
                for (; (iter != listu.end()) && (op.v0 != *iter); ++iter) {}
                if (iter != listu.end())
                {
                    listu.erase(iter);
                }
            }
            break;
         case 'g':
            {
                const u_t* pskp = skplu.get(op.v0);
                lu_t::const_iterator iter = listu.begin();
                for (u_t steps = 0; (iter != listu.end()) && (steps < op.v0);
                     ++iter, ++steps) {}
                const u_t* plst = (iter == listu.end() ? 0 : &(*iter));
                ok = (plst == 0) == (pskp == 0);
                if (ok && plst)
                {
                    ok = *plst == *pskp;
                }
            }
            break;
          default:
             cerr << "Unsupported command: " << op.cmd << '\n';
             ok = false;
        }
        skplu.debug_print(cerr);
        if (ok)
        {
            u_t sz_skp = skplu.size();
            u_t sz_lst = listu.size();
            ok = sz_skp == sz_lst;
            if (!ok)
            {
                cerr << "#(skplu)="<<sz_skp << " != #(listu)="<<sz_lst << '\n';
            }
        }
    }
    if (!ok)
    {
        cerr << "Failed with\n  special";
        for (const Op& op: ops)
        {
            cerr << ' ' << op.cmd << ' ' << op.v0;
            if (op.cmd == 'i')
            {
                cerr << ' ' << op.v1;
            }
        }
        cerr << '\n';
    }
    return ok;
}

int main(int argc, char** argv)
{
    bool ok = false;
    if (string(argv[1]) == "special")
    {
        vop_t ops;
        for (int ai = 2; ai < argc; )
        {
            char cmd = argv[ai][0];
            u_t v1 = (cmd == 'i' ? stoi(argv[ai + 2]) : 0);
            Op op(cmd, stoi(argv[ai + 1]), v1);
            ops.push_back(op);
            ai += (cmd == 'i' ? 3 : 2);
        }
        ok = test(ops);
    }
    else
    {
        // random // combs
        ok = false;
    }
    int rc = ok ? 0 : 1;
    return rc;
}
