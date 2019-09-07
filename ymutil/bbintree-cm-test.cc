#include "bbintree.h"
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

#include <cstring>

using namespace std;

typedef unsigned u_t;
typedef vector<u_t> vu_t;

u_t debug_flags = 0;

void permutation_first(vu_t &p, u_t n)
{
    p = vu_t(n);
    iota(p.begin(), p.end(), 0);
}

class Data
{
 public:
    Data(int _v=0) : v(_v), count(1), vmin(_v), vmax(_v) {}
    int v;
    u_t count;
    int vmin, vmax;
};
bool operator<(const Data& d0, const Data& d1) { return d0.v < d1.v; }

class BBTCB_MinMax : public BBinTreeCallBack<Data>
{
 public:
    void insert_pre_balance(node_ptr_t p)
    {
        if (debug_flags & 0x2) { cerr << __func__ << '\n'; }
        for (node_ptr_t pp = p->parent; pp; p = pp, pp = pp->parent)
        {
            pp->data.count += 1;
            pp->data.vmin = min(pp->data.vmin, p->data.vmin);
            pp->data.vmax = max(pp->data.vmax, p->data.vmax);
        }
    }
    void remove_replace(node_ptr_t p, node_ptr_t s)
    {
        if (debug_flags & 0x2) { cerr << __func__ << '\n'; }
        // init_data(s);
        s->data.count = p->data.count - 1;
        minmax_by_children(s, p->child);
        node_ptr_t pp = s->parent;
        if (pp != p)
        {
            init_data(pp);
            int ci = int(s == pp->child[1]);
            int oci = 1 - ci;
            const node_ptr_t gc2[2] = {pp->child[oci], s->child[oci]};
            for (int gci = 0; gci != 2; ++gci)
            {
                const node_ptr_t gc = gc2[gci];
                if (gc)
                {
                    pp->data.count += gc->data.count;
                    minmax_by(pp, gc);
                }
            }
            for (pp = pp->parent; pp != p; pp = pp->parent)
            {
                re_minmax(pp);
                --(pp->data.count);
            }
        }
        init_data(p); // not needed ??
    }
    void remove_pre_balance(node_ptr_t p, node_ptr_t c)
    {
        if (debug_flags & 0x2) { cerr << __func__ << '\n'; }
        if (c)
        {
            init_data(c);
            for (int ci = 0; ci != 2; ++ci)
            {
                node_ptr_t cc = c->child[ci];
                c->data.count += (cc ? cc->data.count : 0);
            }
            re_minmax(c);
            for (node_ptr_t pp = c->parent; pp; pp = pp->parent)
            {
                init_minmax(pp);
                --(pp->data.count);
                re_minmax(pp);
            }
        }
        else
        {
            for (node_ptr_t pp = p->parent; pp; pp = pp->parent)
            {
                init_minmax(pp);
                --(pp->data.count);
                re_minmax(pp);
            }
        }
    }
    void pre_rotate(node_ptr_t p, unsigned ci)
    {
        if (debug_flags & 0x2) { cerr << __func__ << '\n'; }
        node_ptr_t c = p->child[ci];
        c->data.count = p->data.count;
        minmax_by(c, p);
        int oci = 1 - ci;
        node_ptr_t oc = p->child[oci];
        // node_ptr_t occ = oc ? oc->child[ci] : 0;
        node_ptr_t gc2[2] = {c->child[oci], oc};
        init_data(p);
        for (int gci = 0; gci != 2; ++gci)
        {
            node_ptr_t gc = gc2[gci];
            if (gc)
            {
                p->data.count += gc->data.count;
                minmax_by(p, gc);
            }
        }
    }
 private:
    static void init_data(node_ptr_t p)
    {
        p->data.count = 1;
        init_minmax(p);
    }
    static void init_minmax(node_ptr_t p)
    {
        Data& d = p->data;
        d.vmin = d.vmax = d.v;
    }
    static void re_minmax(node_ptr_t p)
    {
        Data& data = p->data;
        data.vmin = data.vmax = data.v;
        minmax_by_children(p, p->child);
    }
    static void minmax_by_children(node_ptr_t p, const node_ptr_t* chldren)
    {
        for (int ci = 0; ci != 2; ++ci)
        {
            node_ptr_t pc = chldren[ci];
            if (pc)
            {
                minmax_by(p, pc);
            }
        }
    }
    static void minmax_by(node_ptr_t p, const node_ptr_t c)
    {
        min_by(p, c);
        max_by(p, c);
    }
    static void min_by(node_ptr_t p, const node_ptr_t c)
    {
        if (p->data.vmin > c->data.vmin)
        {
            p->data.vmin = c->data.vmin;
        }
    }
    static void max_by(node_ptr_t p, const node_ptr_t c)
    {
        if (p->data.vmax < c->data.vmax)
        {
            p->data.vmax = c->data.vmax;
        }
    }
};

typedef BBinTree<Data, std::less<Data>, BBTCB_MinMax> bbtmm_t;

static void print_subtree(ostream &os, const bbtmm_t::node_ptr_t node,
    const string pfx="")
{
    const Data& d = node->data;
    os << pfx << "v="<<d.v << ", min="<<d.vmin << ", max="<<d.vmax <<
        ", count="<<d.count << '\n';
    string cpfx = pfx + string("  ");
    for (int ci = 0; ci != 2; ++ci)
    {
        if (node->child[ci])
        {
            print_subtree(os, node->child[ci], cpfx);
        }
        else
        {
            os << cpfx << "--null--\n";
        }
    }
}

static bool valid_node(const bbtmm_t::node_ptr_t node,
    u_t& ncount, int& nmin, int& nmax)
{
    bool ok = true;
    const Data& data = node->data;
    ncount = 1;
    nmin = nmax = data.v;
    for (int ci = 0; ok && (ci < 2); ++ci)
    {
        const bbtmm_t::node_ptr_t child = node->child[ci];
        if (child)
        {
            u_t ccount;
            int cmin, cmax;
            ok = valid_node(child, ccount, cmin, cmax);
            if (ok)
            {
                ncount += ccount;
                nmin = min(nmin, cmin);
                nmax = max(nmax, cmax);
            }
        }
    }
    ok = ok && (ncount == data.count);
    if (ok && (ncount > 0))
    {
        ok = ok && (nmin == data.vmin);
        ok = ok && (nmax == data.vmax);
    }
    return ok;
}

static bool valid(const bbtmm_t& tree)
{

    bool ok = true;
    const bbtmm_t::node_ptr_t root = tree.get_root();
    if (root)
    {
        if (debug_flags & 0x1) { print_subtree(cerr, root); }
        u_t rcount;
        int rmin, rmax;
        ok = valid_node(root, rcount, rmin, rmax);
    }
    return ok;
}

static int test_insert_deletion(const vu_t& vin, const vu_t& vout)
{
    bool ok = true;
    bbtmm_t tree;
    u_t iin, iout;
    for (iin = 0; ok && (iin < vin.size()); ++iin)
    {
        u_t v = vin[iin];
        tree.insert(Data(v));
        ok = valid(tree);
    }
    for (iout = 0; ok && (iout < vout.size()); ++iout)
    {
        u_t v = vout[iout];
        tree.remove(Data(v));
        ok = valid(tree);
    }
    if (!ok)
    {
        cerr << "Failed, debug with\n -special " << iin;
        for (u_t k = 0; k < iin; ++k)
        {
            cerr << ' ' << vin[k];
        }
        for (u_t k = 0; k < iout; ++k)
        {
            cerr << ' ' << vout[k];
        }
        cerr << '\n';
    }
    return ok ? 0 : 1;
}

static int special(int argc, char **argv)
{
    vu_t vin, vout;
    int ai = 0;
    u_t n_in = strtoul(argv[ai++], 0, 0);
    while (vin.size() < n_in)
    {
        debug_flags |= (1 + 2);
        vin.push_back(strtoul(argv[ai++], 0, 0));
    }
    while (ai < argc)
    {
        vout.push_back(strtoul(argv[ai++], 0, 0));
    }
    int rc = test_insert_deletion(vin, vout);
    return rc;
}

static int test_size(u_t size)
{
    int rc = 0;
    vu_t vin, vout;
    bool more_in = true;
    permutation_first(vin, size);
    while (more_in && (rc == 0))
    {
        bool more_out = true;
        permutation_first(vout, size);
        while (more_out && (rc == 0))
        {
            rc = test_insert_deletion(vin, vout);
            more_out = next_permutation(vout.begin(), vout.end());
        }
        more_in = next_permutation(vin.begin(), vin.end());
    }
    return rc;
}

int main(int argc, char **argv)
{
    int rc = 0;
    bbtmm_t tree;

    if (!strcmp(argv[1], "-special"))
    {
        rc = special(argc - 2, argv + 2);
    }
    else
    {
        u_t size = strtoul(argv[1], 0, 0);
        rc = test_size(size);
    }
    return rc;
}
