// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2018/April/13

#include <iostream>
#include <fstream>
#include <string>
// #include <set>
// #include <map>
// #include <vector>
#include <utility>

#include <cstdlib>
// #include <gmpxx.h>

using namespace std;

// typedef mpz_class mpzc_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
// typedef vector<ul_t> vul_t;

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

typedef WHMatrix<char> mtxc_t;

static unsigned dbg_flags;

class Rotate
{
 public:
    Rotate(istream& fi);
    ~Rotate() { delete porig; }
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    void rotate(mtxc_t &result, const mtxc_t &m) const;
    void gravity(mtxc_t &m) const;
    void decide(const mtxc_t &m);
    unsigned c_line_longest(const mtxc_t &m, char c,
        int x0, int y0, int dx, int dy) const;
    void mtx_print(const mtxc_t &m, const char *label="") const;
    unsigned n, k;
    mtxc_t *porig;
    const char *solution;
};

Rotate::Rotate(istream& fi) : porig(0), solution("")
{
    fi >> n >> k;
    porig = new mtxc_t{n, n};
    mtxc_t &orig = *porig;
    for (unsigned y = n; y > 0;)
    {
        --y;
        string row;
        fi >> row;
        for (unsigned x = 0; x < n; ++x)
        {
            char c = row[x];
            orig.put(x, y, c);
        }
    }
}

void Rotate::solve_naive()
{
    const mtxc_t &orig = *porig;
    mtxc_t m{orig.w, orig.h};
    if (dbg_flags & 0x1) { mtx_print(orig, "orig"); }
    rotate(m, orig);
    if (dbg_flags & 0x1) { mtx_print(m, "rotated"); }
    gravity(m);
    if (dbg_flags & 0x1) { mtx_print(m, "gravity"); }
    decide(m);
}

void Rotate::solve()
{
    solve_naive();
}

void Rotate::rotate(mtxc_t &result, const mtxc_t &m) const
{
    for (unsigned x = 0; x < m.w; ++x)
    {
        for (unsigned y = 0; y < m.h; ++y)
        {
            char c = m.get(x, y);
            result.put(y, m.h - x - 1, c);
        }
    }
}

void Rotate::gravity(mtxc_t &m) const
{
    for (unsigned x = 0; x < m.w; ++x)
    {
        unsigned yin = 0, yout = 0;
        for (; yin != m.h; ++yin)
        {
            char c = m.get(x, yin);
            if (c != '.')
            {
                m.put(x, yout, c);
                ++yout;
            }
        }
        for (; yout != m.h; ++yout)
        {
            m.put(x, yout, '.');
        } 
    }
}

static void maximize(unsigned &var, unsigned val)
{
    if (var < val)
    {
        var = val;
    }
}

void Rotate::decide(const mtxc_t &m)
{
    bool rb_win[2] = {false, false};
    for (unsigned i = 0; i != 2; ++i)
    {
        const char cRB = "RB"[i];
        bool win = false;

        for (unsigned x = 0; (x != m.w) && !win; ++x)
        {
            win = (c_line_longest(m, cRB, x, 0, 0, 1) >= k);
        }

        for (unsigned y = 0; (y != m.h) && !win; ++y)
        {
            win = (c_line_longest(m, cRB, 0, y, 1, 0) >= k);
        }

        // north-east
        for (unsigned xy = 0; (xy < m.h) && !win; ++xy)
        {
            win =
                (c_line_longest(m, cRB, 0, xy, 1, 1) >= k) ||
                (c_line_longest(m, cRB, xy, 0, 1, 1) >= k);
        }
        // south-east
        for (unsigned xy = 0; (xy < m.h) && !win; ++xy)
        {
            win =
                (c_line_longest(m, cRB, 0, xy, 1, -1) >= k) ||
                (c_line_longest(m, cRB, xy, m.h - 1, 1, -1) >= k);
        }

        rb_win[i] = win;
    }
    if (rb_win[0])
    {
        solution = rb_win[1] ? "Both" : "Red";
    }
    else
    {
        solution = rb_win[1] ? "Blue" : "Neither";
    }
}

unsigned Rotate::c_line_longest(
    const mtxc_t &m, char c, int x0, int y0, int dx, int dy) const
{
    unsigned longest = 0;
    unsigned n_curr = 0;
    for (int x = x0, y = y0;
        (0 <= x && x < int(m.w)) && (0 <= y && y < int(m.h));
        x += dx, y += dy)
    {
        char cxy = m.get(x, y);
        if (cxy == c)
        {
            ++n_curr;
            maximize(longest, n_curr);
        }
        else
        {
            n_curr = 0;
        }
    }
    return longest;
}

void Rotate::mtx_print(const mtxc_t &m, const char *label) const
{
    cerr << label << "\n";
    for (unsigned y = m.h; y > 0;)
    {
        --y;
        for (unsigned x = 0; x < m.w; ++x)
        {
            cerr << m.get(x, y);
        }
        cerr << "\n";
    }
}

void Rotate::print_solution(ostream &fo) const
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

    void (Rotate::*psolve)() =
        (naive ? &Rotate::solve_naive : &Rotate::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Rotate rotate(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (rotate.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        rotate.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
