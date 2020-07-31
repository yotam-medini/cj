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
bool lt(const AUD<dim>& x0, const AUD<dim>& x1)
{
    bool ret = true;
    for (int i = 0; ret && (i < dim); ++i)
    {
        ret = (x0[i] < x1[i]);
    }
    return ret;
}

template <int dim>
bool lte(const AUD<dim>& x0, const AUD<dim>& x1)
{
    bool ret = true;
    for (int i = 0; ret && (i < dim); ++i)
    {
        ret = (x0[i] <= x1[i]);
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


// if a[i][d] constant then  im=ie
template <int dim>
u_t bisect(VD<dim>& a, au2_2_t& lh_seg, u_t d, u_t ib, u_t ie)
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

// K=2
class KDSegTreeNode
{
 public:
    KDSegTreeNode(u_t _c=0) : count(_c) {}
    virtual ~KDSegTreeNode() {}
    virtual bool leaf() const { return false; }
    virtual void print(ostream& os, u_t depth) const = 0;
    u_t count;
};

class KDSegTreeInternal: public KDSegTreeNode
{
 public:
    KDSegTreeInternal(u_t _c=0) : 
        KDSegTreeNode(_c), 
        lh_seg{ {{0, 0}, {0, 0}} }, 
        child{0, 0} {}
    virtual ~KDSegTreeInternal() 
    {
        delete child[0];
        delete child[1];
    }
    void print(ostream& os, u_t depth) const
    {
        const string indent(2*depth, ' ');
        os << indent << "{\n";
        os << indent << "  LH_SEG: ["  <<
            lh_seg[0][0] << ", " << lh_seg[0][1] << "] [" <<
            lh_seg[1][0] << ", " << lh_seg[1][1] << "]\n";
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
        os << indent << "}\n";
        
    }
    au2_2_t lh_seg; // [low, high][min, max]
    KDSegTreeNode* child[2];
};

template <int dim>
class KDSegTreeLeaf: public KDSegTreeNode
{
 public:
    KDSegTreeLeaf(u_t _c=0, const AUD<dim>& _x={0, }) :
        KDSegTreeNode(_c), pt{_x} {}
    virtual bool leaf() const { return true; }
    void print(ostream& os, u_t depth) const
    {
        const string indent(2*depth, ' ');
        os << indent << "[";
        const char* sep = "";
        for (u_t x: pt)
        {
            os << sep << x; sep = ", ";
        }
        os << "]\n";
    }
    AUD<dim> pt;
};

template <int dim>
KDSegTreeNode* create_sub_kds_tree(VD<dim>& a, u_t depth, u_t ib, u_t ie)
{
    const u_t d = depth % dim;
    const u_t sz = ie - ib;
    KDSegTreeNode* t = nullptr;
    if (sz == 1)
    {
        t = new KDSegTreeLeaf<dim>(0, a[ib]);
    }
    else
    {
        KDSegTreeInternal* tint = new KDSegTreeInternal;
        t = tint;
        u_t im = bisect<dim>(a, tint->lh_seg, d, ib, ie);
        tint->child[0] = create_sub_kds_tree<dim>(a, depth + 1, ib, im);
        if (im < ie)
        {
            tint->child[1] = create_sub_kds_tree<dim>(a, depth + 1, im, ie);
        }
    }
    return t;
}

template <int dim>
class KD_SegTree
{
 public:
    KD_SegTree() : root(0), root_bbox{{0, 0}} {}
    virtual ~KD_SegTree() { delete root; }
    void init_leaves(const VMinMaxD<dim>& aminmax);
    void add_segment(const AU2D<dim>& seg)
    {
        node_add_segment(root, seg, root_bbox, 0);
    }
    void print(ostream& os=cerr) const;
 private:
    KDSegTreeNode* create_sub_tree(VD<dim>& a, u_t depth, u_t ib, u_t ie);
    void node_add_segment(KDSegTreeNode* t, const AU2D<dim>& seg, 
        AU2D<dim>& bbox, const u_t depth);
    KDSegTreeNode* root;
    AU2D<dim> root_bbox;
};

template <int dim>
void KD_SegTree<dim>::init_leaves(const VMinMaxD<dim>& aminmax)
{
    if (!aminmax.empty())
    {
        VMinMaxD<dim> as(aminmax)
        sort(au.begin(), au.end());
        VMinMaxD<dim> au;
        au.reserve(au.size());
        bbox = as[0];
        au.push_back(as[0]);
        for (const AU2D<dim>& mm: as)
        {
            if (as != au.back())
            {
                as.push_back(mm);
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
    }
#if 0
    VD<dim> aflat;
    aflat.reserve(2*aminmax.size());
    for (const AU2D<dim>& mm: aminmax)
    {
        AUD<dim> m;
        for (u_t i = 0; i != 2; ++i)
        {
            for (u_t d = 0; d != dim; ++d)
            {
                m[i] = mm[d][i];
            }
            aflat.push_back(m);
        }
    }
    // remove duplications
    sort(aflat.begin(), aflat.end());
    VD<dim> uflat;
    uflat.reserve(aflat.size());
    for (const AUD<dim>& pt: aflat)
    {
        if (uflat.empty() || (uflat.back() != pt))
        {
            uflat.push_back(pt);
        }
    }
    if (!aflat.empty())
    {
        bbox = aminmax[0];
        for 
        create_sub_kds_tree<dim>(uflat, 0, 0, uflat.size());
    }
#endif
}

template <int dim>
void KD_SegTree<dim>::node_add_segment(KDSegTreeNode* t, const AU2D<dim>& seg,
    AU2D<dim>& bbox, u_t depth)
{
    KDSegTreeInternal* tint = dynamic_cast<KDSegTreeInternal*>(t);
    if (tint)
    {
        // u_t const d = depth % dim;
        bool lte_l = lte01<dim>(seg, bbox, 0);
        bool lte_h = lte01<dim>(bbox, seg, 1);
        if (lte_l && lte_h)
        {
            ++(tint->count);
        }
        // bool lt_l = tint->lh_seg[0] < seg[d][1];
        // bool lt_h = seg[d][0] < tint->lh_seg[1];
        bool lt_l = false, lt_h = false; // TEMPORARY !!!!!!!!!!!
        if (lt_l || lt_h)
        {
            for (u_t ci = 0; ci != 2; ++ci)
            {
                node_add_segment(tint->child[ci], seg, bbox, depth + 1);
            }
        }
    }
}

template <int dim>
void KD_SegTree<dim>::print(ostream& os) const
{
    os << "{ KD_SegTree<" << dim << ">\n";
    os << "}\n";
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
    for (const AU2D<dim>& seg: segs) // may just 2/3 ?
    {
        tree.add_segment(seg);
    }
    tree.print();
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

static void usage(const string& prog)
{
    const string indent(prog.size() + 2, ' ');
    cerr << prog << " # ....\n" <<
        indent << "<dim> s <b1> <e1> <b2> <e2> ... p <x1> <x2> ...\n";
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
        rc = 1;
    }
    else
    {
        rc = dim_specific_main(argc - 1, argv + 1);
    }
    return rc;
}
