#include <iostream>
#include <numeric>
#include <vector>
#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef long long ll_t;

// See: https://cp-algorithms.com/data_structures/segment_tree.html#range-updates-lazy-propagation
//

// n=5
//              [0:4]
//       [0:2]        [3:4]
//    [0:1] [2:2]  [3:3] [4:4]
// [0:0] [1:]

class SegmentTreeNode
{
 public:
    typedef ll_t e_t;
    SegmentTreeNode(e_t _add=0) : add(_add) {}
    e_t add;
};

class SegmentTree
{
 public:
    typedef SegmentTreeNode node_t;
    typedef node_t::e_t e_t;
    SegmentTree(size_t _sz) : sz(_sz), tnodes(4*sz, node_t()) {} 
    void update(size_t l, size_t r, e_t add);
    e_t query_pos(size_t i) const;
    void print(ostream& os) const;
 private:
    typedef vector<SegmentTreeNode> vnode_t;
    void update_tree(
        size_t l,
        size_t r,
        e_t add,
        size_t ti,
        size_t segl,
        size_t segr);
    e_t query_tree(size_t i, size_t ti, size_t segl, size_t segr) const;
    void print_tree(
        ostream& os,
        const string& indent,
        size_t al,
        size_t ar,
        size_t ti) const;
    const size_t sz;
    vnode_t tnodes;
};

void SegmentTree::update(size_t l, size_t r, e_t add)
{
    update_tree(l, r, add, 0, 0, sz - 1);
}

void SegmentTree::update_tree(
    size_t l,
    size_t r,
    e_t add,
    size_t ti,
    size_t segl,
    size_t segr
)
{
    // sehl <= l <= r <= segr
    if ((segl == l) && (r == segr))
    {
        tnodes[ti].add += add;
    }
    else
    {
        size_t delta = segr - segl;
        size_t midl = segl + delta/2;
        size_t midr = midl + 1;
        if (l <= midl)
        {
            update_tree(l, min(r, midl), add, 2*ti + 1, segl, midl);
        }
        if (midr <= r)
        {
            update_tree(max(l, midr), r, add, 2*ti + 2, midr, segr);
        }
    }
}

SegmentTree::e_t SegmentTree::query_pos(size_t i) const
{
    return query_tree(i, 0, 0, sz - 1);
}

SegmentTree::e_t
SegmentTree::query_tree(size_t i, size_t ti, size_t segl, size_t segr) const
{
    e_t r = tnodes[ti].add;
    if (segl < segr)
    {
        size_t delta = segr - segl;
        size_t midl = segl + delta/2;
        size_t midr = midl + 1;
        e_t qr = (i <= midl)
            ? query_tree(i, 2*ti + 1, segl, midl)
            : query_tree(i, 2*ti + 2, midr, segr);
        r += qr;
    }
    return r;
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
    Update(size_t _l=0, size_t _r=0, e_t _add=0) : l(_l), r(_r), add(_add) {}
    size_t l;
    size_t r;
    e_t add;
};
typedef vector<Update> vupdate_t;

int test_query(
    const vector<SegmentTree::e_t>& a,
    const SegmentTree& st,
    size_t i,
    const vupdate_t& updates,
    size_t n_upd
)
{
    typedef SegmentTree::e_t e_t;
    int rc = 0;
    const e_t naive = a[i];
    const e_t mature = st.query_pos(i);
    if (naive != mature)
    {
        rc = 1;
        cerr << "naive="<<naive << ", mature="<<mature << '\n';
        st.print(cerr);
        cerr << "failed: specific " << a.size() << ' ' << i;
        for (size_t j = 0; j < n_upd; ++j)
        {
             const Update& u = updates[j];
             cerr << ' ' << u.l << ' ' << u.r << ' ' << u.add;
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
    for (size_t i = 0; (rc == 0) && (i < n); ++i)
    {
        rc = test_query(a, st, i, updates, n_upd);
    }
    return rc;
}


int test_case(size_t n, size_t i, const vupdate_t& updates)
{
    typedef SegmentTree::e_t e_t;
    int rc = 0;
    vector<e_t> a(n, 0);
    SegmentTree st(n);
    bool single_query = (i < n);
    for (size_t iupd = 0; (rc == 0) && (iupd < updates.size()); ++iupd)
    {
        const Update& update = updates[iupd];
        for (size_t ai = update.l; ai <= update.r; ++ai)
        {
            a[ai] += update.add;
        }
        st.update(update.l, update.r, update.add);
        if (!single_query)
        {
            rc = test_case_all_queries(a, st, updates, iupd + 1);
        }
    }
    if (single_query)
    {
        rc = test_query(a, st, i, updates, updates.size());
    }
    return rc;
}

int test_specific(int argc, char** argv)
{
    int ai = 0;
    size_t n = strtoul(argv[ai++], nullptr, 0); 
    size_t i = strtoul(argv[ai++], nullptr, 0); 
    cerr << "n=" << n << ", i="<<i << "... updates\n";
    vupdate_t updates;
    while (ai + 2 < argc)
    {
        size_t l = strtoul(argv[ai++], nullptr, 0); 
        size_t r = strtoul(argv[ai++], nullptr, 0); 
        ull_t add = strtoul(argv[ai++], nullptr, 0); 
        updates.push_back(Update(l, r, add));
    }
    return test_case(n, i, updates);
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
    e_t vmin = strtoul(argv[ai++], nullptr, 0); 
    e_t vmax = strtoul(argv[ai++], nullptr, 0); 
    cerr << "ntests="<<ntests <<
        ", n_min="<<n_min << ", n_max="<<n_max <<
        ", upd_max="<<upd_max << 
        ", vmin="<<vmin << ", vmax="<<vmax <<

        '\n';
    for (size_t ti = 0; (rc == 0) && (ti < ntests); ++ti)
    {
        cerr << "tested: " << ti << '/' << ntests << '\n';
        size_t n = n_min + rand() % ((n_max + 1) - n_min);
        size_t n_upd = rand() % (upd_max + 1);
        vupdate_t updates;
        while (updates.size() < n_upd)
        {
            size_t l = rand() % n;
            size_t r = rand() % n;
            if (r < l) { swap(l, r); }
            e_t v = vmin + rand() % ((vmax + 1) - vmin);
            updates.push_back(Update(l, r, v));
        }
        rc = test_case(n, n, updates);
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
