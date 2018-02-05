// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <set>
#include <map>
#include <algorithm>
#include <vector>
#include <utility>

#include <cstdlib>
#include <gmpxx.h>

using namespace std;

// typedef mpz_class mpzc_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef mpz_class mpzc_t;
typedef mpq_class mpqc_t;
typedef vector<unsigned> vu_t;
typedef vector<double> vd_t;
typedef vector<mpqc_t> vq_t;

static unsigned dbg_flags;

class Point
{
 public:
    Point(mpqc_t vx=0, mpqc_t vy=0) : x(vx), y(vy) {}
    mpqc_t x, y;
    
};

class Segment
{
 public:
  Segment(mpqc_t vxb=0, mpqc_t vyb=0, mpqc_t va=1, mpqc_t vxe=1) :
      xb(vxb), yb(vxb), a(va), xe(vxe)
      {}
  static void intersect(Point &pt, const Segment &l0, const Segment &l1);
  bool intersects(Point &pt, const Segment &line);
  mpqc_t comp_ye() const { return a*(xe - xb) + yb; }
  string str () const;
  mpqc_t xb, yb;
  mpqc_t a; // 1/p
  mpqc_t xe;
};

string Segment::str() const
{
    ostringstream os;
    mpqc_t ye = comp_ye();
    os << "[(" << xb.get_d() << ", " << yb.get_d() << "), "
           "(" << xe.get_d() << ", " << ye.get_d() << ")]";
    return os.str();
}

void Segment::intersect(Point &pt, const Segment &l0, const Segment &l1)
{
    // assuming s0.a != s1.a
    // y = a0(x - xb0) + yb0 = a1(x - xb1) + yb1 
    // (a0 - a1)x = a1xb1 - a0xb0 + yb1 - yb0
    mpqc_t numer = l1.a * l1.xb - l0.a * l0.xb + l1.yb + l0.yb;
    mpqc_t denom = l0.a - l1.a;
    pt.x = numer / denom;
    pt.y = l0.a * (pt.x - l0.xb) + l0.yb;
}

bool Segment::intersects(Point &pt, const Segment &line)
{
    intersect(pt, *this, line);
    bool ret = (xb <= pt.x) && (pt.x <= xe);
    return ret;
}

typedef set<Segment> setseg_t;
typedef map<unsigned, Segment, greater<unsigned> > u2seg_t;
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
    double solve_dim(unsigned di) const;
    mpqc_t eval(const mpqc_t *pt) const;
    mpqc_t deval(unsigned di, const mpqc_t x) const;
    void dim_lut_sorted(unsigned di, vu_t &lut) const;
    bool dim_lt(unsigned di, const unsigned &i0, const unsigned& i1) const
    {
        const vq_t &v = xyz_ships[di];
        return v[i0] < v[i1];
    }
    void build(unsigned dim, vseg_t &poly) const;
    void poly_update(
        vseg_t &poly, Segment &line, const Point&, vseg_t::iterator) const;
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
        dp[i] = mpqc_t(1, p[i]);
    }
}

void Starwars::solve_naive()
{
    unsigned n = n_ships();
    mpqc_t pt[3]{0, 0,  0};
    for (unsigned di = 0; di < 3; ++di)
    {
        mpqc_t minmax_power = -1;
        vu_t lut;
        const vq_t &xs = xyz_ships[di];
        pt[di] = xs[0];
        dim_lut_sorted(di, lut);
        for (unsigned il = 0; il < n; ++il)
        {
            unsigned i0 = lut[il];
            //   a(x-x0)
            const mpqc_t &a0 = dp[i0];
            mpqc_t b0 = -a0 * xs[i0];
            for (unsigned ir = il + 1; ir < n; ++ir)
            {
                unsigned i1 = lut[ir];
                mpqc_t a1 = -dp[i1];
                mpqc_t b1 = -a1 * xs[i1];
                // intersect:  a0*x + b0 = x1*x + b1
                mpqc_t x = (b1 - b0) / (a0 - a1);
                mpqc_t xpower = deval(di, x);
                if ((minmax_power < 0) || minmax_power > xpower)
                {
                    minmax_power = xpower;
                    pt[di] = x;
                }
            }
        }
    }
    mpqc_t qsolution = eval(pt);
    solution = qsolution.get_d();
}

void Starwars::solve()
{
    mpqc_t pt[3]{0, 0, 0};
    for (unsigned di = 0; di < 3; ++di)
    {
        pt[di] = solve_dim(di);
    }
    mpqc_t qsolution = eval(pt);
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


double Starwars::solve_dim(unsigned di) const
{
    vseg_t pls;
    build(di, pls);
    return 0;
}

void Starwars::build(unsigned di, vseg_t& poly) const
{
    const vq_t &xs = xyz_ships[di];
    const unsigned n = xs.size();
    vu_t lut;
    dim_lut_sorted(di, lut);
    unsigned i0 = lut[0];
    unsigned iz = lut[n - 1];
    mpqc_t xe = xs[iz];
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
                    if (seg.intersects(psect, line))
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

mpqc_t Starwars::eval(const mpqc_t *pt) const
{
    mpqc_t max_power = 0;
    for (unsigned i = 0, n = n_ships(); i < n; ++i)
    {
        mpqc_t power = 0;
        for (unsigned di = 0; di < 3; ++di)
        {
            mpqc_t delta = xyz_ships[di][i] - pt[di];
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

mpqc_t Starwars::deval(unsigned di, const mpqc_t x) const
{
    mpqc_t max_power = 0;
    const vq_t &xs = xyz_ships[di];
    for (unsigned i = 0, n = n_ships(); i < n; ++i)
    {
        mpqc_t delta = xs[i] - x;
        mpqc_t power = dp[i] * (delta > 0 ? delta : -delta);
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
