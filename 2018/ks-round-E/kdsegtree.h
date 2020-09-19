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
    // AVUD<dim> lut_inv; // inverse of lut
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
    void init_leaves_New(const vmm_t& aminmax);
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
    bool are_all_same(const VMinMaxD<dim>& aminmax, const view_t& view) const;
    node_t* create_sub_tree(VMinMaxD<dim>& aminmax,
        AVEPD<dim>& pts, AU2D<dim>& ft, AU2D<dim>& bbox, u_t depth);
    node_t* create_sub_tree(const vmm_t& aminmax, const view_t& view, u_t depth);
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
void KD_SegTree<dim>::init_leaves_New(const vmm_t& aminmax)
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
bool KD_SegTree<dim>::are_all_same(
    const VMinMaxD<dim>& aminmax, const view_t& view) const
{
    bool all_same = true;
    const u_t n_pts = view.lut[0].size(); // all lut-s are same size
    bool picked[2] = {false, false};
    AUD<dim> pick[2]; //  = aminmax[imed];
    if (n_pts > 0)
    {
        const u_t imed = view.lut[0][n_pts / 2];
        const u_t zo = imed % 2;
        box_to_pt<dim>(pick[zo], aminmax[imed / 2], zo);
        picked[zo] = true;
    }
    for (u_t li = 0; all_same && (li != n_pts); ++li)
    {
        const u_t i = view.lut[0][li];
        const u_t zo = i % 2;
        AUD<dim> pt;
        box_to_pt<dim>(pt, aminmax[i / 2], zo);
        if (picked[zo])
        {
            all_same = (pick[zo] == pt);
        }
        else
        {
            pick[zo] = pt;
            picked[zo] = true;
        }
    }
    return all_same;
}

template <int dim>
KDSegTreeNode<dim>* KD_SegTree<dim>::create_sub_tree(
    const vmm_t& aminmax, const view_t& view, u_t depth)
{
    node_t* t = new node_t();
    t->bbox = view.bbox;
    bool all_same = are_all_same(aminmax, view);
    if (!all_same)
    {
        // const u_t n_pts = view.lut[0].size(); // all lut-s are same size
        const u_t d = depth % dim;
        const vu_t& lut = view.lut[d];
        vu_t::const_iterator lb, ub; // of the bbox !!!!!!!!!!!!!
        lb = upper_bound(lut.begin(), lut.end(), view.bbox[d][0],
            [aminmax, d](u_t xv, u_t i) -> bool
            {
                const u_t zo = i % 2;
                const u_t v = aminmax[i/2][d][zo];
                bool lt = (xv < v) || ((xv == v) && (zo == 1));
                // cerr << "yotam: xv="<<xv << ", i="<<i << ", v="<<v << 
                //    ", lt="<<lt<<'\n';
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
        cerr << "lbi="<<lbi << ", ubi="<<ubi << '\n';
        if (lbi > ubi)
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
            // u_t imed = n_pts/2;
            u_t imed = (lbi + ubi) / 2;
            const u_t aimed = lut[imed];
            // const u_t xmed = aminmax[aimed / 2][d][aimed % 2] + (aimed % 2);
            u_t xmed = aminmax[aimed / 2][d][aimed % 2];
            view_t sub_view(view.take);
            sub_view.bbox = view.bbox;
            u_t ci = 0;

            // low
            if ((aimed % 2 == 0) && (xmed > sub_view.bbox[d][0]))
            {
                --xmed;
            }
            sub_view.bbox[d][1] = xmed;
            if (sub_view.bbox[d][1] == view.bbox[d][1])
            {
                sub_view.age = view.age + 1;
            }
            sub_view.set_lut(d, aminmax, lut);
            sub_view.set_lut_others(d, view);
            t->child[ci++] = create_sub_tree(aminmax, sub_view, depth + 1);

            // high
            sub_view.bbox[d][0] = xmed + 1;
            sub_view.bbox[d][1] = view.bbox[d][1];
            if (view.bbox[d][0] <= view.bbox[d][1])
            {
                sub_view.set_lut(d, aminmax, lut);
                sub_view.set_lut_others(d, view);
                t->child[ci++] = create_sub_tree(aminmax, sub_view, depth + 1);
            }
        }
    }
    return t;

}

template <int dim>
void KD_SegTree<dim>::init_leaves(const VMinMaxD<dim>& aminmax)
{
 if (true) { init_leaves_New(aminmax); return;}
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
        sort(amm.begin(), amm.end());
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
    u_t cut_low = ep_low.p + (!ep_low.high ? 0 : 1);
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
        pushed = true;
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
