// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
// #include <set>
#include <map>
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

class Line
{
 public:
    Line(double va=1., double vx=0., double vy=0.) : a(va), x(vx), y(vy) {}
    double a, x, y;
};

typedef map<double, Line> inc2line_t;
typedef map<double, Line, greater<double>> dec2line_t;

class PolyLines
{
 public:
    PolyLines() {}
    void linsert(const Line& line)
    {
        xl2line.insert(xl2line.end(), inc2line_t::value_type(line.x, line));
        al2line.insert(xl2line.end(), inc2line_t::value_type(line.a, line));
    }
    inc2line_t xl2line;
    inc2line_t al2line;
    dec2line_t xr2line;
    dec2line_t ar2line;
};

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
    void build(unsigned dim, PolyLines &pls);
    vd_t xyz_ships[3];
    vd_t p;
    vd_t dp;
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
    p = dp = vd_t(vd_t::size_type(n), 0.);
    for (unsigned i = 0; i < n; ++i)
    {
        fi >> xyz_ships[0][i] >> xyz_ships[1][i] >> xyz_ships[2][i] >> p[i];
        dp[i] = 1./p[i];
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
            double a0 = dp[i0];
            double b0 = -a0 * xs[i0];
            for (unsigned ir = il + 1; ir < n; ++ir)
            {
                unsigned i1 = lut[ir];
                double a1 = -dp[i1];
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
    for (unsigned di = 0; di < 3; ++di)
    {
        PolyLines pls;
        build(di, pls);
    }
}

void Starwars::build(unsigned di, PolyLines& pls)
{
    const vd_t &xs = xyz_ships[di];
    const unsigned n = xs.size();
    vu_t lut;
    dim_lut_sorted(di, lut);
    unsigned i0 = lut[0];
    Line line0(dp[i0], xs[i0], 0.);
    pls.linsert(line0);
    for (unsigned i = 1; i < n; ++i)
    {
        unsigned luti = lut[i];
        double x = xs[luti];
        double a = dp[luti];
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
    iota(lut, n);
    sort(lut.begin(), lut.end(), bind(&Starwars::dim_lt, *this, di, _1, _2));
}


double Starwars::solve_dim(unsigned di) const
{
    return 0;
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

double Starwars::deval(unsigned di, const double x) const
{
    double max_power = 0;
    const vd_t &xs = xyz_ships[di];
    for (unsigned i = 0, n = n_ships(); i < n; ++i)
    {
        double delta = xs[i] - x;
        double power = dp[i] * double(delta > 0 ? delta : -delta);
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
