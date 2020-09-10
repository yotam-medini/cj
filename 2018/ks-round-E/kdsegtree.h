#include <iostream>
#include <algorithm>
#include <array>
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
using AVUD = array<vu_t, dim>;

template <int dim>
using AU2D = array<au2_t, dim>; // Box = product of segments, [begin:end]

template <int dim>
using VDMinMax = vector<AUD2<dim>>;

template <int dim>
using VMinMaxD = vector<AU2D<dim>>;

class EndPoint
{
 public:
    EndPoint(u_t _p=0, u_t h=0) : p(_p), high(h) {}
    u_t p;
    bool high;
};
static bool operator==(const EndPoint& ep0, const EndPoint& ep1)
{
    bool eq = ((ep0.p == ep1.p) && (ep0.high == ep1.high));
    return eq;
}
static bool operator!=(const EndPoint& ep0, const EndPoint& ep1)
{
    return !(ep0 == ep1);
}
static bool operator<(const EndPoint& ep0, const EndPoint& ep1)
{
    bool lt = (ep0.p < ep1.p) || 
        ((ep0.p == ep1.p) && (int(ep0.high) < int(ep1.high)));
    return lt;
}
typedef vector<EndPoint> vep_t;
template <int dim>
using AVEPD = array<vep_t, dim>;

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
    node_t* create_sub_tree(VMinMaxD<dim>& aminmax,
        AVEPD<dim>& pts, AU2D<dim>& ft, AU2D<dim>& bbox, u_t depth);
    u_t get_cut_index(const vu_t& a, const au2_t& be, VMinMaxD<dim>& aminmax,
        u_t d) const;
    bool cut_push(VMinMaxD<dim>& a, const AU2D<dim> mm, const EndPoint& cut_ep,
        u_t d, bool low) const;
    bool intersect_push(VMinMaxD<dim>& a, const AU2D<dim> mm,
        const EndPoint& ep_low, const EndPoint& ep_high, u_t d) const;
    void node_add_segment(node_t* t, const AU2D<dim>& seg, const u_t depth);
    u_t node_pt_n_intersections(const node_t* t, const AUD<dim>& pt) const;
    node_t* root;
};

template <int dim>
void KD_SegTree<dim>::init_leaves(const VMinMaxD<dim>& aminmax)
{
    if (!aminmax.empty())
    {
        AU2D<dim> bbox = aminmax[0];
        // AVUD<dim> pts_all;
        AVEPD<dim> pts_all;
        for (u_t d = 0; d < dim; ++d)
        {
            pts_all[d].reserve(2*aminmax.size());
        }
        for (const AU2D<dim>& mm: aminmax)
        {
            for (u_t d = 0; d < dim; ++d)
            {
                pts_all[d].push_back(EndPoint(mm[d][0], false));
                pts_all[d].push_back(EndPoint(mm[d][1], true));
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
        // AVUD<dim> upts;
        AVEPD<dim> upts;
        AU2D<dim> from_to;
        for (u_t d = 0; d < dim; ++d)
        {
            sort(pts_all[d].begin(), pts_all[d].end());
            vep_t& uptsd = upts[d];
            for (const EndPoint& ep: pts_all[d])
            {
                if (uptsd.empty() || (uptsd.back() != ep))
                {
                    uptsd.push_back(ep);
                }
            }
            from_to[d] = au2_t{0, u_t(uptsd.size() - 1)};
        }
        VMinMaxD<dim> amm(aminmax);
        root = create_sub_tree(amm, upts, from_to, bbox, 0);
    }
}

template <int dim>
u_t KD_SegTree<dim>::get_cut_index(const vu_t& a, const au2_t& be,
    VMinMaxD<dim>& aminmax, u_t d) const
{
    const u_t mi = (be[0] + be[1])/2;
    u_t cut_index = mi;
    au2_t cv;
    u_t cut_value = cv[0] = a[mi];
    cv[1] = (mi + 1) < be[1] ? a[mi + 1] : cut_value;
    if (cv[0] + 1 < cv[1])
    {
        // change cut_value to next ?
        au2_t n_split{0, 0};
        for (const AU2D<dim>& mm: aminmax)
        {
            const au2_t& mmd = mm[d];
            for (u_t zo: {0, 1})
            {
                if ((mmd[0] <= cv[zo]) && (cv[zo] < mmd[1]))
                { 
                    ++n_split[zo];
                }
            }
        }
        if (n_split[0] > n_split[1])
        {
            ++cut_index; // mi + 1;
        }
    }
    return cut_index;
}

template <int dim>
bool
KD_SegTree<dim>::cut_push(VMinMaxD<dim>& a, const AU2D<dim> mm,
    const EndPoint& cut_ep, u_t d, bool low) const
{
    bool pushed = false;
    if (low)
    {
        u_t cut_value = cut_ep.p - (cut_ep.high ? 0 : 1);
        if ((pushed = (mm[d][0] <= cut_value)))
        {
            if (mm[d][1] <= cut_value)
            {
                a.push_back(mm);
            }
            else
            {
                AU2D<dim> cmm(mm);
                cmm[d][1] = cut_value;
                a.push_back(cmm);
            }
        }
    }
    else
    {
        u_t cut_value = cut_ep.p + (cut_ep.high ? 1 : 0);
        if ((pushed = (cut_value <= mm[d][1])))
        {
            if (cut_value <= mm[d][0])
            {
                a.push_back(mm);
            }
            else
            {
                AU2D<dim> cmm(mm);
                cmm[d][0] = cut_value;
                a.push_back(cmm);
            }
        }
    }
    return pushed;
}

template <int dim>
bool
KD_SegTree<dim>::intersect_push(VMinMaxD<dim>& a, const AU2D<dim> mm,
    const EndPoint& ep_low, const EndPoint& ep_high, u_t d) const
{
    bool pushed = false;
    u_t cut_low = ep_low.p + (ep_low.high ? 0 : 1);
    u_t cut_high = ep_high.p - (ep_high.high ? 0 : 1);
    if ((cut_low <= mm[d][1]) && (mm[d][0] <= cut_high))
    {
        if ((cut_low <= mm[d][0]) && (mm[d][1] <= cut_high))
        {
            a.push_back(mm);
        }
        else
        {
            AU2D<dim> cmm(mm);
            if (cmm[d][0] < cut_low)
            {
                cmm[d][0] = cut_low;
            }
            if (cmm[d][1] > cut_high)
            {
                cmm[d][1] = cut_high;
            }
            a.push_back(cmm);
        }
    }
    return pushed;
}

template <int dim>
KDSegTreeNode<dim>* KD_SegTree<dim>::create_sub_tree(VMinMaxD<dim>& aminmax,
   AVEPD<dim>& pts, AU2D<dim>& from_to, AU2D<dim>& bbox, u_t depth)
{
    const u_t d = depth % dim;
    node_t* t = new node_t();
    bool all_same = adjacent_find(aminmax.begin(), aminmax.end(),
        not_equal_to<AU2D<dim>>()) == aminmax.end();
    const au2_t ft_d_save = from_to[d];
    if (all_same) //  || (sz <= 2))
    {
        t->bbox = aminmax[0];
    }
    else
    {
        t->bbox = bbox;
        // u_t mi = get_cut_index(pts[d], be[d], aminmax, d);
        const bool cut = (ft_d_save[0] + 1 < ft_d_save[1]);
        const u_t mi = (ft_d_save[0] + ft_d_save[1])/2;
        const u_t pl = ft_d_save[0], ph = pl + 1;
        const EndPoint& cut_ep = pts[d][mi];
        const au2_t bbox_d_save = bbox[d];
        u_t n_child = 0;
        for (u_t lhi = 0; lhi != (cut ? 2 : 1); ++lhi)
        {
            const bool low = (lhi == 0);
            VMinMaxD<dim> a;
            for (const AU2D<dim> mm: aminmax)
            {
                bool pushed = (cut
                    ? cut_push(a, mm, cut_ep, d, low)
                    : intersect_push(a, mm, pts[d][pl], pts[d][ph], d));
                if (pushed)
                {
                     const au2_t& curr = a.back()[d];
                     if (a.size() == 1)
                     {
                         bbox[d] = curr;
                     }
                     else
                     {
                         if (bbox[d][0] > curr[0])
                         {
                             bbox[d][0] = curr[0];
                         }
                         if (bbox[d][1] < curr[1])
                         {
                             bbox[d][1] = curr[1];
                         }
                     }
                }
            }
            if (!a.empty())
            {
                u_t zo = int(low);
                const u_t ft_save = from_to[d][zo];
                from_to[d][zo] = mi;
                t->child[n_child] = create_sub_tree(a, pts, from_to, bbox,
                    depth + 1);
                from_to[d][zo] = ft_save;
                ++n_child;
            }
        }
        bbox[d] = bbox_d_save;
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
