// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <fstream>
#include <string>
// #include <set>
// #include <map>
#include <vector>
#include <utility>
#include <limits>
#include <cmath>
#include <sstream>

// #include <cstdlib>

// #include <gmpxx.h>
#include "tricirc.h"

using namespace std;

// typedef mpz_class mpzc_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
// typedef vector<ul_t> vul_t;

static unsigned dbg_flags;

class Circle
{
 public:
    Circle(double _x=0, double _y=0, double _r=0): x(_x), y(_y), r(_r) {}
    string str() const
    {
        ostringstream os;
        os << "{Circle(("<<x<<",  "<<y<<"), "<<r<<")}";
        return os.str();
    }
    double x, y, r;
    
};
typedef vector<Circle> vcirc_t;

class Watering
{
 public:
    Watering(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    void get_sprinklers();
    bool plants_covered(const Circle &s0, const Circle &s1) const;
    bool circ_in_circ(const Circle &plant, const Circle &sprinkler) const;
    bool twocirc(
        double &x, double &y, double &r,
        double x0, double y0, double r0,
        double x1, double y1, double r1) const;
    vcirc_t plants;
    vcirc_t sprinklers;
    double solution;
};

const double eps = std::numeric_limits<float>::epsilon();

Watering::Watering(istream& fi) : solution(0)
{
    unsigned n_sprinklers;
    fi >> n_sprinklers;
    plants.reserve(n_sprinklers);
    for (unsigned i = 0; i < n_sprinklers; ++i)
    {
        double x, y, r;
        fi >> x >> y >> r;
        plants.push_back(Circle(x, y, r));
    }
}

void Watering::solve_naive()
{
    solve();
}

void Watering::solve()
{
    get_sprinklers();
    unsigned n_sprinklers = sprinklers.size();
    solution = sprinklers[0].r;
    Circle sol_sprink0 = sprinklers[0], sol_sprink1 = sprinklers[0];
    bool any = false;
    for (unsigned si0 = 0; si0 != n_sprinklers; ++si0)
    {
        const Circle &sprinkler0 = sprinklers[si0];
        for (unsigned si1 = si0 + 1; si1 != n_sprinklers; ++si1)
        {
            const Circle &sprinkler1 = sprinklers[si1];
            bool covered = plants_covered(sprinkler0, sprinkler1);
            if (covered)
            {
                double rmax = max(sprinkler0.r, sprinkler1.r);
                if ((solution > rmax) || !any)
                {
                    any = true;
                    solution = rmax;
                    sol_sprink0 = sprinkler0;
                    sol_sprink1 = sprinkler1;
                }
            }
        }
    }
    cerr << "Cover: " << sol_sprink0.str() << " " << sol_sprink1.str() << "\n";
}

void Watering::get_sprinklers()
{
    bool ok = true;
    sprinklers = plants;
    unsigned n = plants.size();
    for (unsigned i0 = 0; i0 < n && ok; ++i0)
    {
        const Circle &c0 = plants[i0];
        for (unsigned i1 = i0 + 1; i1 < n && ok; ++i1)
        {
            double x, y, r;
            const Circle &c1 = plants[i1];
            bool ok = twocirc(x, y, r, c0.x, c0.y, c0.r, c1.x, c1.y, c1.r);
            sprinklers.push_back(Circle(x, y, r));
            for (unsigned i2 = i1 + 1; i2 < n && ok; ++i2)
            {
                const Circle &c2 = plants[i2];
                bool ok = tricirc(
                    x, y, r, 
                    c0.x, c0.y, c0.r,
                    c1.x, c1.y, c1.r,
                    c2.x, c2.y, c2.r);
                if (ok)
                {
                    sprinklers.push_back(Circle(x, y, r));
                }
                else
                {
                    cerr << "Failure in tricirc: colinear?:"
                    "  c0="<<c0.str()<<
                    ", c1="<<c1.str()<<
                    ", c2="<<c2.str()<< "\n";
                }
            }
        }
    }
}

bool Watering::twocirc(
    double &x, double &y, double &r,
    double x0, double y0, double r0,
    double x1, double y1, double r1) const
{
    bool ok = true;
    double dx = x1 - x0;
    double dy = y1 - y0;
    double cdist2 = dx*dx + dy*dy;
    double cdist = sqrt(cdist2);
    if (cdist < eps)
    {
       x = x0; y = y0; r = max(r0, r1);
    }
    else
    {
        r = (cdist + r0 + r1) / 2.;
        double d_to_c0 = r - r0;
        double q0 = d_to_c0/cdist;
        x = x0 + q0*dx;
        y = y0 + q0*dy;
    }
    return ok;
}

bool Watering::plants_covered(const Circle &s0, const Circle &s1) const
{
   bool covered = true;
   for (unsigned i = 0, n = plants.size(); i < n && covered; ++i)
   {
       const Circle &plant = plants[i];
       covered = circ_in_circ(plant, s0) || circ_in_circ(plant, s1);
   }
   return covered;
}

bool
Watering::circ_in_circ(const Circle &c, const Circle &cbig) const
{
   double dx = cbig.x - c.x;
   double dy = cbig.y - c.y;
   double dist2 = dx*dx + dy*dy;
   // dist + c.r < = cbig.r
   double dr = cbig.r - c.r;
   bool covered = (dr >= 0.) && (dist2 <= dr*dr + eps);
   return covered;
}

void Watering::print_solution(ostream &fo) const
{
    fo << " " << solution;
}

int main(int argc, char ** argv)
{
    const string dash("-");

    bool naive = false;
    bool tellg = false;
    int rc = 0, ai;

    for (ai = 1; (rc == 0) && (ai < argc) && (argv[ai][0] == '-') &&
        argv[ai][1] != '\0'; ++ai)
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
        else
        {
            cerr << "Bad option: " << opt << "\n";
            return 1;
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

    string ignore;
    unsigned n_cases;
    *pfi >> n_cases;
    getline(*pfi, ignore);

    void (Watering::*psolve)() =
        (naive ? &Watering::solve_naive : &Watering::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Watering watering(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (watering.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        watering.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
