// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
// #include <set>
#include <algorithm>
#include <vector>
#include <utility>

#include <cstdlib>
#include <gmpxx.h>

using namespace std;

// typedef mpz_class mpzc_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
// typedef mpz_class mpzc_t;
// typedef mpq_class mpqc_t;
typedef vector<unsigned> vu_t;
typedef vector<double> vd_t;
typedef vector<mpq_class> vq_t;

static unsigned dbg_flags;

static string qptrs(const mpq_class &q, const char *label="")
{
    ostringstream os;
    const char *p = (const char *)(&q);
    const void **ppn = (const void **)(p + 8);
    const void **ppd = (const void **)(p + 24);
    const void *pn = *ppn;
    const void *pd = *ppd;
    
    os << label << " ptrs: (" << 
        // (void*)(q.mp[0]._mp_num._mp_d) << ", " <<
        // (void*)(q.mp[0]._mp_den._mp_d) << 
        pn << ", " << pd << 
        ")";
    return os.str();
}

class Point
{
 public:
    Point(mpq_class vx=0, mpq_class vy=0) : x(vx), y(vy) {}
    mpq_class x, y;
    string str() const;    
};

string Point::str() const
{
    ostringstream os;
    os << "[" << x.get_d() << ", " << y.get_d() << "]";
    return os.str();
}

class Segment
{
 public:
    Segment(
        const mpq_class &vxb=0, 
        const mpq_class &vyb=0, 
        const mpq_class &va=1, 
        const mpq_class &vxe=1) :
        xb(vxb), yb(vyb), a(va), xe(vxe)
        {}
    static void intersect(Point &pt, const Segment &l0, const Segment &l1);
    bool intersects_line(Point &pt, const Segment &line) const;
    bool intersects_segment(Point &pt, const Segment &seg) const;
    bool intersects(Point &pt, const Segment &seg) const;
    mpq_class comp_y(const mpq_class& x) const { return a*(x - xb) + yb; }
    mpq_class comp_ye() const { return comp_y(xe); }
    bool xinside(const mpq_class& x) const;
    string str() const;
    mpq_class xb, yb;
    mpq_class a; // 1/p
    mpq_class xe;
};

string Segment::str() const
{
    ostringstream os;
    mpq_class ye = comp_ye();
    os << "[(" << xb.get_d() << ", " << yb.get_d() << "), "
           "(" << xe.get_d() << ", " << ye.get_d() << ")]";
    return os.str();
}

void Segment::intersect(Point &pt, const Segment &l0, const Segment &l1)
{
    // assuming s0.a != s1.a
    // y = a0(x - xb0) + yb0 = a1(x - xb1) + yb1 
    // (a0 - a1)x = a0xb0 - a1xb1 + yb1 - yb0
#if 0
    mpq_class numer = l0.a * l0.xb - l1.a * l1.xb + l1.yb + l0.yb;
#else
    cerr << qptrs(l1.a, "l1.a") << "\n";
    mpq_class a0(l0.a);
    mpq_class xb0(l0.xb);
    mpq_class a1(l1.a);
    mpq_class xb1(l1.xb);
    mpq_class yb1(l1.yb);
    mpq_class yb0(l0.yb);
    mpq_class numer = a0 * xb0 - a1 * xb1 + yb1 + yb0;
#endif
    mpq_class denom = l0.a - l1.a;
    pt.x = numer / denom;
    pt.y = l0.a * (pt.x - l0.xb) + l0.yb;
    if (dbg_flags & 0x4) {
        cerr << "intersect(" << l0.str() << ", " << l1.str() << "): " << 
            pt.str() << "\n";
    }
}

// assuming xb <= xe
bool Segment::intersects_line(Point &pt, const Segment &line) const
{
    intersect(pt, *this, line);
    bool ret = (xb <= pt.x) && (pt.x <= xe);
    return ret;
}

// assuming xb <= xe
bool Segment::intersects(Point &pt, const Segment &seg) const
{
    intersect(pt, *this, seg);
    bool ret = xinside(pt.x) && seg.xinside(pt.x);
    return ret;
}

bool Segment::xinside(const mpq_class& x) const
{
    bool ret = (x == xb) || (x == xe) || ((xb < x) == (x < xe));
    return ret;
}

typedef vector<Segment> vseg_t;

class Starwars
{
 public:
    Starwars(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    unsigned n_ships() const { return p.size(); }
    mpq_class solve_dim(unsigned di) const;
    mpq_class eval(const mpq_class *pt) const;
    mpq_class deval(unsigned di, const mpq_class x) const;
    void lut_xsorted(vu_t &lut, const vq_t& xs) const;
    void dim_lut_sorted(unsigned di, vu_t &lut) const;
    bool dim_lt(unsigned di, const unsigned &i0, const unsigned& i1) const
    {
        const vq_t &v = xyz_ships[di];
        return v[i0] < v[i1];
    }
    void build(unsigned dim, vseg_t &poly_inc, vseg_t &poly_dec) const;
    void build_inc(unsigned dim, vseg_t &poly, const vq_t& xs) const;
    void poly_update(
        vseg_t &poly, Segment &line, const Point&, vseg_t::iterator) const;
    ostream& ovq(ostream&, const vq_t&, const char* label="") const;
    vq_t xyz_ships[3];
    vu_t p;
    vq_t dp;
    double solution;
};

Starwars::Starwars(istream& fi) :
    solution(0)
{
    unsigned n;
    fi >> n;
    for (unsigned di = 0; di < 3; ++di)
    {
        xyz_ships[di] = vq_t(vd_t::size_type(n), 0);
    }
    p = vu_t(vd_t::size_type(n), 0.);
    dp = vq_t(vd_t::size_type(n), 0);
    for (unsigned i = 0; i < n; ++i)
    {
        
        fi >> xyz_ships[0][i] >> xyz_ships[1][i] >> xyz_ships[2][i] >> p[i];
        dp[i] = mpq_class(1, p[i]);
    }
}

void Starwars::solve_naive()
{
    unsigned n = n_ships();
    mpq_class pt[3]{0, 0,  0};
    for (unsigned di = 0; di < 3; ++di)
    {
        mpq_class minmax_power = -1;
        vu_t lut;
        const vq_t &xs = xyz_ships[di];
        pt[di] = xs[0];
        dim_lut_sorted(di, lut);
        for (unsigned il = 0; il < n; ++il)
        {
            unsigned i0 = lut[il];
            //   a(x-x0)
            const mpq_class &a0 = dp[i0];
            mpq_class b0 = -a0 * xs[i0];
            for (unsigned ir = il + 1; ir < n; ++ir)
            {
                unsigned i1 = lut[ir];
                mpq_class a1 = -dp[i1];
                mpq_class b1 = -a1 * xs[i1];
                // intersect:  a0*x + b0 = x1*x + b1
                mpq_class x = (b1 - b0) / (a0 - a1);
                mpq_class xpower = deval(di, x);
                if ((minmax_power < 0) || minmax_power > xpower)
                {
                    minmax_power = xpower;
                    pt[di] = x;
                }
            }
        }
    }
    mpq_class qsolution = eval(pt);
    solution = qsolution.get_d();
}

void Starwars::solve()
{
    mpq_class pt[3]{0, 0, 0};
    for (unsigned di = 0; di < 3; ++di)
    {
        pt[di] = (n_ships() > 1 ? solve_dim(di) : xyz_ships[di][0]);
    }
    mpq_class qsolution = eval(pt);
    solution = qsolution.get_d();
}

static void iota(vu_t& v, unsigned n, unsigned val=0)
{
    v.clear();
    v.reserve(n);
    for (unsigned k = 0; k < n; ++k, val++)
    {
        v.push_back(val);
    }
}

void Starwars::dim_lut_sorted(unsigned di, vu_t &lut) const
{
    using namespace::placeholders;
    unsigned n = n_ships();
    iota(lut, n);
    sort(lut.begin(), lut.end(), bind(&Starwars::dim_lt, *this, di, _1, _2));
}

void Starwars::lut_xsorted(vu_t &lut, const vq_t& xs) const
{
    iota(lut, xs.size());
    sort(lut.begin(), lut.end(),
        [&xs](const unsigned& i0, const unsigned& i1)
        { return xs[i0] < xs[i1]; });
}

mpq_class Starwars::solve_dim(unsigned di) const
{
    mpq_class ret;
    vseg_t poly_inc, poly_dec;
    build(di, poly_inc, poly_dec);
    // Linear search = good enough now ?
    vseg_t::const_iterator iinc = poly_inc.begin();
    vseg_t::const_reverse_iterator idec = poly_dec.rbegin();
    bool crossed = false;
    while (!crossed)
    {
        Point pt;
        const Segment &seg_inc = *iinc;
        const Segment &seg_dec = *idec;
        crossed = seg_inc.intersects(pt, seg_dec);
        if (crossed)
        {
            if (dbg_flags & 0x10) { cerr << "pt="<<pt.str()<<"\n"; }
            ret = pt.y;
        }
        else
        {
            if (seg_inc.xe < seg_dec.xb)
            {
                ++iinc;
            }
            else
            {
                ++idec;
            }
        }
    }
    return ret;
}

void Starwars::build(unsigned di, vseg_t& poly_inc, vseg_t &poly_dec) const
{
    const vq_t &xs = xyz_ships[di];
    vq_t xs_mirror(xs.size(), 0);
    // transform(xs.begin(), xs.end(), xs_mirror.begin(), negate<mpq_class>());
    for (unsigned i = 0; i < xs.size(); ++i) 
    {
        xs_mirror[i] = -xs[i];
    }
    if (dbg_flags & 0x8) {
        ovq(cerr, xs, "xs") << "\n";
        ovq(cerr, xs_mirror, "xs_mirror") << "\n";
    }
    build_inc(di, poly_dec, xs_mirror);
    for (auto i = poly_dec.begin(), e = poly_dec.end(); i != e; ++i)
    {
        Segment &seg = *i;
cerr << "before: seg=" << seg.str() << "\n";
        seg.xb = -seg.xb;
        seg.a = -seg.a;
        seg.xe = -seg.xe;
cerr << "after: seg=" << seg.str() << "\n";
    }
    build_inc(di, poly_inc, xs);
}

void Starwars::build_inc(unsigned di, vseg_t& poly, const vq_t& xs) const
{
    const unsigned n = xs.size();
    vu_t lut;
    lut_xsorted(lut, xs);
    unsigned i0 = lut[0];
    unsigned iz = lut[n - 1];
    mpq_class xe = xs[iz];
    Segment seg0(xs[i0], 0, dp[i0], xe);
    unsigned plast = p[i0];
    poly.push_back(seg0);
    for (unsigned i = 1; i < n; ++i)
    {
        unsigned luti = lut[i];
        unsigned pcurr = p[luti];
        if (pcurr > plast)
        { // find the first if any segment that intersect next line.
            Segment line(xs[luti], 0, dp[luti], xe);
            unsigned ihigh = poly.size() - 1;
            if (line.comp_ye() >= poly[ihigh].comp_ye())
            {
                Point psect;
                unsigned ilow = 0;
                while (ilow < ihigh)
                {
                    unsigned imid = (ilow + ihigh) / 2;
                    Segment &seg = poly[imid];
                    if (seg.intersects_line(psect, line))
                    {
                        ilow = ihigh = imid;
                    }
                    else if (psect.y < seg.yb)
                    {
                        ilow = imid + 1;
                    }
                    else
                    {
                        ihigh = imid - 1;
                    }
                }
                poly_update(poly, line, psect, poly.begin() + ilow);
            }
        }
    }
    if (dbg_flags & 0x2) {
        cerr << "Poly[" << poly.size() << "]:\n";
        for (auto i = poly.begin(), e = poly.end(); i != e; ++i) {
            cerr << "  " << i->str() << "\n";
        }
    }
}

void Starwars::poly_update(
    vseg_t &poly, 
    Segment &line, 
    const Point &pt, 
    vseg_t::iterator at) const
{
    Segment &segsect = *at;
    if ((pt.x == segsect.xb) || (pt.x == segsect.xe))
    {
        if (pt.x == segsect.xe)
        {
            ++at;
        }
        poly.erase(at, poly.end());
    }
    else
    {
        segsect.xe = pt.x;
        poly.erase(++at, poly.end());
        
    }
    line.xb = pt.x;
    line.yb = pt.y;
    poly.push_back(line);
}

mpq_class Starwars::eval(const mpq_class *pt) const
{
    mpq_class max_power = 0;
    for (unsigned i = 0, n = n_ships(); i < n; ++i)
    {
        mpq_class power = 0;
        for (unsigned di = 0; di < 3; ++di)
        {
            mpq_class delta = xyz_ships[di][i] - pt[di];
            power += (delta > 0 ? delta : -delta);
        }
        power *= dp[i];
        if (dbg_flags & 0x1) { cerr << "i="<<i << ", power="<<power << "\n"; }
        if (max_power < power)
        {
           max_power = power;
        }
    }
    if (dbg_flags & 0x1) {
        cerr << "eval(" << pt[0] << ", " << pt[1] << ", " << pt[2] << ")=" <<
            max_power << "\n";
    }
    return max_power;
}

mpq_class Starwars::deval(unsigned di, const mpq_class x) const
{
    mpq_class max_power = 0;
    const vq_t &xs = xyz_ships[di];
    for (unsigned i = 0, n = n_ships(); i < n; ++i)
    {
        mpq_class delta = xs[i] - x;
        mpq_class power = dp[i] * (delta > 0 ? delta : -delta);
        if (max_power < power)
        {
           max_power = power;
        }
    }
    return max_power;
}

void Starwars::print_solution(ostream &fo) const
{
    fo << ' ' << fixed << setprecision(6) << solution;
}

ostream& Starwars::ovq(ostream& os, const vq_t &v, const char* label) const
{
    os << label << "[";
    const char *sep = "";
    for (auto i = v.begin(), e = v.end(); i != e; ++i) 
    {
        os << sep << *i;
        sep = ", ";
    }
    os << "]";
    return os;
}

int main(int argc, char ** argv)
{
    const string dash("-");

    bool naive = false;
    bool tellg = false;
    int rc = 0, ai;

    for (ai = 1; (rc == 0) && (argv[ai][0] == '-') && argv[ai][1] != '\0'; ++ai)
    {
        const string opt(argv[ai]);
        if (opt == string("-naive"))
        {
            naive = true;
        }
        else if (opt == string("-debug"))
        {
            dbg_flags = strtoul(argv[++ai], 0, 0);
        }
        else if (opt == string("-tellg"))
        {
            tellg = true;
        }
    }
    
    int ai_in = ai;
    int ai_out = ai + 1;
    istream *pfi = (argc <= ai_in || (string(argv[ai_in]) == dash))
         ? &cin
         : new ifstream(argv[ai_in]);
    ostream *pfo = (argc <= ai_out || (string(argv[ai_out]) == dash))
         ? &cout
         : new ofstream(argv[ai_out]);

    if ((!pfi) || (!pfo) || pfi->fail() || pfo->fail())
    {
        cerr << "Open file error\n";
        exit(1);
    }

    unsigned n_cases;
    *pfi >> n_cases;

    void (Starwars::*psolve)() =
        (naive ? &Starwars::solve_naive : &Starwars::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Starwars starwars(*pfi);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << ", " << fpos << ")\n";
            fpos_last = fpos;
        }

        (starwars.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        starwars.print_solution(fout);
        fout << "\n";
        fout.flush();

    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
