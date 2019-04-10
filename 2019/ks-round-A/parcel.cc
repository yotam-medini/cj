// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <iostream>
#include <fstream>
#include <string>
#include <utility>
#include <vector>
#include <set>
#include <array>
#include <algorithm>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef vector<string> vs_t;

typedef array<int, 2> i2_t;
typedef array<u_t, 3> u3_t;
typedef vector<u3_t> vu3_t;

static unsigned dbg_flags;

static u_t dist_l1(const i2_t &pt0, const i2_t &pt1)
{
    u_t dl1 = abs(pt1[0] - pt0[0]) + abs(pt1[1] - pt0[1]);
    return dl1;
}

class BaseWHMatrix
{
  public:
    BaseWHMatrix(unsigned _w, unsigned _h) : w(_w), h(_h) {}
    const unsigned w; // width
    const unsigned h; // hight
  protected:
    unsigned xy2i(unsigned x, unsigned y) const 
    {
        unsigned ret = h*x + y;
        return ret;
    }
    void i2rc(unsigned &x, unsigned &y, unsigned i) const 
    {
        x = i / h;
        y = i % h;
    }
};

template <class T>
class WHMatrix : public BaseWHMatrix
{
  public:
    WHMatrix(unsigned _w=0, unsigned _h=0) : 
        BaseWHMatrix(_w, _h), _a(w*h > 0 ? new T[w*h] : 0) {}
    virtual ~WHMatrix() { delete [] _a; }
    const T& get(unsigned x, unsigned y) const { return _a[xy2i(x, y)]; }
    void put(unsigned x, unsigned y, const T &v) const { _a[xy2i(x, y)] = v; }
  private:
    T *_a;
};

typedef WHMatrix<u_t>  mtxu_t;

class Parcel
{
 public:
    Parcel(istream& fi);
    ~Parcel() { delete pdists; }
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    u_t compute_dists(); // and return max
    bool dist_can(u_t d) const;
    u_t r, c;
    vs_t grid;
    u_t solution;
    mtxu_t *pdists;
    vu3_t dist_xy;
};

Parcel::Parcel(istream& fi) : solution(0), pdists(0)
{
    fi >> r >> c;
    grid.reserve(r);
    for (u_t y = 0; y < r; ++y)
    {
        string s;
        fi >> s;
        grid.push_back(s);
    }
}

void Parcel::solve_naive()
{
    u_t best = r + c + 1; // infinity
    for (u_t y = 0; y < r; ++y)
    {
        for (u_t x = 0; x < c; ++x)
        {
            char save = grid[y].at(x);
            grid[y].at(x) = '1';
            u_t dist = compute_dists();
            if (best > dist)
            {
                best = dist;
            }
            grid[y].at(x) = save;
        }
    }
    solution = best;
}

void Parcel::solve()
{
    // solve_naive();
    compute_dists();
    mtxu_t &dists = *pdists;
    dist_xy.reserve(r*c);
    u_t n_offices = 0;
    for (u_t x = 0; x < c; ++x)
    {
        for (u_t y = 0; y < r; ++y)
        {
            dist_xy.push_back(u3_t({dists.get(x, y), x, y}));
            n_offices += (grid[y].at(x) - '0');
        }
    }
    if (n_offices < r*c - 1)
    {
        sort(dist_xy.begin(), dist_xy.end());
        u_t dlow = 0, dhigh = dist_xy.back()[0] + 1;
        while (dlow + 1 < dhigh)
        {
            u_t dmid = (dlow + dhigh)/2;
            if (dist_can(dmid))
            {
                dhigh = dmid;
            }
            else
            {
                dlow = dmid;
            }
        }
        solution = dhigh;
    }
}

u_t Parcel::compute_dists()
{
    u_t max_dist = 0;
    pdists = new mtxu_t(c, r);
    mtxu_t &dists = *pdists;
    for (u_t x = 0; x < c; ++x)
    {
        for (u_t y = 0; y < r; ++y)
        {
            dists.put(x, y, r + x + 1); // infinity
        }
    }
    for (u_t x = 0; x < c; ++x)
    {
        for (u_t y = 0; y < r; ++y)
        {
            if (grid[y].at(x) == '1')
            {
                for (u_t qx = 0; qx < c; ++qx)
                {
                    for (u_t qy = 0; qy < r; ++qy)
                    {
                        u_t dx = (x < qx ? qx - x : x - qx);
                        u_t dy = (y < qy ? qy - y : y - qy);
                        u_t d1 = dx + dy;
                        if (dists.get(qx, qy) > d1)
                        {
                            dists.put(qx, qy, d1);
                        }
                    }
                }
            }
        }
    }
    
    for (u_t x = 0; x < c; ++x)
    {
        for (u_t y = 0; y < r; ++y)
        {
            u_t dxy = dists.get(x, y);
            if (max_dist < dxy)
            {
                max_dist = dxy;
            }
        }
    }
    
    return max_dist;
}

bool Parcel::dist_can(u_t d) const
{
    bool can = true;
    // find max 1/8 rotated rect - bounding.
    int ne_max = 0;
    int sw_max = r + c + 1;
    int nw_max = -c;
    int se_max = r;
    bool any = false;
    i2_t ne_rep({0, 0}), sw_rep({0, 0}), nw_rep({0, 0}), se_rep({0, 0});
    static const i2_t xy00({0, 0});
    i2_t boundry_reps[4] = {xy00, xy00, xy00, xy00};
    const mtxu_t &dists = *pdists;
    for (int x = 0; x < int(c); ++x)
    {
        for (int y = 0; y < int(r); ++y)
        {
            u_t dxy = dists.get(x, y);
            if (dxy > d)
            {
                any = true;
                i2_t xy({x, y});
                int xpy = x + y;
                int ymx = y - x;
                if (ne_max < xpy) 
                { 
                    ne_max = xpy; boundry_reps[0] = xy;
                }
                if (sw_max > xpy) 
                { 
                    sw_max = xpy; boundry_reps[2] = xy;
                }
                if (nw_max < ymx) 
                { 
                    nw_max = ymx; boundry_reps[1] = xy;
                }
                if (se_max > ymx) 
                { 
                    se_max = ymx; boundry_reps[3] = xy;
                }
            }
        }
    }
    if (any)
    {
        int width_ne = ne_max - sw_max;
        int width_nw = nw_max - se_max;
        set<i2_t> centers;
        int ne_mid = (ne_max + sw_max)/2;
        int nw_mid = (nw_max + se_max)/2;
        for (int ne_step = 0; ne_step <= (width_ne % 2); ++ ne_step)
        {
            for (int nw_step = 0; nw_step <= (width_nw % 2); ++ nw_step)
            {   // solve
                //   x + y = ne_mid + ne_step
                //   y - x = nw_mid + nw_step
                int c2x = ne_mid + ne_step - (nw_mid + nw_step);
                int c2y = ne_mid + ne_step + nw_mid + nw_step;
                for (int cx_step = 0; cx_step <= (c2x % 2); ++ cx_step)
                {
                    int cx = c2x/2 + cx_step;
                    for (int cy_step = 0; cy_step <= (c2y % 2); ++ cy_step)
                    {
                        int cy = c2y/2 + cy_step;
                        centers.insert(centers.end(), i2_t({cx, cy}));
                    }         
                }
            }
        }
        u_t d_bdy_minmax = r + c + 1;
        for (const i2_t &center: centers)
        {
            u_t d_bdy = 0;
            for (u_t bi = 0; bi < 4; ++bi)
            {
                const i2_t &boundry_rep = boundry_reps[bi];
                u_t dcb = dist_l1(center, boundry_rep);
                if (d_bdy < dcb) 
                {
                    d_bdy = dcb;
                }
            }
            if (d_bdy_minmax > d_bdy)
            {
                d_bdy_minmax = d_bdy;
            }
        }
        if (dbg_flags & 0x1) { cerr << "d="<<d << 
            ", d_bdy_minmax="<<d_bdy_minmax << "\n"; }
        can = (d_bdy_minmax <= d);
    }
    return can;
}

void Parcel::print_solution(ostream &fo) const
{
    fo << ' ' << solution;
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

    void (Parcel::*psolve)() =
        (naive ? &Parcel::solve_naive : &Parcel::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Parcel problem(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (problem.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        problem.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
