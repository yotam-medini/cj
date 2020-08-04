#include <algorithm>
#include <array>
#include <iostream>
#include <vector>

using namespace std;
typedef unsigned u_t;
typedef array<u_t, 2> au2_t;
typedef array<au2_t, 2> au2_2_t;
typedef vector<au2_2_t> vau2_2_t;
typedef vector<u_t> vu_t;

template <int dim>
using AUD = array<u_t, dim>;

template <int dim>
using VD = vector<AUD<dim>>;

template <int dim>
using AUD2 = array<AUD<dim>, 2>;

template <int dim>
using AU2D = array<au2_t, dim>; // Box = product of segments

template <int dim>
using VDMinMax = vector<AUD2<dim>>;

template <int dim>
using VMinMaxD = vector<AU2D<dim>>;

template <int dim>
class LessAU
{
 public:
    LessAU(u_t _d) : d(_d) {}
    bool operator()(const AUD<dim>& x0, const AUD<dim>& x1) const
    {
        bool lt = x0[d] < x1[d];
        return lt;
    }
 private:
    u_t d;
};

template <int dim>
bool lt01(const AU2D<dim>& x0, const AU2D<dim>& x1, u_t z0)
{
    bool ret = true;
    for (int i = 0; ret && (i < dim); ++i)
    {
        ret = (x0[i][z0] < x1[i][z0]);
    }
    return ret;
}

template <int dim>
bool lte01(const AU2D<dim>& x0, const AU2D<dim>& x1, u_t z0)
{
    bool ret = true;
    for (int i = 0; ret && (i < dim); ++i)
    {
        ret = (x0[i][z0] <= x1[i][z0]);
    }
    return ret;
}

template <int dim>
bool boxes_intersect(const AU2D<dim>& b0, const AU2D<dim>& b1)
{
    bool intersect = true;
    for (int d = 0; intersect && (d < dim); ++d)
    {
        intersect = (b0[d][0] <= b1[d][1]) && (b1[d][0] <= b0[d][1]);
    }
    return intersect;
}

template <int dim>
bool pt_in_box(const AUD<dim>& pt, const AU2D<dim>& box)
{
    bool inside = true;
    for (int d = 0; inside && (d < dim); ++d)
    {
        inside = (box[d][0] <= pt[d]) && (pt[d] <= box[d][1]);
    }
    return inside;
}

// if a[i][d] constant then  im=ie
template <int dim>
u_t OLDbisect(VD<dim>& a, au2_2_t& lh_seg, u_t d, u_t ib, u_t ie)
{
    typedef AUD<dim> audim_t;
    u_t im = (ib + ie + 1)/2; // we want low noy to be smaller
    typename VD<dim>::iterator b = a.begin();
    const LessAU<dim> ltd(d);
    nth_element(b + ib, b + im, b + ie, ltd);
    u_t dmin = (*min_element(b + ib, b + im, ltd))[d];
    u_t l_max = (*max_element(b + ib, b + im, ltd))[d];
    u_t h_min = (*min_element(b + im, b + ie, ltd))[d];
    u_t dmax = (*max_element(b + im, b + ie, ltd))[d];
    lh_seg[0][0] = dmin;
    lh_seg[0][1] = l_max;
    lh_seg[1][0] = h_min;
    lh_seg[1][1] = dmax;
    if (l_max == h_min)
    {
        u_t nlow = count_if(b + ib, b + im,
            [d, l_max](const audim_t& x) -> bool { return x[d] == l_max; });
        u_t nhigh = count_if(b + im, b + ie,
            [d, h_min](const audim_t& x) -> bool { return x[d] == h_min; });
        im = (nlow < nhigh ? im - nlow : im + nhigh);
        // consuder nlow + nlow == sz == ie - ib
        nth_element(b + ib, b + im, b + ie, ltd);
        if (nlow < nhigh)
        {
            lh_seg[0][1] = (ib < im ? (*max_element(b + ib, b + im, ltd))[d] :
                dmin);
        }
        else
        {
            lh_seg[1][0] = (im < ie ? (*min_element(b + im, b + ie, ltd))[d] :
                dmax);
        }
    }
    return im;
}

void bisect(vu_t& a, au2_2_t& lh_seg)
{
    sort(a.begin(), a.end());
    lh_seg[0][0] = a.front();
    lh_seg[1][1] = a.back();
    if (a.front() == a.back())
    {
        lh_seg[0][1] = lh_seg[1][0] = a.front();
    }
    else
    {
        u_t nvals = 0;
        u_t last(-1);
        for (u_t x: a)
        {
            nvals += (x != last ? 1 : 0);
            last = x;
        }
        // nvals >= 2 !!
        u_t nlow = nvals/2;
        nvals = 0;
        last = u_t(-1);
        u_t i;
        for (i = 0; nvals < nlow; ++i)
        {
            nvals += (a[i] != last ? 1 : 0);
            last = a[i];
        }
        lh_seg[0][1] = last;
        for (; a[i] == last; ++i) {}
        lh_seg[1][0] = a[i];
    }
}

template <int dim>
class KDSegTreeNode
{
 public:
    KDSegTreeNode(u_t _c=0) : child{0, 0}, count(_c) {}
    virtual ~KDSegTreeNode();
    void print(ostream& os, u_t depth) const;
    KDSegTreeNode* child[2];
    AU2D<dim> bbox;
    u_t count;
};

template <int dim>
KDSegTreeNode<dim>::~KDSegTreeNode()
{
    delete child[0];
    delete child[1];
}

template <int dim>
void KDSegTreeNode<dim>::print(ostream& os, u_t depth) const
{
    const string indent(2*depth, ' ');
    os << indent << "BB: ";
    for (u_t i = 0; i != 2; ++i)
    {
        os << "[";
        const char* sep = "";
        for (u_t d = 0; d != dim; ++d)
        {
            os << sep << bbox[d][i]; sep = ", ";
        }
        os << "]" << (i == 0 ? " -> " : "");
    }
    os << "\ncount=" << count << "\n";
    for (u_t ci = 0; ci != 2; ++ci)
    {
        if (child[ci])
        {
            child[ci]->print(os, depth + 1);
        }
        else
        {
            os << indent << "  --\n";
        }
    }
}

template <int dim>
class KD_SegTree
{
 public:
    KD_SegTree() : root(0) {}
    virtual ~KD_SegTree() { delete root; }
    void init_leaves(const VMinMaxD<dim>& aminmax);
    void add_segment(const AU2D<dim>& seg)
    {
        node_add_segment(root, seg, 0);
    }
    u_t pt_n_intersections(const AUD<dim>& pt) const
    {
        return (root ? node_pt_n_intersections(root, pt) : 0);
    }
    void print(ostream& os=cerr) const;
 private:
    typedef KDSegTreeNode<dim> node_t;
    node_t* create_sub_tree(VMinMaxD<dim>& aminmax, u_t depth, AU2D<dim>& bbox);
    void node_add_segment(node_t* t, const AU2D<dim>& seg, const u_t depth);
    u_t node_pt_n_intersections(const node_t* t, const AUD<dim>& pt) const;
    node_t* root;
};

template <int dim>
void KD_SegTree<dim>::init_leaves(const VMinMaxD<dim>& aminmax)
{
    if (!aminmax.empty())
    {
        VMinMaxD<dim> as(aminmax);
        sort(as.begin(), as.end());
        VMinMaxD<dim> au;
        au.reserve(as.size());
        AU2D<dim> bbox = as[0];
        au.push_back(as[0]);
        for (const AU2D<dim>& mm: as)
        {
            if (mm != au.back())
            {
                au.push_back(mm);
                for (u_t d = 0; d < dim; ++d)
                {
                    if (bbox[d][0] > mm[d][0])
                    {
                        bbox[d][0] = mm[d][0];
                    }
                    if (bbox[d][1] < mm[d][1])
                    {
                        bbox[d][1] = mm[d][1];
                    }
                }
            }
        }
        root = create_sub_tree(au, 0, bbox);
    }
}

template <int dim>
KDSegTreeNode<dim>* KD_SegTree<dim>::create_sub_tree(
   VMinMaxD<dim>& aminmax, u_t depth, AU2D<dim>& bbox)
{
    const u_t d = depth % dim;
    node_t* t = new node_t();
    bool all_same = adjacent_find(aminmax.begin(), aminmax.end(),
        not_equal_to<AU2D<dim>>()) == aminmax.end();
    if (all_same)
    {
        t->bbox = aminmax[0];
    }
    else
    {
        t->bbox = bbox;
        vu_t a;
        a.reserve(2*aminmax.size());
        for (const AU2D<dim> mm: aminmax)
        {
            a.push_back(mm[d][0]);
            a.push_back(mm[d][1]);
        }
        au2_2_t lh_seg;
        bisect(a, lh_seg);
        // u_t cut_val = sort_midval(a);
        if (lh_seg[0][0] == lh_seg[1][1]) // constant
        {
            t->child[0] = create_sub_tree(aminmax, depth + 1, bbox);
        }
        else
        {
            u_t cut_val = (lh_seg[0][1] + lh_seg[1][0])/2;
            VMinMaxD<dim> amm[2];
            bool any_cut = false;
            for (const AU2D<dim> mm: aminmax)
            {
                if (mm[d][1] <= lh_seg[0][1])
                {
                    amm[0].push_back(mm);
                }
                else if (mm[d][0] >= lh_seg[1][0])
                {
                    amm[1].push_back(mm);
                }
                else // split
                {
                    any_cut = true;
                    amm[0].push_back(mm);
                    amm[0].back()[d][1] = cut_val;
                    amm[1].push_back(mm);
                    amm[1].back()[d][0] = cut_val + 1;
                }
            }
            const au2_t bbox_d = bbox[d];
            if (any_cut)
            {
                lh_seg[0][1] = cut_val;
                lh_seg[1][0] = cut_val + 1;
            }
            for (u_t ci: {0, 1})
            {
                if (!amm[ci].empty())
                {
                    bbox[d] = lh_seg[ci];
                    t->child[0] = create_sub_tree(amm[ci], depth + 1, bbox);
                }
            }
            bbox[d] = bbox_d; // restore
        }
    }
    return t;
}

template <int dim>
void KD_SegTree<dim>::node_add_segment(KDSegTreeNode<dim>* t,
    const AU2D<dim>& seg, u_t depth)
{
    // u_t const d = depth % dim;
    bool lte_l = lte01<dim>(seg, t->bbox, 0);
    bool lte_h = lte01<dim>(t->bbox, seg, 1);
    if (lte_l && lte_h)
    {
        ++(t->count);
    }
    for (node_t* child: t->child)
    {
        if (child && boxes_intersect<dim>(seg, child->bbox))
        {
            node_add_segment(child, seg, depth);
        }
    }
}

template <int dim>
u_t KD_SegTree<dim>::node_pt_n_intersections(
    const node_t* t, const AUD<dim>& pt) const
{
    u_t n = 0;
    if (pt_in_box<dim>(pt, t->bbox))
    {
        n += t->count;
        for (const node_t* child: t->child)
        {
            if (child)
            {
                n += node_pt_n_intersections(child, pt);
            }
        }
    }
    return n;
}

template <int dim>
void KD_SegTree<dim>::print(ostream& os) const
{
    os << "{ KD_SegTree<" << dim << ">\n";
    if (root) { root->print(os, 0); }
    os << "}\n";
}

template <int dim>
u_t pt_n_intersections_naive(const AUD<dim>& pt, const VMinMaxD<dim>& segs)
{
    u_t n = 0;
    for (const AU2D<dim>& seg: segs)
    {
        n += pt_in_box<dim>(pt, seg);
    }
    return n;
}

template <int dim>
int test(const VMinMaxD<dim>& segs, const VD<dim>& pts)
{
    int rc = 0;
    // vau2_2_t a;
    KD_SegTree<dim> tree;
    cerr << "init_leaves\n";
    tree.init_leaves(segs);
    tree.print();
    cerr << "adding segments\n";
    u_t si = 0;
    VMinMaxD<dim> segs_added;
    for (const AU2D<dim>& seg: segs) // just 2/3 ?
    {
        if (si % 3 != 2)
        {
            tree.add_segment(seg);
            segs_added.push_back(seg);
        }
        ++si;
    }
    tree.print();
    for (u_t pti = 0, np = pts.size(); (rc == 0) && (pti < np); ++pti)
    {
        const AUD<dim>& pt = pts[pti];
        u_t n_naive = pt_n_intersections_naive<dim>(pt, segs_added);
        u_t n_kdt = tree.pt_n_intersections(pt);
        cerr << "pti="<<pti << ", n_kdt="<<n_kdt << '\n';
        if (n_kdt != n_naive)
        {
            rc = 1;
            cerr << "Failure: n_kdt="<<n_kdt << ", n_naive="<<n_naive << "\n"
                "  " << dim << " s";
            for (const AU2D<dim>& seg: segs)
            {
                for (u_t zo: {0, 1})
                {
                    for (u_t d = 0; d != dim; ++d)
                    {
                        cerr << ' ' << seg[d][zo];
                    }
                }
            }
            cerr << " p";
            for (u_t d = 0; d != dim; ++d)
            {
                cerr << ' ' << pt[d];
            }
            cerr << '\n';
        }
    }
    return rc;
}

template <int dim>
int specific_main(u_t argc, char **argv)
{
    u_t ai = 0;
    int rc = 1;
    if (string(argv[0]) == string("s"))
    {
        ++ai;
        VMinMaxD<dim> segs;
        while (string(argv[ai]) != string("p"))
        {
            AU2D<dim> seg;
            for (u_t i = 0; i != dim; ++i)
            {
                for (u_t mmi = 0; mmi != 2; ++mmi, ++ai)
                {
                    seg[i][mmi] = stoi(argv[ai]);
                }
            }
            segs.push_back(seg);
        }
        ++ai; // sskip "p"
        VD<dim> pts;
        while (ai < argc)
        {
            AUD<dim> pt;
            for (u_t i = 0; i != dim; ++i, ++ai)
            {
                pt[i] = stoi(argv[ai]);
            }
            pts.push_back(pt);
        }
        rc = test<dim>(segs, pts);
    }
    return rc;
}

int dim_specific_main(int argc, char **argv)
{
    u_t dim = stoi(argv[0]);
    int rc = 0;
    switch (dim)
    {
     case 1:
        rc = specific_main<1>(argc - 1, argv + 1);
        break;
     case 2:
        rc = specific_main<2>(argc - 1, argv + 1);
        break;
     default:
        cerr << "Unsupported dim=" << dim << '\n';
    }
    return rc;
}

#include <cstdlib>

template <int dim>
int rand_test(u_t maxval, u_t n_segs, u_t n_pts)
{
    VMinMaxD<dim> segs;
    while (segs.size() < n_segs)
    {
        AU2D<dim> seg;
        for (u_t d = 0; d != dim; ++d)
        {
            for (u_t zo: {0, 1})
            {
                seg[d][zo] = rand() % (maxval + 1);
            }
            u_t v0 = rand() % maxval;
            seg[d][0] = v0;
            seg[d][1] = v0 + (rand() % (maxval + 1 - v0));
        }
        segs.push_back(seg);
    }
    VD<dim> pts;
    while (pts.size() < n_pts)
    {
        AUD<dim> pt;
        for (u_t d = 0; d != dim; ++d)
        {
            pt[d] = rand() % (maxval + 1);
        }
        pts.push_back(pt);
    }
    int rc = test<dim>(segs, pts);
    return rc;
}

static int rand_main(int argc, char **argv)
{
    int rc = 0;
    int ai = 0;
    u_t dim = stoi(argv[ai++]);
    u_t n_tests = stoi(argv[ai++]);
    u_t maxval = stoi(argv[ai++]);
    u_t n_segs = stoi(argv[ai++]);
    u_t n_pts = stoi(argv[ai++]);
    for (u_t ti = 0; (rc == 0) && (ti != n_tests); ++ti)
    {
    switch (dim)
    {
     case 1:
        rc = rand_test<1>(maxval, n_segs, n_pts);
        break;
     case 2:
        rc = rand_test<2>(maxval, n_segs, n_pts);
        break;
     default:
        cerr << "Unsupported dim=" << dim << '\n';
    }

    }
    return rc;
}

static void usage(const string& prog)
{
    const string indent(prog.size() + 2, ' ');
    cerr << prog << " # ....\n" <<
        indent << "<dim> s <b1> <e1> <b2> <e2> ... p <x1> <x2> ...\n" <<
        indent << "or\n" <<
        indent << "rand <dim> <#tests> <maxval> <#segs> <#points>\n";
}

int main(int argc, char **argv)
{
    int rc = 0;
    if (argc == 1)
    {
        usage(argv[0]);
        rc = 1;
    }
    else if (string(argv[1]) == string("rand"))
    {
        cerr << "rand: not yet\n";
        rc = rand_main(argc - 2, argv + 2);
    }
    else
    {
        rc = dim_specific_main(argc - 1, argv + 1);
    }
    return rc;
}
