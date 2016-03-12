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

#include "ratgeo.h"

using namespace std;

static unsigned dbg_flags;

typedef vector<int> vi_t;

class Matrix
{
 public:
    Matrix(unsigned vw, unsigned vh) : 
       w(vw), h(vh), _e(vi_t::size_type(w*h), 0)
    {}
    unsigned a(unsigned x, unsigned y) const { return _e[i(x, y)]; }
    void a(unsigned x, unsigned y, int v) { _e[i(x, y)] = v; }
    const unsigned w, h;
 private:
    unsigned i(unsigned x, unsigned y) const 
    {
        unsigned ret = y*w + x; 
        if (ret > _e.size())
        {
           cerr << "Fuck!\n";
        }
        return ret;
    }
    vi_t _e;
};

unsigned gcd(unsigned m, unsigned n)
{
   while (n)
   {
      unsigned t = n;
      n = m % n;
      m = t;
   }
   return m;
}

unsigned lcm(unsigned m, unsigned n)
{
    return (m*n)/gcd(m, n);
}

#if 0
class ModSteps
{
 public:
     ModSteps(int vm=0, int n=0): m(vm), n(vn), period(0) {}
     void compute();
     vi_t v;
     unsigned period;
 private:
     void compute1(int d);
};

void ModSteps::compute()
{
    v.clear();
    if (m || n)
    {
        if ((m == 0) || (n == 0))
        {
            compute1(m + n);
        }
        else if ((m > 0) == (n > 0)) // same sign
        {
        }
        else // different signs
        {
        }
    }
}
#endif

class XY
{
  public:
    XY(int vx=0, int vy=0): x(vx), y(vy) {}  
    int xy(int i01) const { return i01 ? y : x; }
    int x, y;
};

bool operator<(const XY& xy0, const XY& xy1)
{
    bool lt = (xy0.x < xy1.x) || ((xy0.x == xy1.x) && (xy0.y < xy1.y));
    return lt;
}

bool operator==(const XY& xy0, const XY& xy1)
{
    return xy0.x == xy1.x && xy0.y == xy1.y;
}

XY operator+(const XY& xy0, const XY& xy1)
{
    return XY(xy0.x + xy1.x, xy0.y + xy1.y);
}

class PingPong
{
 public:
    PingPong(istream& fi, bool large=false);
    void solve();
    void print_solution(ostream&);
 private:
    void solve_small();
    void solve_large();
    void solve_colinear();
    void solve_regular();
    unsigned sweep(XY& end, const XY& start,
                   unsigned d_major, bool count_1st) const;
    bool inside(const XY& p) const
    {
        return ((0 <= p.x) && (p.x < dim.x)) &&
               ((0 <= p.y) && (p.y < dim.y));
    }
    XY dim;
    XY delta[2];
    XY orig;
    unsigned solution;
    bool _large;
};

PingPong::PingPong(istream& fi, bool large) : solution(0), _large(large)
{
    fi >> dim.x >> dim.y;
    fi >> delta[0].x >> delta[0].y;
    fi >> delta[1].x >> delta[1].y;
    fi >> orig.x >> orig.y;
}

void PingPong::solve() 
{ 
    if (_large) 
    {
        solve_large();
    }
    else
    {
        solve_small(); 
    }
}

void PingPong::solve_large()
{
    if (delta[0].x * delta[1].y - delta[0].y * delta[1].x == 0)
    {
        solve_colinear();
    }
    else
    {
        solve_regular();
    }
}

void PingPong::solve_colinear()
{
}

void PingPong::solve_regular()
{
#if 0
    // find when ray: origin->delta[0] leaves the board
    unsigned k = max(dim.x, dim.y);
    int i0 = -1;
    for (unsigned i = 0; i < 2; ++i)
    {
        int delta0 = delta[0].xy(i);
        if (delta0 != 0)
        {
            int oxyi = origin.xy(i);
            int to_edge = (delta0 > 0 ? dim.xy(i) - oxyi - 1 : oxyi);
            int ki = to_edge / delta0;
            if (k > ki)
            {
                k = ki;
                i0 = i;
            }
        }
    }
#endif
}

unsigned PingPong::sweep(
    XY& end, const XY& start, unsigned d_major, bool count_1st) const
{
    unsigned triggered = 0;
#if 0
    // Find inital 2 rays hitting the boundary.
    for (unsigned di = 0; di < 2; ++di)
    {
        for (i = 0; i < 2; ++i)
        {
            int d = delta[di].xy(i);
            if (d != 0)
            {
                int oxyi = start.xy(i);
                int to_edge = (d > 0 ? dim.xy(i) - oxyi - 1 : oxyi);
                int ki = to_edge / d;
                if (k > ki)
                {
                    k = ki;
                    i0 = i;
                }
            }
        }
    }
#endif
    return triggered;
}


#if 0
void PingPong::solve_large()
{
    Matrix mtx(dim.x, dim.y);
    set<XY> triggered;
    set<XY> gen;
    gen.insert(gen.end(), orig);
    while (!gen.empty())
    {
        set<XY> next_gen;
        for (auto i = gen.begin(); i != gen.end(); ++i)
        {
            const XY& xy = *i;
            mtx.a(xy.x, xy.y, 1);
            triggered.insert(xy);
            for (unsigned mi = 0; mi < 2; ++mi)
            {
                XY mxy = xy + delta[mi];
                if (inside(mxy) && (triggered.find(mxy) == triggered.end()))
                {
                    next_gen.insert(mxy);
                }
            }
        }
        gen = next_gen;
    }
    mtx.a(orig.x, orig.y, 7);
    solution = triggered.size();

    for (int r = dim.y - 1; r >= 0; --r)
    {
        cerr << "  ";
        for (int c = 0; c < dim.x; ++c)
        {
            cerr << mtx.a(c, r);
        }
        cerr << "\n";
    }
    cerr << "\n";
}
#endif

void PingPong::solve_small()
{
    set<XY> triggered;
    set<XY> gen;
    gen.insert(gen.end(), orig);
    while (!gen.empty())
    {
        set<XY> next_gen;
        for (auto i = gen.begin(); i != gen.end(); ++i)
        {
            const XY& xy = *i;
            triggered.insert(xy);
            for (unsigned mi = 0; mi < 2; ++mi)
            {
                XY mxy = xy + delta[mi];
                if (inside(mxy) && (triggered.find(mxy) == triggered.end()))
                {
                    next_gen.insert(mxy);
                }
            }
        }
        gen = next_gen;
    }
    solution = triggered.size();
}

void PingPong::print_solution(ostream &fo)
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

    if (argc > 3) { dbg_flags = strtoul(argv[3], 0, 0); }
    bool large = (argc > 4) && !strcmp(argv[4], "-large");

    unsigned n_cases;
    *pfi >> n_cases;

    ostream &fout = *pfo;
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        PingPong pingpong(*pfi, large);
        // cerr << "Case ci="<<ci << " (ci+1)="<<ci+1 << "\n";
        pingpong.solve();
        fout << "Case #"<< ci+1 << ":";
        pingpong.print_solution(fout);
        fout << "\n";
        fout.flush();
        
    }
    
    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
