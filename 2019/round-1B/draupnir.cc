// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2018/April/12

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <utility>

#include <cstdlib>
#include <cmath>
#include <limits>
#include <unistd.h>


using namespace std;

// typedef mpz_class mpzc_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<int> vi_t;
// typedef vector<ul_t> vul_t;

static unsigned dbg_flags;

class ErrLog
{
 public:
    ErrLog(const char *fn=0) : _f(fn ? new ofstream(fn) : 0) {}
    ~ErrLog() { delete _f; }
    template <class T>
    ErrLog& operator<<(const T &x)
    {
        if (_f)
        {
            cerr << x;
            (*_f) << x;
        }
        return *this;
    }
    void flush() { if (_f) { _f->flush(); } }
    bool active() const { return (_f != 0); }
 private:
    ofstream *_f;
};


class BaseMatrix
{
  public:
    BaseMatrix(unsigned _m, unsigned _n) : m(_m), n(_n) {}
    const unsigned m; // rows
    const unsigned n; // columns
  protected:
    unsigned rc2i(unsigned r, unsigned c) const 
    {
        unsigned ret = r*n + c;
        return ret;
    }
    void i2rc(unsigned &r, unsigned &c, unsigned i) const 
    {
        r = i / n;
        c = i % n;
    }
};

template <class T>
class Matrix : public BaseMatrix
{
  public:
    Matrix(unsigned _m, unsigned _n) : BaseMatrix(_m, _n), _a(new T[m *n]) {}
    virtual ~Matrix() { delete [] _a; }
    const T& get(unsigned r, unsigned c) const { return _a[rc2i(r, c)]; }
    void put(unsigned r, unsigned c, const T &v) const { _a[rc2i(r, c)] = v; }
  private:
    T *_a;
};


typedef double lineq_scalar_t;
typedef std::vector<lineq_scalar_t> vscalar_t;
typedef Matrix<lineq_scalar_t> lineq_mtx_t;

double lineq_eps = numeric_limits<float>::epsilon();

static void eliminate(lineq_mtx_t &aa, vscalar_t &bb, unsigned i, unsigned k)
{
    lineq_scalar_t q = aa.get(k, i);
    lineq_scalar_t dq = 1./q;

    // Swap rows i,k and canonize new i-row
    lineq_scalar_t t = bb[k];
    bb[k] = bb[i];
    bb[i] = t*dq;
    for (unsigned j = i; j < aa.n; ++j)
    {
        t = aa.get(k, j);
        aa.put(k, j, aa.get(i, j));
        t *= dq;
        aa.put(i, j, t);
    }
    aa.put(i, i, 1); // cleaner one instead of q*dq

    lineq_scalar_t bbi = bb[i];
    for (unsigned low = i + 1; low < aa.m; ++low)
    {
        t = aa.get(low, i);
        if (fabs(t) > lineq_eps)
        {
            for (unsigned j = i + 1; j < aa.n; ++j)
            {
                 lineq_scalar_t v = aa.get(low, j);
                 v -= t * aa.get(i, j);
                 aa.put(low, j, v);
            }
        }
        aa.put(low, i, 0);
        bb[low] -= t*bbi;
    }
}

static void diagonalize(lineq_mtx_t &aa, vscalar_t &bb)
{
    for (unsigned j = aa.n; j > 0; )
    {
        --j;
        // zero i-column above j-row
        lineq_scalar_t bbj = bb[j];
        for (unsigned i = 0; i < j; ++i)
        {
            lineq_scalar_t q = aa.get(i, j);
            aa.put(i, j, 0);
            bb[i] -= q*bbj;
        }
    }
}

bool lineq_solve(vscalar_t &x, const lineq_mtx_t &a, const vscalar_t &b)
{
    bool ok = (a.m == a.n) && (a.m == b.size());
    if (ok)
    {
        lineq_mtx_t aa{a.m, a.n}; // copy
        vscalar_t bb(b);
        for (unsigned i = 0; i < a.m; ++i)
        {
            for (unsigned j = 0; j < a.n; ++j)
            {
                aa.put(i, j, a.get(i, j));
            }
        }

        vector<unsigned> perm;
        perm.reserve(a.m);
        for (unsigned i = 0; ok && (i < a.m); ++i)
        {
            perm.push_back(i);
        }
        for (unsigned i = 0; ok && (i < a.m); ++i)
        {
            // find largest pivot
            unsigned imax = i;
            for (unsigned k = imax + 1; k < a.m; ++k)
            {
                if (fabs(aa.get(imax, i)) < fabs(aa.get(k, i)))
                {
                    imax = k;
                }
            }
            ok = fabs(aa.get(imax, i)) > lineq_eps;
            if (ok)
            {
                eliminate(aa, bb, i, imax);
                swap(perm[i], perm[imax]);
            }
        }
        if (ok)
        {
            diagonalize(aa, bb);
            x = bb;
        }
    }
    return ok;
}


class Draupnir
{
 public:
    Draupnir(istream& fi, ErrLog &el);
    void solve_naive(istream& fi, ostream &fo);
    void solve(istream& fi, ostream &fo);
    void print_solution(ostream&) const;
 private:
    bool readline_ints(istream &fi, vi_t &v);
    ErrLog &errlog;
};

bool Draupnir::readline_ints(istream &fi, vi_t &v)
{
   v.clear();
   string line;
   getline(fi, line);
   istringstream  iss(line);
   while (!iss.eof())
   {
       int x;
       iss >> x;
       if (!iss.fail())
       {
           v.push_back(x);
       }
   }
   return fi.eof();
}

Draupnir::Draupnir(istream& fi, ErrLog &el) : errlog(el)
{
}

void Draupnir::solve_naive(istream& fi, ostream &fo)
{
    vi_t v;
    readline_ints(fi, v);
    int t = v[0], w = v[1];
    if (w == 0) { exit(1); }

    lineq_mtx_t A(6, 6);
    double da[6][6] = {
      { 2, 1, 1, 1, 1, 1},
      { 4, 2, 1, 1, 1, 1},
      { 8, 2, 2, 1, 1, 1},
      {16, 4, 2, 2, 1, 1},
      {32, 4, 2, 2, 2, 1},
      {64, 8, 4, 2, 2, 2},
    };
    
    for (int i = 0; i < 6; ++i)
    {
        for (int j = 0; j < 6; ++j)
        {
            A.put(i, j, da[i][j]);
        }
    }

    for (int ti = 0; ti < t; ++ti)
    {
        vscalar_t b, x;
        vi_t ints;
        for (int si = 0; si < 6; ++si)
        {
            fo << si + 1 << "\n"; fo.flush();
            readline_ints(fi, ints);
            b.push_back(ints[0]);
        }
        lineq_solve(x, A, b);
        const char *sep = "";
        for (int si = 0; si < 6; ++si)
        {
            fo << sep << round(x[si]);  sep = " ";
        }
        fo << "\n"; fo.flush();
        readline_ints(fi, ints);
        int verdict = ints[0];
        if (verdict == -1)
        {
            exit(1);
        }
    }
}

void Draupnir::solve(istream& fi, ostream &fo)
{
    solve_naive(fi, fo);
}

void Draupnir::print_solution(ostream &fo) const
{
}

int main(int argc, char ** argv)
{
    const string dash("-");

    bool naive = false;
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

    ErrLog errlog(dbg_flags & 0x1 ? "/tmp/ymcj.log" : 0);
    if (dbg_flags & 0x1) 
    {
        errlog << "pid = " << getpid() << "\n"; errlog.flush();
        int slept = 0;
        while (slept < 90)
        {
            sleep(1);
            ++slept;
        }
    }

    string ignore;
    void (Draupnir::*psolve)(istream&, ostream&) =
        (naive ? &Draupnir::solve_naive : &Draupnir::solve);
    for (unsigned ci = 0; ci < 1; ci++)
    {
        Draupnir problem(*pfi, errlog);
        (problem.*psolve)(*pfi, *pfo);
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
