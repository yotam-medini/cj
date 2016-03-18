// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <map>
#include <vector>
#include <utility>

#include <cstdlib>
#include <cmath>


#include "curved.h"

using namespace std;

static unsigned dbg_flags;

class Fly
{
 public:
    Fly(istream& fi);
    void solve();
    void print_solution(ostream&);
 private:
    double f, R, t, r, g;       
    double solution;
};

Fly::Fly(istream& fi) : solution(0.)
{
    fi >> f >> R >> t >> r >> g;
}

void Fly::solve()
{
    static const double sqrt2 = sqrt(2.);

    if ((2*f > g) || (R - t < f))
    {
        solution = 1.;
    }
    else
    {
        // Compute miss area.
        // miss squares
        const double gmiss = g - 2*f;
        const double R_t_f = R - t - f;
        const double R_t_f_2 = R_t_f * R_t_f;
        const double sqrt2_r = sqrt2 * r;
        const double g_2r = g + 2*r;

        const unsigned n_diag = floor((R - t - f + sqrt2_r) / (sqrt2 * g_2r));
        const double xy0 = r + n_diag * g_2r + f;
        const double diag_csq_area = area_curved_sqaure(xy0, xy0, gmiss, R_t_f);
        
        // Consider first octet
        unsigned n_squares_octet = 0;
        double octet_curved_area = 0.;
        double x0_left = r + g_2r;
        for (unsigned row = 0; row <= n_diag; ++row, x0_left += g_2r)
        {
            double y0 = r + row * g_2r; // (r + g); //  + f;
            double x1_diag = r + f + gmiss + row * g_2r;
            double y1 = y0 + gmiss;
            double x1_max = sqrt(R_t_f_2 - y1*y1);
            int n_sq = floor((x1_max - x1_diag) / g_2r);
            if (dbg_flags & 0x1) {
                n_sq = floor((x1_max + r - x0_left) / g_2r);
            }
            if (n_sq > 0)
            {
                n_squares_octet += n_sq;
            }
            for (double x0 = x0_left + max(n_sq, 0) * g_2r; 
                 x0 + f < R_t_f; x0 += g_2r)
            {
                octet_curved_area += 
                    area_curved_sqaure(x0 + f, y0 + f, gmiss, R_t_f);
            }
        }
        unsigned n_whole_squares = 4 * n_diag + 8 * n_squares_octet;
        double whole_squares_area = n_whole_squares * gmiss * gmiss;
        double curved_area = 4 * diag_csq_area + 8 * octet_curved_area;
        double miss_area = whole_squares_area + curved_area;
        double whole_area = M_PI * R * R;
        solution = 1. - miss_area / whole_area;
    }
}

void Fly::print_solution(ostream &fo)
{
    // fo << " " << solution;
    char s[0x10];
    sprintf(s, "%6.6f", solution);
    fo << " " << s;
}

int main(int argc, char ** argv)
{
    // cerr << "yotam acq=" << area_curved_sqaure(7, 7, 1, 10) << "\n";
    const string dash("-");

    istream *pfi = (argc < 2 || (string(argv[1]) == dash))
         ? &cin
         : new ifstream(argv[1]);
    ostream *pfo = (argc < 3 || (string(argv[2]) == dash))
         ? &cout
         : new ofstream(argv[2]);

    if (argc > 3) { dbg_flags = strtoul(argv[3], 0, 0); }
    // bool large = (argc > 4) && !strcmp(argv[4], "-large");

    unsigned n_cases;
    *pfi >> n_cases;

    ostream &fout = *pfo;
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Fly problem(*pfi);
        // cerr << "Case ci="<<ci << " (ci+1)="<<ci+1 << "\n";
        problem.solve();
        fout << "Case #"<< ci+1 << ":";
        problem.print_solution(fout);
        fout << "\n";
        fout.flush();
        
    }
    
    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}

