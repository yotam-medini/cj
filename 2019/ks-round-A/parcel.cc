// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <iostream>
#include <fstream>
#include <string>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<string> vs_t;

static unsigned dbg_flags;

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
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    u_t get_dist() const;
    u_t r, c;
    vs_t grid;
    u_t solution;
};

Parcel::Parcel(istream& fi) : solution(0)
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
            u_t dist = get_dist();
            if (best > dist)
            {
                best = dist;
            }
            grid[y].at(x) = save;
        }
    }
    solution = best;
}

u_t Parcel::get_dist() const
{
    u_t dist = 0;
    mtxu_t dgrid(c, r);
    for (u_t x = 0; x < c; ++x)
    {
        for (u_t y = 0; y < r; ++y)
        {
            dgrid.put(x, y, r + x + 1); // infinity
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
                        if (dgrid.get(qx, qy) > d1)
                        {
                            dgrid.put(qx, qy, d1);
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
            u_t dxy = dgrid.get(x, y);
            if (dist < dxy)
            {
                dist = dxy;
            }
        }
    }
    
    return dist;
}

void Parcel::solve()
{
    solve_naive();
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
