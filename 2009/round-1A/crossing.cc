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
// #include <gmpxx.h>

using namespace std;

// typedef mpz_class mpzc_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
// typedef vector<ul_t> vul_t;
typedef vector<ull_t> vull_t;

static unsigned dbg_flags;

static const ull_t ull_infty = ~ull_t(0);

typedef enum { D_EW, D_SN } direction_t;

class Corner
{
 public:
    Corner (ull_t e = ull_infty) : earliest(e) {}
    ull_t earliest;
};

class Cross
{
 public:
    Cross(ull_t vS=1, ull_t vW=1, ull_t vT=0) :
         S(vS), W(vW), T(vT),
         period(S + W),
         Tadd((period - (T % period)) % period)
    {
    }
    ull_t wait_from_dir(ull_t t, direction_t d) const;
    ull_t S, W, T; // period - input
    ull_t period;
    ull_t Tadd;
    Corner corners[2][2]; // x y
};
typedef vector<Cross> vcross_t;

ull_t Cross::wait_from_dir(ull_t t, direction_t d) const
{
    ull_t m = (t + Tadd) % period;
    ull_t ret = (d == D_SN
        ? (m < S ? 0 : period - m)
        : (m >= S ? 0 : S - m));
    return ret;
}

class Crossing
{
 public:
    Crossing(istream& fi);
    void solve();
    void print_solution(ostream&) const;
 private:
    void solve_naive();
    void solve_naive_corner(ull_t t, 
        unsigned x, unsigned y, unsigned cx, unsigned cy);
    const Cross& cross(unsigned i, unsigned j) const
        { return _cross[M * i + j]; }
    Cross& cross(unsigned x, unsigned y) { return _cross[M * y + x]; }
    unsigned N, M;
    vcross_t _cross;
    ull_t solution;
    
};

Crossing::Crossing(istream& fi) : solution(0)
{
    fi >> N >> M;
    _cross = vcross_t(vcross_t::size_type(N * M), Cross());
    for (int y = N - 1; y >= 0; --y)
    {
        for (unsigned x = 0; x < M; ++x)
        {
            ull_t S, W, T;
            fi >> S >> W >> T;
            cross(x, y) = Cross(S, W, T);
        }
    }
}

void Crossing::solve()
{
    if (dbg_flags == 0)
    {
        solve_naive();
    }
}

void Crossing::solve_naive()
{
    solve_naive_corner(0, 0, 0, 0, 0);
    const Cross& right_top_cross = cross(M - 1, N - 1);
    const Corner& right_top_corner = right_top_cross.corners[1][1];
    solution = right_top_corner.earliest;
}

void Crossing:: solve_naive_corner(ull_t t,
    unsigned x, unsigned y, unsigned cx, unsigned cy)
{
    Cross& crs = cross(x, y);
    Corner& corner = crs.corners[cx][cy];
    if (corner.earliest > t)
    {
        corner.earliest = t;
        ull_t xwait = crs.wait_from_dir(t, D_EW);
        ull_t ywait = crs.wait_from_dir(t, D_SN);

        // intuitively forward

        // Prefer shorter wait first
        for (unsigned xy_order = 0; xy_order != 2; ++xy_order)
        {
            if ((xy_order == 0) == (xwait < ywait))
            {
                if (cx == 0)
                {
                    solve_naive_corner(t + xwait + 1, x, y, 1, cy);
                }
                else if (x + 1 < M)
                {
                    solve_naive_corner(t + 2, x + 1, y, 0, cy);
                }
            }
            else
            {
                if (cy == 0)
                {
                    solve_naive_corner(t + ywait + 1, x, y, cx, 1);
                }
                else if (y + 1 < N)
                {
                    solve_naive_corner(t + 2, x, y + 1, cx, 0);
                }
            }
        }

        // intuitively backward - less likely to produce progress

        if (cx == 1)
        {
            solve_naive_corner(t + xwait + 1, x, y, 0, cy);
        }
        else if (x > 0)
        {
            solve_naive_corner(t + 2, x - 1, y, 1, cy);
        }

        if (cy == 1)
        {
            solve_naive_corner(t + ywait + 1, x, y, cx, 0);
        }
        else if (y > 0)
        {
            solve_naive_corner(t + 2, x, y - 1, cx, 1);
        }
    }
}

void Crossing::print_solution(ostream &fo) const
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
    // bool large = (argc > 4) && !strcmp(argv[4], "-large");

    unsigned n_cases;
    *pfi >> n_cases;

    ostream &fout = *pfo;
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Crossing problem(*pfi);
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
