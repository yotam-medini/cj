// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <fstream>
#include <string>
// #include <set>
// #include <map>
#include <vector>
#include <utility>

#include <cstdlib>
#include <gmpxx.h>

using namespace std;

typedef mpz_class mpzc_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<unsigned> vu_t;
typedef vector<ul_t> vul_t;
typedef vector<ull_t> vull_t;

static unsigned dbg_flags;

struct XYXY_M3
{
    XYXY_M3(unsigned vxi=0, unsigned vyi=0, unsigned vx1i=0, unsigned vy1i=0):
         xi(vxi), yi(vyi), x1i(vx1i), y1i(vy1i) {}
    unsigned xi, yi, x1i, y1i;
};
typedef vector<XYXY_M3> v_xyxy_m3_t;

class Crop
{
 public:
    Crop(istream& fi);
    void solve();
    void solve_naive();
    void print_solution(ostream&) const;
 private:
    unsigned n;
    ull_t A, B, C, D, x0, y0, M;
    mpzc_t solution;
};

Crop::Crop(istream& fi) :
    solution(0)
{
    fi >> n >> A >> B >> C >> D >> x0 >> y0 >> M;
}

void Crop::solve()
{
    ull_t n_xy[3][3];
    ull_t n_xyxy[3][3][3][3];
    v_xyxy_m3_t xy_comps[3][3];

    for (unsigned xi = 0; xi != 3; ++xi)
    {
        for (unsigned yi = 0; yi != 3; ++yi)
        {
            n_xy[xi][yi] = 0;
            for (unsigned x1i = 0; x1i != 3; ++x1i)
            {
                for (unsigned y1i = 0; y1i != 3; ++y1i)
                {
                    n_xyxy[xi][yi][x1i][y1i] = 0;
                    if ((xi < x1i) || ((xi == x1i) && (yi <= y1i)))
                    {
                        unsigned xcmp = (6 - (xi + x1i)) % 3;
                        unsigned ycmp = (6 - (yi + y1i)) % 3;
                        XYXY_M3 xyxy_m3(xi, yi, x1i, y1i);
                        xy_comps[xcmp][ycmp].push_back(xyxy_m3);
                    }
                }
            }
        }
    }


    ull_t x = x0;
    ull_t y = y0;
    for (ull_t t = 0; t != n; ++t)
    {
        unsigned xm3 = x % 3;
        unsigned ym3 = y % 3;
        unsigned n_xy_old = n_xy[xm3][ym3];

        const v_xyxy_m3_t& v = xy_comps[xm3][ym3];
        for (v_xyxy_m3_t::const_iterator i = v.begin(), e = v.end();
             i != e; ++i)
        {
            const XYXY_M3 &xyxy = *i;
            unsigned n2 = n_xyxy[xyxy.xi][xyxy.yi][xyxy.x1i][xyxy.y1i];
            solution += n2;
        }

        for (unsigned xi = 0; xi != 3; ++xi)
        {
            for (unsigned yi = 0; yi != 3; ++yi)
            {
                unsigned n_peer = n_xy[xi][yi];
                unsigned nn = n_xyxy[xi][yi][xm3][ym3] + n_peer;
                n_xyxy[xi][yi][xm3][ym3] = nn;
                n_xyxy[xm3][ym3][xi][yi] = nn;
            }
        }
        n_xy[xm3][ym3] = n_xy_old + 1;
        x = (A*x +B) % M;
        y = (C*y +D) % M;
    }
}

void Crop::solve_naive()
{
    vul_t x(vul_t::size_type(n), 0);
    vul_t y(vul_t::size_type(n), 0);

    x[0] = x0 % 3;
    y[0] = y0 % 3;
    ull_t xp = x0;
    ull_t yp = y0;
if (dbg_flags & 0x1) { cerr << "[0]=(" << xp <<", "<<yp<<")\n";}
    for (unsigned i = 1; i < n; ++i)
    {
        ull_t xnext = (A*xp + B) % M;
        ull_t ynext = (C*yp + D) % M;
if (dbg_flags & 0x1) { cerr << "[" << i << "]=(" << xnext <<", "<<ynext<<")\n";}
        x[i] = xnext % 3;
        y[i] = ynext % 3;
        xp = xnext;
        yp = ynext;
    }
    for (unsigned i = 0; i < n; ++i)
    {
        for (unsigned j = i + 1; j < n; ++j)
        {
            unsigned xij = x[i] + x[j];
            unsigned yij = y[i] + y[j];
            for (unsigned k = j + 1; k < n; ++k)
            {
                if ( (((xij + x[k]) % 3) == 0) && (((yij + y[k]) % 3) == 0) )
                {
if (dbg_flags & 0x1) { cerr << "[" << i<<", "<< j<<", " << k << "]\n"; }
                    ++solution;
                }
            }
        }
    }

}

void Crop::print_solution(ostream &fo) const
{
    fo << " " << solution;
}

int main(int argc, char ** argv)
{
    const string dash("-");

    istream *pfi = (argc < 2 || (string(argv[1]) == dash))
         ? &cin
         : new ifstream(argv[1]);
    ostream *pfo = (argc < 3 || (string(argv[2]) == dash))
         ? &cout
         : new ofstream(argv[2]);

    if ((!pfi) || (!pfo) || pfi->fail() || pfo->fail())
    {
        cerr << "Open file error\n";
        exit(1);
    }

    if (argc > 3) { dbg_flags = strtoul(argv[3], 0, 0); }
    bool naive = (argc > 4) && !strcmp(argv[4], "-naive");
    // bool large = (argc > 4) && !strcmp(argv[4], "-large");

    unsigned n_cases;
    *pfi >> n_cases;

    ostream &fout = *pfo;
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Crop problem(*pfi);
        // cerr << "Case ci="<<ci << " (ci+1)="<<ci+1 << "\n";
        if (naive)
        {
            problem.solve_naive();
        }
        else
        {
            problem.solve();
        }
        fout << "Case #"<< ci+1 << ":";
        problem.print_solution(fout);
        fout << "\n";
        fout.flush();

    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}

