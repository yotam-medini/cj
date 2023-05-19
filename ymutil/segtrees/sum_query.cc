#include <iostream>
#include <numeric>
#include <vector>
#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;

// See: https://cp-algorithms.com/data_structures/segment_tree.html#sum-queries
//

// n=5
//                 [0:4]
//         [0:2]        [3:4]
//    [0:1]    [2:2]  [3:3] [4:4]
// [0:0] [1:1]

class SegmentTreeNode
{
 public:
    typedef ull_t e_t;
    SegmentTreeNode(e_t _add=0) : add(_add) {}
    e_t add;
};
typedef vector<SegmentTreeNode> vnode_t;

class SegmentTree
{
 public:
    typedef SegmentTreeNode node_t;
    typedef node_t::e_t e_t;
    typedef vector<e_t> ve_t;
    typedef vector<SegmentTreeNode> vnode_t;
    SegmentTree(size_t _sz);
    void update(size_t i, e_t v);
    e_t sum_query(size_t l, size_t r) const;
    void print(ostream& os) const;
 private:
    typedef vector<ve_t> vve_t;
    // void build(size_t ti, size_t al, size_t ar);
    void update_tree(size_t i, e_t v, size_t al, size_t ar, size_t ti);
    e_t query(
        size_t al,
        size_t ar,
        size_t ti,
        size_t segl, 
        size_t segr) const;
    void print_tree(
        ostream& os,
        const string& indent,
        size_t al,
        size_t ar,
        size_t ti) const;
    const size_t sz;
    vnode_t tnodes;
};

SegmentTree::SegmentTree(size_t _sz) :
    sz(_sz),
    tnodes(4*sz, node_t(0))
{
}

void SegmentTree::update(size_t i, e_t v)
{
    update_tree(i, v, 0, sz - 1, 0);
}

void SegmentTree::update_tree(size_t i, e_t v, size_t al, size_t ar, size_t ti)
{
    tnodes[ti].add += v;
    if (al < ar)
    {
        const size_t delta = ar - al;
        size_t midl = al + delta/2;
        size_t midr = midl + 1;
        if (i <= midl)
        {
            update_tree(i, v, al, midl, 2*ti + 1);
        }
        else
        {
            update_tree(i, v, midr, ar, 2*ti + 2);
        }
    }
}

SegmentTree::e_t SegmentTree::sum_query(size_t al, size_t ar) const
{
    e_t r = query(al, ar, 0, 0, sz - 1);
    return r;
}

SegmentTree::e_t SegmentTree::query(
    size_t al,
    size_t ar,
    size_t ti,
    size_t segl, 
    size_t segr) const
{
    // assumin segl <= al <= ar << segr
    e_t sum = 0;
    if ((segl == al) && (ar == segr))
    {
        sum = tnodes[ti].add;
    }
    else
    {
        const size_t delta = segr - segl;  // assuming tk <= tr, delta >= 0
        const size_t midl = segl + delta/2;
        const size_t midr = midl + 1;
        e_t suml = (al <= midl)
            ? query(al, min(ar, midl), 2*ti + 1, segl, midl) : 0;
        e_t sumr = (ar >= midr)
            ? query(max(al, midr), ar, 2*ti + 2, midr, segr) : 0;
        sum = suml + sumr;
    }
    return sum;
}

void SegmentTree::print(ostream& os) const 
{
    print_tree(os, string(""), 0, sz - 1, 0);
}

void SegmentTree::print_tree(
    ostream& os,
    const string& indent,
    size_t al,
    size_t ar,
    size_t ti) const
{
    os << indent << "[" << ti << "] [" << al << ": " << ar << "] = " <<
        tnodes[ti].add << '\n';
    if (al < ar)
    {
        string subindent = indent + string("  ");
        const size_t delta = ar - al;  // assuming tk <= tr, delta >= 0
        const size_t midl = al + delta/2;
        const size_t midr = midl + 1;
        if (al <= midl)
        {
            print_tree(os, subindent, al, midl, 2*ti + 1);
        }
        if (ar >= midr)
        {
            print_tree(os, subindent, midr, ar, 2*ti + 2);
        }
    }
}

class Update
{
 public:
    typedef SegmentTree::e_t e_t;
    Update(size_t _i, e_t _v=0) : i(_i), v(_v) {}
    size_t i;
    e_t v;
};
typedef vector<Update> vupdate_t;

int test_query(
    const vector<SegmentTree::e_t>& a,
    const SegmentTree& st,
    size_t al,
    size_t ar,
    const vupdate_t& updates,
    size_t n_upd
)
{
    typedef SegmentTree::e_t e_t;
    int rc = 0;
    const e_t naive = accumulate(a.begin() + al, a.begin() + ar + 1, e_t(0));
    const e_t mature = st.sum_query(al, ar);
    if (naive != mature)
    {
        rc = 1;
        cerr << "naive="<<naive << ", mature="<<mature << '\n';
        st.print(cerr);
        cerr << "failed: specific " << a.size() << ' ' << al << ' ' << ar;
        for (size_t j = 0; j < n_upd; ++j)
        {
             cerr << ' ' << updates[j].i << ' ' << updates[j].v;
        }
        cerr << '\n';
    }
    return rc;
}

int test_case_all_queries(
    const vector<SegmentTree::e_t>& a,
    const SegmentTree& st,
    const vupdate_t& updates,
    size_t n_upd
)
{
    const size_t n = a.size();
    int rc = 0;
    for (size_t al = 0; (rc == 0) && (al < n); ++al)
    {
        for (size_t ar = al; (rc == 0) && (ar < n); ++ar)
        {
            rc = test_query(a, st, al, ar, updates, n_upd);
        }
    }
    return rc;
}


int test_case(size_t n, size_t al, size_t ar, const vupdate_t& updates)
{
    typedef SegmentTree::e_t e_t;
    int rc = 0;
    vector<e_t> a(n, 0);
    SegmentTree st(n);
    bool single_query = (al < n) && (ar < n);
    for (size_t iupd = 0; (rc == 0) && (iupd < updates.size()); ++iupd)
    {
        const Update& update = updates[iupd];
        a[update.i] = update.v;
        st.update(update.i, update.v);
        if (!single_query)
        {
            rc = test_case_all_queries(a, st, updates, iupd + 1);
        }
    }
    if (single_query)
    {
        rc = test_query(a, st, al, ar, updates, updates.size());
    }
    return rc;
}

int test_specific(int argc, char** argv)
{
    int ai = 0;
    size_t n = strtoul(argv[ai++], nullptr, 0); 
    size_t al = strtoul(argv[ai++], nullptr, 0); 
    size_t ar = strtoul(argv[ai++], nullptr, 0); 
    vupdate_t updates;
    while (ai + 1 < argc)
    {
        size_t i = strtoul(argv[ai++], nullptr, 0); 
        ull_t v = strtoul(argv[ai++], nullptr, 0); 
        updates.push_back(Update(i, v));
    }
    return test_case(n, al, ar, updates);
}

int test_random(int argc, char** argv)
{
    typedef SegmentTree::e_t e_t;
    int rc = 0;
    int ai = 0;
    const size_t ntests = strtoul(argv[ai++], nullptr, 0); 
    size_t n_min = strtoul(argv[ai++], nullptr, 0); 
    size_t n_max = strtoul(argv[ai++], nullptr, 0); 
    size_t upd_max = strtoul(argv[ai++], nullptr, 0); 
    e_t vmax = strtoul(argv[ai++], nullptr, 0); 
    cerr << "ntests="<<ntests <<
        ", n_min="<<n_min << ", n_max="<<n_max <<
        ", upd_max="<<upd_max << ", vmax="<<vmax <<
        '\n';
    for (size_t ti = 0; (rc == 0) && (ti < ntests); ++ti)
    {
        cerr << "tested: " << ti << '/' << ntests << '\n';
        size_t n = n_min + rand() % ((n_max + 1) - n_min);
        size_t n_upd = rand() % (upd_max + 1);
        vupdate_t updates;
        while (updates.size() < n_upd)
        {
            size_t i = rand() % n;
            e_t v = rand() % (vmax + 1);
            updates.push_back(Update(i, v));
        }
        rc = test_case(n, 0, n - 1, updates);
    }
    return rc;
}

int main(int argc, char** argv)
{
    int rc = 0;
    string method(argc > 1 ? argv[1] : "");
    if (method == string("specific"))
    {
        rc = test_specific(argc - 2, argv + 2);
    }
    else if (method == string("random"))
    {
        rc = test_random(argc - 2, argv + 2);
    }
    else
    {
        cerr << "Bad method: " << method << '\n';
        rc = 1;
    }
    return rc;
}
