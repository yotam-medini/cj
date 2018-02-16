// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20
// After peeking at contest analysis page.

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

class Ship
{
 public:
    Ship(double x=0, double y=0, double z=0, double receive_power=1):
        xyz{x, y, z},
        p(receive_power),
        dp(1./receive_power)
    {
        dp = 1./p;
        for (unsigned di = 0; di < 3; ++di) { pxyz[di] = dp * xyz[di]; }
    }
    double xyz[3];
    double p;
    double dp;
    double pxyz[3];
};
typedef vector<Ship> vship_t;

class Starwars
{
 public:
    Starwars(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    unsigned n_ships() const { return ships.size(); }
    bool is_sufficient(double power) const;
    void get_octagon_bounds(
        double *low8, double *high8, unsigned si, double power) const;
    vship_t ships;
    double solution;
};

Starwars::Starwars(istream& fi) :
    solution(0)
{
    unsigned n;
    fi >> n;
    ships.reserve(n);
    for (unsigned i = 0; i < n; ++i)
    {
        double x, y, z, p;
        fi >> x >> y >> z >> p;
        ships.push_back(Ship(x, y, z, p));
    }
}

void Starwars::solve_naive()
{
    solve();
}

void Starwars::solve()
{
    double low_power = 0;
    double high_power = 3 * 1e6;
    if (n_ships() > 1)
    {
        if (is_sufficient(low_power))
        {
            cerr << "Low bug";
            exit(1);
        }
        if (!is_sufficient(high_power))
        {
            cerr << "High bug";
            exit(1);
        }
        double delta = high_power - low_power;
        while (delta > 1e-7)
        {
            double mid = (low_power + high_power)/2.;
            if (is_sufficient(mid))
            {
                high_power = mid;
            }
            else
            {
                low_power = mid;
            }
            delta = high_power - low_power;
        }
        solution = high_power;
    }
}

void Starwars::print_solution(ostream &fo) const
{
    fo << ' ' << fixed << setprecision(6) << solution;
}

static void mini(double &v, double x) { if (v > x) { v = x; } }
static void maxi(double &v, double x) { if (v < x) { v = x; } }

bool Starwars::is_sufficient(double power) const
{
    bool sufficient = true;
    unsigned n = n_ships();
    double low8[8], high8[8];
    get_octagon_bounds(low8, high8, 0, power);
    for (unsigned si = 1; sufficient && (si < n); ++si)
    {
        double l8[8], h8[8];
        get_octagon_bounds(l8, h8, si, power);
        for (unsigned oi = 0; sufficient && (oi < 8); ++oi)
        {
            maxi(low8[oi], l8[oi]);
            mini(high8[oi], h8[oi]);
            sufficient = (low8[oi] <= high8[oi]);
        }
    }

    // Now combine pairs if doubke inequalities
    // 0=000    x+y+z
    // 1=001    x+y-z
    // 2=010    x-y+z
    // 3=011    x-y-z
    // 4=100   -x+y+z
    // 5=101   -x+y-z
    // 6=110   -x-y+z
    // 7=111   -x-y-z
    sufficient = sufficient && (low8[0] - high8[7] <= high8[0] - low8[7]);
    sufficient = sufficient && (low8[1] - high8[6] <= high8[1] - low8[6]);
    sufficient = sufficient && (low8[2] - high8[5] <= high8[2] - low8[5]);
    sufficient = sufficient && (low8[3] - high8[4] <= high8[3] - low8[4]);

    return sufficient;
}

void Starwars::get_octagon_bounds(
    double *low8, double *high8, unsigned si, double e) const
{
    // Octagon equatioons around (x0, y0, z0) with power dp
    //      -E <= dp( [Sx](x-x0) + [Sy](y-y0) + [Sz](z-z0)) <= E
    // Where Sx,Sy,Sz are +1 or -1, depending on 3 bits of  0<=u<8
    // Put  a = dp([Sx]x0 + [Sy]y0 + [Sz]z0)
    //      -E + a <= dp([Sx]x + [Sy]y + [Sz]z) <= E + a
    //      -E + a <= dp([Sx]x + [Sy]y + [Sz]z) <= E + a
    //      -E + a <= dp([Sx]x + [Sy]y + [Sz]z) <= E + a
    //      (-E + a)/dp <= [Sx]x + [Sy]y + [Sz]z) <= (E + a)/dp
    //      p(-E + a) <= [Sx]x + [Sy]y + [Sz]z) <= p(E + a)
    const Ship &ship = ships[si];
    for (unsigned u = 0; u < 8; ++u)
    {
        double a = 0;
        for (unsigned di = 0; di < 3; ++di)
        {
            const double pdi = ship.pxyz[di];
            a += ((u & (0x1 << di)) == 0 ? pdi : - pdi);
        }
        low8[u] = ship.p * (-e + a);
        high8[u] = ship.p * (e + a);
    }
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
                fpos_last << ", " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
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
