#include <iostream>
#include <algorithm>
#include <array>
#include <vector>

bool kd_debug = false;

using namespace std;
typedef unsigned u_t;
typedef array<u_t, 2> au2_t;
typedef array<au2_t, 2> au2_2_t;
typedef vector<au2_2_t> vau2_2_t;
typedef vector<u_t> vu_t;
typedef vector<bool> vb_t;

template <int dim>
using AUD = array<u_t, dim>;

template <int dim>
using VD = vector<AUD<dim>>;

template <int dim>
using AUD2 = array<AUD<dim>, 2>;

template <int dim>
using AVUD = array<vu_t, dim>;

template <int dim>
using AU2D = array<au2_t, dim>; // Box = product of segments, [begin:end]

template <int dim>
using VDMinMax = vector<AUD2<dim>>; // points

template <int dim>
using VMinMaxD = vector<AU2D<dim>>; // segments

template <int dim>
void box_to_pt(AUD<dim>& pt, const AU2D<dim>& box, u_t zo)
{
    for (u_t d = 0; d != dim; ++d)
    {
        const au2_t& seg = box[d];
        pt[d] = seg[zo];
    }
}

void minby(u_t& v, u_t x)
{
    if (v > x)
    {
        v = x;
    }
}

void maxby(u_t& v, u_t x)
{
    if (v < x)
    {
        v = x;
    }
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
    os << "\n" << indent << "count=" << count << "\n";
    for (u_t ci: {0, 1})
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
class _KDSG_View
{
 public:
    typedef _KDSG_View<dim> view_t;
    _KDSG_View(vb_t& _take, u_t _age=0) : age(_age), take(_take) {}
    void set_lut(u_t d, const VMinMaxD<dim>& aminmax, const vu_t& plut);
    void set_lut_others(u_t d, const view_t& parent_view);
    u_t age;
    AU2D<dim> bbox;
    AVUD<dim> lut; // per dimension sorted  2*index + {0,1}={from,to}
    vb_t& take;
};

template <int dim>
void _KDSG_View<dim>::set_lut(u_t d, const VMinMaxD<dim>& aminmax,
    const vu_t& plut)
{
    lut[d].clear();
    const au2_t& seg = bbox[d];
    for (u_t li: plut)
    {
        u_t i = li / 2;
        const au2_t& mm = aminmax[i][d];
        if ((mm[0] <= seg[1]) && (seg[0] <= mm[1]))
        {
            lut[d].push_back(li);
        }
    }
}

template <int dim>
void _KDSG_View<dim>::set_lut_others(u_t d, const view_t& parent_view)
{
    const vu_t& dlut = lut[d];
    for (u_t i: dlut)
    {
        take[i] = true;
    }
    for (u_t di = 0; di != dim; ++di)
    {
        if (di != d)
        {
            lut[di].clear();
            for (u_t i: parent_view.lut[di])
            {
                if (take[i])
                {
                    lut[di].push_back(i);
                }
            }
        }
    }
    for (u_t i: dlut)
    {
        take[i] = false;
    }
}


template <int dim>
class KD_SegTree
{
 public:
    typedef VMinMaxD<dim> vmm_t;
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
    typedef _KDSG_View<dim> view_t;
    node_t* create_sub_tree(const vmm_t& aminmax, const view_t& view, u_t depth);
    void node_add_segment(node_t* t, const AU2D<dim>& seg, const u_t depth);
    u_t node_pt_n_intersections(const node_t* t, const AUD<dim>& pt) const;
    node_t* root;
};

template <int dim>
void KD_SegTree<dim>::init_leaves(const vmm_t& aminmax)
{
    const u_t n_mm = aminmax.size();
    vb_t take(2*n_mm, false);
    view_t view(take, 0);
    for (u_t d = 0; d < dim; ++d)
    {
        au2_t& bb = view.bbox[d];
        bb = aminmax[0][d];
        vu_t& lut = view.lut[d];
        // vu_t& lut_inv = view.lut_inv[d];
        lut.reserve(2*n_mm);
        for (u_t i = 0, i2 = 0; i != n_mm; ++i)
        {
            lut.push_back(i2++);
            lut.push_back(i2++);
            minby(bb[0], aminmax[i][d][0]);
            maxby(bb[1], aminmax[i][d][1]);
        }
        sort(lut.begin(), lut.end(),
            [aminmax, d](u_t i0, u_t i1) -> bool
            {
                u_t ft0 = i0 % 2;
                u_t ft1 = i1 % 2;
                i0 /= 2;
                i1 /= 2;
                u_t x0 = aminmax[i0][d][ft0];
                u_t x1 = aminmax[i1][d][ft1];
                bool lt = (x0 < x1) || ((x0 == x1) && (ft0 < ft1));
                return lt;
            });
    }
    root = create_sub_tree(aminmax, view, 0);
}

template <int dim>
KDSegTreeNode<dim>* KD_SegTree<dim>::create_sub_tree(
    const vmm_t& aminmax, const view_t& view, u_t depth)
{
    node_t* t = new node_t();
    t->bbox = view.bbox;
    if (!view.lut[depth % dim].empty())
    {
        const u_t d = depth % dim;
        const vu_t& lut = view.lut[d];
        u_t aimed = u_t(-1); // undef
        if (view.bbox[d][0] < view.bbox[d][1])
        {
            vu_t::const_iterator lb, ub; // of the bbox !!!!!!!!!!!!!
            lb = upper_bound(lut.begin(), lut.end(), view.bbox[d][0],
                [aminmax, d](u_t xv, u_t i) -> bool
                {
                    const u_t zo = i % 2;
                    const u_t v = aminmax[i/2][d][zo];
                    bool lt = (xv < v) || ((xv == v) && (zo == 1));
                    return lt;
                });
            ub = lower_bound(lut.begin(), lut.end(), view.bbox[d][1],
                [aminmax, d](u_t i, u_t xv) -> bool
                {
                    const u_t zo = i % 2;
                    const u_t v = aminmax[i/2][d][zo];
                    bool lt = (v < xv) || ((v == xv) && (zo == 0));
                    return lt;
                });
            u_t lbi = lb - lut.begin();
            u_t ubi = ub - lut.begin() - 1;
            if (kd_debug) { cerr << "lbi="<<lbi << ", ubi="<<ubi << '\n'; }
            if (lbi <= ubi)
            {
                u_t imed = (lbi + ubi)/2;
                aimed = lut[imed];
            }
        }
        if (aimed == u_t(-1))
        {
            if (view.age + 1 < dim)
            {
                view_t sub_view(view);
                ++sub_view.age;
                t->child[0] = create_sub_tree(aminmax, sub_view, depth + 1);
            }
        }
        else
        {
            // need to find sub-range within bbox[d];
            const u_t zo = aimed % 2;
            u_t xmed = aminmax[aimed / 2][d][zo];
            view_t sub_view(view.take);
            sub_view.bbox = view.bbox;
            u_t ci = 0;

            // low
            sub_view.bbox[d][1] = xmed - (1 - zo); // if zo=0, xmed is high
            sub_view.set_lut(d, aminmax, lut);
            sub_view.set_lut_others(d, view);
            t->child[ci++] = create_sub_tree(aminmax, sub_view, depth + 1);

            // high
            sub_view.bbox[d][0] = sub_view.bbox[d][1] + 1;
            sub_view.bbox[d][1] = view.bbox[d][1];
            if (view.bbox[d][0] <= view.bbox[d][1])
            {
                sub_view.set_lut(d, aminmax, lut);
                sub_view.set_lut_others(d, view);
                t->child[ci] = create_sub_tree(aminmax, sub_view, depth + 1);
            }
        }
    }
    return t;
}

template <int dim>
void KD_SegTree<dim>::node_add_segment(KDSegTreeNode<dim>* t,
    const AU2D<dim>& seg, u_t depth)
{
    bool lte_l = lte01<dim>(seg, t->bbox, 0);
    bool lte_h = lte01<dim>(t->bbox, seg, 1);
    if (lte_l && lte_h)
    {
        ++(t->count);
    }
    else
    {
        for (node_t* child: t->child)
        {
            if (child && boxes_intersect<dim>(seg, child->bbox))
            {
                node_add_segment(child, seg, depth);
            }
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
                for (u_t d = 0; d != dim; ++d)
                {
                    for (u_t zo: {0, 1})
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
            for (u_t d = 0; d != dim; ++d)
            {
                for (u_t zo = 0; zo != 2; ++zo, ++ai)
                {
                    seg[d][zo] = stoi(argv[ai]);
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
