// #include <iosfwd>
#include <iostream>
typedef unsigned u_t;

template<typename T> class SkipListNode; // fwd decl
template <typename T>
class SkipListLink
{
 public:
    SkipListLink<T>() : next(0), width(0) {}
    typedef SkipListNode<T>* nodep_t;
    nodep_t next;
    u_t width;    
};

template<typename T>
class SkipListNode
{
 public:
    typedef T data_t;
    SkipListNode<T>(const data_t& d) : data(d) {}
    enum {LEVEL_MAX = 32};
    data_t data;
    SkipListLink<T> link[LEVEL_MAX];
};

template<typename T>
class SkipList
{
 public:
    typedef T data_t;
    SkipList<T>() : sz(0), levels(0) {}
    u_t size() const { return sz; }
    void insert(const data_t& d);
    void remove(const data_t& d);
    data_t* get_pos(u_t i) const;
    void debug_print(std::ostream&) const;
 private:
    typedef SkipListLink<T> link_t;
    typedef SkipListNode<T> node_t;
    typedef node_t* nodep_t;
    enum {LEVEL_MAX = node_t::LEVEL_MAX};
    link_t head[LEVEL_MAX];
    // nodep_t head;
    u_t sz;
    u_t levels;
};

template <typename T>
void SkipList<T>::insert(const data_t& d)
{
    ++sz;
    u_t n_skips = 0;
    link_t* update[LEVEL_MAX] = {0, };
    link_t* pp = &head[levels > 0 ? levels - 1 : 0];
    for (u_t level = levels; level > 0; )
    {
        --level;
        for (; pp->next && (pp->next->data < d);
            pp = &pp->next->link[level], ++n_skips)
        {
        }
        update[level] = pp;
    }
    u_t new_level = 0;
    while (n_skips && new_level + 1 < LEVEL_MAX)
    {
        new_level += (n_skips % 2 == 0 ? 1 : 0);
        n_skips /= 2;
    }
    nodep_t node = new node_t(d);
    for (; levels < new_level + 1; ++levels)
    {
        update[levels] = &head[levels];
    }
    for (u_t level = 0; level <= new_level; ++level)
    {
        nodep_t next = update[level] ? update[level]->next : 0;
        node->link[level].next = next;
        update[level]->next = node;
    }
}

template <typename T>
void SkipList<T>::remove(const data_t& d)
{
    u_t n_skips = 0;
    link_t* update[LEVEL_MAX] = {0, };
    link_t* pp = &head[levels > 0 ? levels - 1 : 0];
    for (u_t level = levels; level > 0; )
    {
        --level;
        for (; pp->next && (pp->next->data < d);
            pp = &pp->next->link[level], ++n_skips)
        {
        }
        update[level] = pp;
    }
    if (pp->next && (pp->next->data == d))
    {
        --sz;
        for (u_t level = 0;
            (level < levels) && (update[level]->next == pp->next); ++level)
        {
            update[level]->next = pp->next->link[level].next;
        }
        delete pp->next;
    }
}

template <typename T>
void SkipList<T>::debug_print(std::ostream& os) const
{
    os << "{ SkipList\n";
    for (u_t level = levels; level > 0; )
    {
        --level;
        os << "  [Level " << level << "]: {\n  ";
        u_t counter = 0;
        for (const node_t* p = head[level].next; p && (counter < 0x40); 
            p = p->link[level].next, ++counter)
        {
            os << "  " << p->data;
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
typedef SkipList<u_t> skplu_t;
typedef list<u_t> lu_t;
class Op
{
 public:
    Op(char _cmd, u_t _v) : cmd(_cmd), v(_v) {}
    char cmd;
    u_t v;
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
            skplu.insert(op.v);
            {
                lu_t::iterator iter = listu.begin();
                for (; (iter != listu.end()) && (op.v < *iter); ++iter) {}
                listu.insert(iter, op.v);
            }
            break;
         case 'r':
            skplu.remove(op.v);
            {
                lu_t::iterator iter = listu.begin();
                for (; (iter != listu.end()) && (op.v != *iter); ++iter) {}
                if (iter != listu.end())
                {
                    listu.erase(iter);
                }
            }
            break;
         case 'g':
            {
                lu_t::iterator iter = listu.begin();
                for (u_t steps = 0; (iter != listu.end()) && (steps < op.v);
                     ++iter, ++steps) {}
                u_t* plst = (iter == listu.end() ? 0 : &(*iter));
                u_t* pskp = 0;
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
            cerr << ' ' << op.cmd << ' ' << op.v;
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
        for (int ai = 2; ai < argc; ai += 2)
        {
            Op op(argv[ai][0], stoi(argv[ai + 1]));
            ops.push_back(op);
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
