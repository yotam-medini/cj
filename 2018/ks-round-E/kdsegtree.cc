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
using VD = vector<array<u_t, dim>>;

template <int dim>
using AUD = array<array<u_t, dim>, 2>;

template <int dim>
using VDMinMax = vector<AUD<dim>>;

template <int dim>
class LessAU
{
 public:
    LessAU(u_t _d) : d(_d) {}
    bool operator()(const array<u_t, dim>& x0, const array<u_t, dim>& x1) const
    {
        bool lt = x0[d] < x1[d];
        return lt;
    }
 private:
    u_t d;
};

// if a[i][d] constant then  im=ie
template <int dim>
u_t bisect(VD<dim>& a, au2_2_t& lhbe, u_t d, u_t ib, u_t ie)
{
    typedef array<u_t, dim> audim_t;
    u_t im = (ib + ie + 1)/2; // we want low noy to be smaller 
    typename VD<dim>::iterator b = a.begin();
    const LessAU<dim> ltd(d);
    nth_element(b + ib, b + im, b + ie, ltd);
    u_t dmin = (*min_element(b + ib, b + im, ltd))[d];
    u_t l_max = (*max_element(b + ib, b + im, ltd))[d];
    u_t h_min = (*min_element(b + im, b + ie, ltd))[d];
    u_t dmax = (*max_element(b + im, b + ie, ltd))[d];
    lhbe[0][0] = dmin;
    lhbe[0][1] = l_max;
    lhbe[1][0] = h_min;
    lhbe[1][1] = dmax;
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
            lhbe[0][1] = (ib < im ? (*max_element(b + ib, b + im, ltd))[d] :
                dmin);
        }
        else
        {
            lhbe[1][0] = (im < ie ? (*min_element(b + im, b + ie, ltd))[d] :
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
        lhbe{ {{0, 0}, {0, 0}} }, 
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
        os << indent << "  LHBE: ["  <<
            lhbe[0][0] << ", " << lhbe[0][1] << ") [" <<
            lhbe[1][0] << ", " << lhbe[1][1] << ")\n";
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
    au2_2_t lhbe; // [low, high][begin, end]
    KDSegTreeNode* child[2];
};

template <int dim>
class KDSegTreeLeaf: public KDSegTreeNode
{
 public:
    KDSegTreeLeaf(u_t _c=0, const array<u_t, dim>& _x={0, }) :
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
    array<u_t, dim> pt;
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
        u_t im = bisect<dim>(a, tint->lhbe, d, ib, ie);
        tint->child[0] = create_sub_kds_tree<dim>(a, depth + 1, ib, im);
        if (im < ie)
        {
            tint->child[1] = create_sub_kds_tree<dim>(a, depth + 1, im, ie);
        }
    }
    return t;
}

template <int dim>
KDSegTreeNode* create_kd_seg_tree(const VDMinMax<dim>& aminmax)
{
    VD<dim> aflat;
    aflat.reserve(2*aminmax.size());
    for (const array<array<u_t, dim>, 2>& mm: aminmax)
    {
        aflat.push_back(mm[0]);
        aflat.push_back(mm[1]);
    }
    // remove duplications
    sort(aflat.begin(), aflat.end());
    VD<dim> uflat;
    uflat.reserve(aflat.size());
    for (const array<u_t, dim>& pt: aflat)
    {
        if (uflat.empty() || (uflat.back() != pt))
        {
            uflat.push_back(pt);
        }
    }
    KDSegTreeNode* t = aflat.empty() ? 0 :
        create_sub_kds_tree<dim>(uflat, 0, 0, uflat.size());
    return t;
}

template <int dim>
void kd_seg_tree_add_segment(KDSegTreeNode* t, const AUD<dim>& seg)
{
}

template <int dim>
int test(const VDMinMax<dim>& bes, const VD<dim>& pts)
{
    int rc = 0;
    // vau2_2_t a;
    KDSegTreeNode* t = create_kd_seg_tree<dim>(bes);
    cout << "t="<<t<<'\n';
    if (t) { t->print(cout, 0); }
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
        VDMinMax<dim> bes;
        while (string(argv[ai]) != string("p"))
        {
            array<array<u_t, dim>, 2> be;
            for (u_t bei = 0; bei != 2; ++bei)
            {
                for (u_t i = 0; i != dim; ++i, ++ai)
                {
                    be[bei][i] = stoi(argv[ai]);
                }
            }
            bes.push_back(be);
        }
        ++ai; // sskip "p"
        VD<dim> pts;
        while (ai < argc)
        {
            array<u_t, dim> pt;
            for (u_t i = 0; i != dim; ++i, ++ai)
            {
                pt[i] = stoi(argv[ai]);
            }
            pts.push_back(pt);
        }
        rc = test<dim>(bes, pts);
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
