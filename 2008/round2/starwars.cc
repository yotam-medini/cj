// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
// #include <set>
// #include <map>
#include <algorithm>
#include <vector>
#include <utility>

#include <cstdlib>
// #include <gmpxx.h>

using namespace std;

// typedef mpz_class mpzc_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<unsigned> vu_t;
typedef vector<double> vd_t;

static unsigned dbg_flags;

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
    double eval(const double *pt) const;
    double deval(unsigned di, const double x) const;
    void dim_lut_sorted(unsigned di, vu_t &lut) const;
    bool dim_lt(unsigned di, const unsigned &i0, const unsigned& i1) const
    {
        const vd_t &v = xyz_ships[di];
        return v[i0] < v[i1];
    }
    vd_t xyz_ships[3];
    vd_t p;
    double solution;
};

Starwars::Starwars(istream& fi) :
    solution(0)
{
    unsigned n;
    fi >> n;
    for (unsigned di = 0; di < 3; ++di)
    {
        xyz_ships[di] = vd_t(vd_t::size_type(n), 0.);
    }
    p = vd_t(vd_t::size_type(n), 0.);
    for (unsigned i = 0; i < n; ++i)
    {
        fi >> xyz_ships[0][i] >> xyz_ships[1][i] >> xyz_ships[2][i] >> p[i];
    }
}

void Starwars::solve_naive()
{
    unsigned n = n_ships();
    double pt[3]{0, 0,  0};
    for (unsigned di = 0; di < 3; ++di)
    {
        double minmax_power = -1;
        vu_t lut;
        const vd_t &xs = xyz_ships[di];
        pt[di] = xs[0];
        dim_lut_sorted(di, lut);
        for (unsigned il = 0; il < n; ++il)
        {
            unsigned i0 = lut[il];
            //   a(x-x0)
            double a0 = 1. / p[i0];
            double b0 = -a0 * xs[i0];
            for (unsigned ir = il + 1; ir < n; ++ir)
            {
                unsigned i1 = lut[ir];
                double a1 = -1. / p[i1];
                double b1 = -a1 * xs[i1];
                // intersect:  a0*x + b0 = x1*x + b1
                double x = (b1 - b0) / (a0 - a1);
                double xpower = deval(di, x);
                if ((minmax_power < 0) || minmax_power > xpower)
                {
                    minmax_power = xpower;
                    pt[di] = x;
                }
            }
        }
    }
    solution = eval(pt);
}

void Starwars::solve()
{
    double cruiser[3];
    bool single = n_ships() == 1;
    for (unsigned di = 0; di < 3; ++di)
    {
        cruiser[di] = single ? xyz_ships[di][0] : solve_dim(di);
    }
    solution = eval(cruiser);
    if (dbg_flags & 0x4) { 
       static double pg[3] = {1.7, 0.7, 0.9};
       eval(pg);
    }
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
    lut = vu_t(vd_t::size_type(n), 0u);
    for (unsigned i = 0; i < n; ++i) { lut[i] = i; }
    sort(lut.begin(), lut.end(), bind(&Starwars::dim_lt, *this, di, _1, _2));
}


double Starwars::solve_dim(unsigned di) const
{
    double x = 0;
    vu_t lut;
    iota(lut, n_ships());
    const vd_t &xyz_di = xyz_ships[di];
    sort(lut.begin(), lut.end(), 
        [&xyz_di](const unsigned &i0, const unsigned &i1)
        {
            return xyz_di[i0] < xyz_di[i1];
        });
    // Left  linear. Given (x0, p), ==>  + (1/p)(x - x0) = (1/p)x - x0/p
    // Right linear. Given (x0, p), ==>  + (1/p)(x0 - x) = (-1/p)x + x0/p
    unsigned n = n_ships();
    vd_t a_left, b_left, a_right, b_right;
    a_left = b_left = a_right = b_right = vd_t(vd_t::size_type(n), 0.);

    a_left[0] = b_left[0] = 0.;
    for (unsigned i0 = 0, i1 = 1; i1 < n; i0 = i1++)
    {
        unsigned idi = lut[i0];
        double dp = 1. / p[idi];
        a_left[i1] = a_left[i0] + dp;
        b_left[i1] = b_left[i0] - dp * xyz_di[idi];
    }

    a_right[n - 1] = b_right[n - 1] = 0.;
    for (unsigned i1 = n - 1, i0 = n - 2; i1 > 0; i1 = i0--)
    {
        unsigned idi = lut[i1];
        double dp = 1. / p[idi];
        a_right[i0] = a_right[i1] - dp;
        b_right[i0] = b_right[i1] + dp * xyz_di[idi];
    }
    
    double minmax = -1.;
    for (unsigned i0 = 0, i1 = 1; i0 < n - 1; i0 = i1++)
    {
        unsigned idi = lut[i0];
        // Max within  xyz_di[lut[i0]], xyz_di[lut[i1]], where left == right
        // aL x + bL = aR x + bR
        // x = (aL - aR) / (bR - bL)
        double xmax = (b_right[i0] - b_left[i1]) / (a_left[i1] - a_right[i0]);
        double v = a_left[i1] * (xmax - xyz_di[idi]) + b_left[idi];
        if (dbg_flags & 0x2) {
            unsigned idi1 = lut[i1];
            double vr = a_right[i0] * (xmax - xyz_di[idi1]) + b_right[idi1];
            cerr << "di="<<di << ", i0="<<i0 << ", v="<<v << ", vr="<<vr << "\n";
        }
        if ((i0 == 0) || (minmax > v))
        {
            minmax = v;
            x = xmax;
        }
    }
    
    return x;
}

double Starwars::eval(const double *pt) const
{
    double max_power = 0;
    for (unsigned i = 0, n = n_ships(); i < n; ++i)
    {
        double power = 0;
        for (unsigned di = 0; di < 3; ++di)
        {
            double delta = xyz_ships[di][i] - pt[di];
            power += (delta > 0 ? delta : -delta);
        }
        power /= p[i];
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

double Starwars::deval(unsigned di, const double x) const
{
    double max_power = 0;
    const vd_t &xs = xyz_ships[di];
    for (unsigned i = 0, n = n_ships(); i < n; ++i)
    {
        double delta = xs[i] - x;
        double power = double(delta > 0 ? delta : -delta) / double(p[i]);
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
