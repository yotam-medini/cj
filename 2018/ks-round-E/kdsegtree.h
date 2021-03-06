#define DROP 1
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
    typedef VD<dim> vpt_t;
#if defined(DROP)
    _KDSG_View(vb_t& _drop, u_t _age=0) : age(_age), drop(_drop) {}
#else
    _KDSG_View(vb_t& _take, u_t _age=0) : age(_age), take(_take) {}
    void set_lut(u_t d, const VMinMaxD<dim>& aminmax, const view_t& pview);
#endif
    void set_lut_others(u_t d, const view_t& parent_view);
    u_t age;
    AU2D<dim> bbox;
    AVUD<dim> lut; // per dimension sorted  2*index + {0,1}={from,to}
    vu_t xlut; // unsorted, intersects but may not be contained in BBox
#if defined(DROP)
    vb_t& drop;
#else
    vb_t& take;
#endif
};

#if !defined(DROP)
template <int dim>
void _KDSG_View<dim>::set_lut(u_t d, const VMinMaxD<dim>& aminmax,
    const view_t& parent_view)
{
    vu_t& lutd = lut[d];
    lutd.clear();
    xlut.clear();
    const au2_t& seg = bbox[d];
    if (seg[0] < seg[1])
    {
        for (u_t li: parent_view.lut[d])
        {
            u_t i = li / 2;
            const au2_t& mm = aminmax[i][d];
            u_t zo = li % 2;
            const u_t x = mm[zo];
            // if ((seg[0] <= x) && (x <= seg[1]))
            bool add = (seg[0] < x) && (x < seg[1]);
            add = add || ((seg[0] == x) && (zo == 1));
            add = add || ((x == seg[1]) && (zo == 0));
            if (add)
            {
                lutd.push_back(li);
            }
        }
    }
    for (u_t li: parent_view.xlut)
    {
        u_t i = li / 2;
        const au2_t& mm = aminmax[i][d];
        if ((mm[0] <= seg[1]) && (seg[0] <= mm[1]))
        {
            xlut.push_back(li);
        }        
    }
}
#endif

template <int dim>
void _KDSG_View<dim>::set_lut_others(u_t d, const view_t& parent_view)
{
#if defined(DROP)
    for (u_t di = 0; di != dim; ++di)
    {
        if (di != d)
        {
            vu_t& lutdi = lut[di];
            lut[di].clear();
            for (u_t i: parent_view.lut[di])
            {
                if (!drop[i])
                {
                    lutdi.push_back(i);
                }
            }
        }
    }
#else
    for (u_t i: xlut)
    {
        take[i] = true;
    }
    for (u_t di = 0; di != dim; ++di)
    {
        if (di != d)
        {
            vu_t& lutdi = lut[di];
            lut[di].clear();
            for (u_t i: parent_view.lut[di])
            {
                if (take[i])
                {
                    lutdi.push_back(i);
                }
            }
        }
    }
    for (u_t i: xlut)
    {
        take[i] = false;
    }
#endif
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
        node_add_segment(root, seg);
    }
    u_t pt_n_intersections(const AUD<dim>& pt) const
    {
        return (root ? node_pt_n_intersections(root, pt) : 0);
    }
    void print(ostream& os=cerr) const;
 private:
    typedef KDSegTreeNode<dim> node_t;
    typedef _KDSG_View<dim> view_t;
    typedef AUD<dim> pt_t;
    typedef VD<dim> vpt_t;
    node_t* create_sub_tree(const vmm_t& amm, const view_t& view, u_t depth);
    void node_add_segment(node_t* t, const AU2D<dim>& seg);
    u_t node_pt_n_intersections(const node_t* t, const AUD<dim>& pt) const;
    void set_drop(vu_t::const_iterator b, vu_t::const_iterator e,
        const vmm_t& aminmax, view_t& sub_view, u_t zo, bool flag)
    {
        for ( ; b != e; ++b)
        {
            const u_t ai = *b;
            if (ai % 2 == zo)
            {
                sub_view.drop[ai] = flag;
            }
        }
    }
    node_t* root;
};

template <int dim>
class MMComp
{
 public:
    MMComp(u_t _d, const VMinMaxD<dim>& mm) : d(_d), aminmax(mm) {}
    bool operator()(u_t i0, u_t i1) const
    {
        u_t ft0 = i0 % 2;
        u_t ft1 = i1 % 2;
        i0 /= 2;
        i1 /= 2;
        u_t x0 = aminmax[i0][d][ft0];
        u_t x1 = aminmax[i1][d][ft1];
        bool lt = (x0 < x1) || ((x0 == x1) && (ft0 < ft1));
        return lt;
    }
 private:
    u_t d;
    const VMinMaxD<dim>& aminmax;
};

template <int dim>
void KD_SegTree<dim>::init_leaves(const vmm_t& aminmax)
{
    const u_t n_mm = aminmax.size();
    const u_t n_mm2 = 2*n_mm;
#if defined(DROP)
    vb_t drop(n_mm2, false);
    view_t view(drop, 0);
#else
    vb_t take(n_mm2, false);
    view_t view(take, 0);
#endif
    for (u_t d = 0; d < dim; ++d)
    {
        au2_t& bb = view.bbox[d];
        bb = aminmax[0][d];
        vu_t& lut = view.lut[d];
        // vu_t& lut_inv = view.lut_inv[d];
        lut.reserve(2*n_mm);
        for (u_t i = 0; i != n_mm; ++i)
        {
            minby(bb[0], aminmax[i][d][0]);
            maxby(bb[1], aminmax[i][d][1]);
        }
        for (u_t i = (bb[0] < bb[1] ? 0 : n_mm); i != n_mm; ++i)
        {
            if (bb[0] < aminmax[i][d][0])
            {
                lut.push_back(2*i + 0);
            }
            if (aminmax[i][d][1] < bb[1])
            {
                lut.push_back(2*i + 1);
            }
        }
        if (kd_debug) { cerr << "init_leaves: d="<<d << " sort("<<lut.size() <<
            ")\n"; }
#if 1
        sort(lut.begin(), lut.end(), MMComp<dim>(d, aminmax));
#else
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
#endif
    }
    view.xlut.reserve(n_mm2);
    for (u_t i = 0; i != n_mm2; ++i)
    {
        view.xlut.push_back(i);
    }
    if (kd_debug) { cerr << "init_leaves: call create_sub_tree\n"; }
    root = create_sub_tree(aminmax, view, 0);
    if (kd_debug) { cerr << "init_leaves: called create_sub_tree\n"; }
}

template <int dim>
KDSegTreeNode<dim>* KD_SegTree<dim>::create_sub_tree(
    const vmm_t& aminmax, const view_t& view, u_t depth)
{
    node_t* t = new node_t();
    t->bbox = view.bbox;
    const u_t d = depth % dim;
    const vu_t& lut = view.lut[d];
    u_t imed = u_t(-1), aimed = u_t(-1); // undef
    if (!view.lut[d].empty())
    {
        imed = lut.size()/2;
        aimed = lut[imed];
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
#if defined(DROP)
        view_t sub_view(view.drop);
#else
        view_t sub_view(view.take);
#endif
        sub_view.bbox = view.bbox;

#if defined(DROP)
        int i, ai;
        const int lut_sz = lut.size();

        // low
        const u_t low_max = xmed - (1 - zo); // if zo=0, xmed is high
        sub_view.bbox[d][1] = low_max;
        int imid = imed + zo;
        i = imid;
        while ((i > 0) && ((ai = lut[i - 1]) % 2 == 1) && 
            (aminmax[ai / 2][d][1] == xmed))
        {
           --i;
        }
        sub_view.lut[d] = vu_t(lut.begin(), lut.begin() + i);
        set_drop(lut.begin() + imid, lut.end(), aminmax, sub_view, 0, true);
        sub_view.set_lut_others(d, view);
        set_drop(lut.begin() + imid, lut.end(), aminmax, sub_view, 0, false);
        t->child[0] = create_sub_tree(aminmax, sub_view, depth + 1);
        
        // high
        const u_t high_min = low_max + 1;
        sub_view.bbox[d][0] = high_min;
        sub_view.bbox[d][1] = view.bbox[d][1];
        i = imid;
        while ((i < lut_sz) && ((ai = lut[i]) % 2 == 0) &&
            (aminmax[ai / 2][d][0] == high_min))
        {
            ++i;
        }
        sub_view.lut[d] = vu_t(lut.begin() + i, lut.end());
        set_drop(lut.begin(), lut.begin() + imid, aminmax, sub_view, 1, true);
        sub_view.set_lut_others(d, view);
        set_drop(lut.begin(), lut.begin() + imid, aminmax, sub_view, 1, false);
        t->child[1] = create_sub_tree(aminmax, sub_view, depth + 1);
#else
        // low
        sub_view.bbox[d][1] = xmed - (1 - zo); // if zo=0, xmed is high
        sub_view.set_lut(d, aminmax, view);
        sub_view.set_lut_others(d, view);
        t->child[0] = create_sub_tree(aminmax, sub_view, depth + 1);

        // high
        sub_view.bbox[d][0] = sub_view.bbox[d][1] + 1;
        sub_view.bbox[d][1] = view.bbox[d][1];
        sub_view.set_lut(d, aminmax, view);
        sub_view.set_lut_others(d, view);
        t->child[1] = create_sub_tree(aminmax, sub_view, depth + 1);
#endif
    }
    return t;
}

template <int dim>
void KD_SegTree<dim>::node_add_segment(KDSegTreeNode<dim>* t,
    const AU2D<dim>& seg)
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
                node_add_segment(child, seg);
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
int test(const VMinMaxD<dim>& leaves, const VD<dim>& pts, 
    const VMinMaxD<dim>& usegs=VMinMaxD<dim>())
{
    int rc = 0;
    // vau2_2_t a;
    KD_SegTree<dim> tree;
    cerr << "init_leaves\n";
    tree.init_leaves(leaves);
    tree.print();
    cerr << "adding segments\n";
    VMinMaxD<dim> segs_added(usegs);
    if (segs_added.empty())
    {
        u_t si = 0;
        for (const AU2D<dim>& seg: leaves) // just 2/3 ?
        {
            if (si % 3 != 2)
            {
                segs_added.push_back(seg);
            }
            ++si;
        }
    }
    for (const AU2D<dim>& seg: segs_added)
    {
        tree.add_segment(seg);
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
                "  " << dim << " l";
            for (const AU2D<dim>& seg: leaves)
            {
                for (u_t d = 0; d != dim; ++d)
                {
                    for (u_t zo: {0, 1})
                    {
                        cerr << ' ' << seg[d][zo];
                    }
                }
            }
            if (!usegs.empty())
            {
                cerr << " s";
                for (const AU2D<dim>& seg: usegs)
                {
                    for (u_t d = 0; d != dim; ++d)
                    {
                        for (u_t zo: {0, 1})
                        {
                            cerr << ' ' << seg[d][zo];
                        }
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
