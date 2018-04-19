// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <utility>

#include <cstdlib>

using namespace std;

typedef unsigned long ul_t;
typedef vector<unsigned> vu_t;;

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

typedef WHMatrix<bool> mtxb_t;


class Waffle
{
 public:
    Waffle(istream& fi);
    ~Waffle() { delete pgrid; }
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    unsigned count_cookies(unsigned xb, unsigned xe, unsigned yb, unsigned ye) 
        const;
    bool get_cuts();
    void print_grid() const;
    unsigned r, c, h, v;
    mtxb_t *pgrid;
    unsigned n_cookies;
    vu_t xcuts, ycuts;
    const char *solution;
};

Waffle::Waffle(istream& fi) : pgrid(0), solution(0)
{
    fi >> r >> c >> h >> v;
    pgrid = new mtxb_t{c, r};
    mtxb_t &grid = *pgrid;
    for (unsigned y = 0; y < r; ++y)
    {
        string row;
        fi >> row;
        for (unsigned x = 0; x < c; ++x)
        {
            grid.put(x, y, row[x] == '@');
        }
    }
}

void Waffle::solve_naive()
{
    n_cookies = count_cookies(0, c, 0, r);
    if (dbg_flags & 0x1) { print_grid(); }
    bool possible = get_cuts();
    const unsigned nv = v + 1;
    const unsigned nh = h + 1;
    const unsigned portion = n_cookies / (nv * nh);

    unsigned xb = 0;
    for (auto xi = xcuts.begin(), xie = xcuts.end(); possible && (xi != xie);
        ++xi)
    {
        unsigned xe = *xi;
        unsigned yb = 0;
        for (auto yi = ycuts.begin(), yie = ycuts.end(); possible && (yi != yie);
            ++yi)
        {
            unsigned ye = *yi;
            unsigned nc = count_cookies(xb, xe, yb, ye);
            possible = possible && (nc == portion);
            yb = ye;
        }
        xb = xe;
    }
    solution = possible ? "POSSIBLE" : "IMPOSSIBLE";
}

bool Waffle::get_cuts()
{
    const unsigned nv = v + 1;
    const unsigned nh = h + 1;
    bool possible = (n_cookies % nv == 0) && (n_cookies % nh == 0);

    const unsigned v_portion = n_cookies / nv;
    const unsigned h_portion = n_cookies / nh;

    // vertical cuts
    unsigned xe = 0;
    for (unsigned xcut = 0; possible && (xcut < v + 1); ++ xcut)
    {
        unsigned curr_xcut = 0;
        unsigned xb = xe, x = xb;
        for (; (x < c) && (curr_xcut < v_portion); ++x)
        {
            unsigned n_cookies_col = count_cookies(x, x + 1, 0, r);
            curr_xcut += n_cookies_col;
        }
        possible = (curr_xcut == v_portion);
        xe = x;
        xcuts.push_back(xe);
    }

    // horizontal cuts
    unsigned ye = 0;
    for (unsigned ycut = 0; possible && (ycut < h + 1); ++ ycut)
    {
        unsigned curr_ycut = 0;
        unsigned yb = ye, y = yb;
        for (; (y < r) && (curr_ycut < h_portion); ++y)
        {
            unsigned n_cookies_row = count_cookies(0, c, y, y + 1);
            curr_ycut += n_cookies_row;
        }
        possible = (curr_ycut == h_portion);
        ye = y;
        ycuts.push_back(ye);
    }
    return possible;
}

void Waffle::solve()
{
    solve_naive();
}

unsigned Waffle::count_cookies(
    unsigned xb, unsigned xe, unsigned yb, unsigned ye) const
{
    unsigned ret = 0;
    const mtxb_t &grid = *pgrid;
    for (unsigned x = xb; x < xe; ++x)
    {
        for (unsigned y = yb; y < ye; ++y)
        {
            if (grid.get(x, y))
            {
                ++ret;
            }
        }
    }
    return ret;
}

void Waffle::print_solution(ostream &fo) const
{
   fo << ' ' << solution;
}

void Waffle::print_grid() const
{
    const mtxb_t &grid = *pgrid;
    for (unsigned y = 0; y < r; ++y)
    {
        for (unsigned x = 0; x < c; ++x)
        {
            bool cookie = grid.get(x, y);
            char cc = cookie ? '@' : '.';
            cerr << cc;
        }
        cerr << "\n";
    }
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

    void (Waffle::*psolve)() =
        (naive ? &Waffle::solve_naive : &Waffle::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Waffle waffle(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (waffle.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        waffle.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
