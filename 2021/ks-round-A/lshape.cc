// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;

static unsigned dbg_flags;

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

typedef Matrix<bool> bmat_t;
typedef Matrix<u_t> umat_t;

static void umat_show(const umat_t& mat)
{
    for (u_t i = 0; i < mat.m; ++i)
    {
        for (u_t j = 0; j < mat.n; ++j)
        {
            cerr << "  " << mat.get(i, j);
        }
        cerr << '\n';
    }
}

class Seg
{
 public:
    Seg(u_t _i=0, u_t _j=0, u_t _len=0) : i(_i), j(_j), len(_len) {}
    u_t i, j, len;
};
typedef vector<Seg> vseg_t;

class LShape
{
 public:
    LShape(istream& fi);
    ~LShape() { delete pmat; }
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    ull_t get_solution() const { return solution; }
 private:
    void get_segments();
    ull_t lcount(const Seg& hseg, const Seg& vseg) const;
    ull_t add2(int a, int b) const;
    u_t R, C;
    bmat_t* pmat;
    ull_t solution;
    vseg_t h_segs, v_segs;
};

LShape::LShape(istream& fi) : solution(0)
{
    fi >> R >> C;
    pmat = new bmat_t(R, C);
    for (ull_t i = 0; i < R; ++i)
    {
        vu_t row; row.reserve(C);
        copy_n(istream_iterator<u_t>(fi), C, back_inserter(row));
        for (u_t j = 0; j < C; ++j)
        {
            pmat->put(i, j, (row[j] != 0));
        }
    }
}

void LShape::solve_naive()
{
    get_segments();
    for (const Seg& hseg: h_segs)
    {
        for (const Seg& vseg: v_segs)
        {
            solution += lcount(hseg, vseg);
        }
    }
}

void LShape::solve()
{
    ull_t n = 0;
    umat_t left_size(R, C);
    umat_t right_size(R, C);
    for (u_t i = 0; i < R; ++i)
    {
        for (u_t j = 0; j < C;)
        {
            for (  ; (j < C) && !pmat->get(i, j); ++j)
            {
                left_size.put(i, j, 0);
                right_size.put(i, j, 0);
            }
            const int j0 = j;
            for (  ; (j < C) && pmat->get(i, j); ++j)
            {
                left_size.put(i, j, j + 1 - j0);
            }
            for (int jd = j - 1; jd >= j0; --jd)
            {
                right_size.put(i, jd, j - jd);
            }
        }
    }
    if (dbg_flags & 0x1) { cerr << "left_size:\n"; umat_show(left_size); }
    if (dbg_flags & 0x1) { cerr << "right_size:\n"; umat_show(right_size); }
    for (u_t j = 0; j < C; ++j)
    {
        for (u_t i = 0; i < R; )
        {
            for (  ; (i < R) && !pmat->get(i, j); ++i)
            {
            }
            const int i0 = i;
            u_t up = 1;
            for (  ; (i < R) && pmat->get(i, j); ++i, ++up)
            {
                const u_t left = left_size.get(i, j);
                const u_t right = right_size.get(i, j);
                n += add2(left, up);
                n += add2(right, up);
            }
            u_t down = 1;
            for (int idown = i - 1; idown >= i0; --idown, ++down)
            {
                const u_t left = left_size.get(idown, j);
                const u_t right = right_size.get(idown, j);
                n += add2(left, down);
                n += add2(right, down);
            }
        }
    }
    solution = n;
}

void LShape::get_segments()
{
    const bmat_t& mat = *pmat;
    for (u_t i = 0; i < R; ++i)
    {
        for (u_t j = 0; j < C; )
        {
            for (; (j < C) && (!mat.get(i, j)); ++j) {}
            u_t jb = j;
            for (; (j < C) && (mat.get(i, j)); ++j) {}
            u_t je = j;
            u_t len = je - jb;
            if (len >= 2)
            {
                h_segs.push_back(Seg(i, jb, len));
            }
        }
    }
    for (u_t j = 0; j < C; ++j)
    {
        for (u_t i = 0; i < R; )
        {
            for (; (i < R) && (!mat.get(i, j)); ++i) {}
            u_t ib = i;
            for (; (i < R) && (mat.get(i, j)); ++i) {}
            u_t ie = i;
            u_t len = ie - ib;
            if (len >= 2)
            {
                v_segs.push_back(Seg(ib, j, len));
            }
        }
    }
}

ull_t LShape::lcount(const Seg& hseg, const Seg& vseg) const
{
    ull_t n = 0;
    if ((hseg.j <= vseg.j) && (vseg.j < hseg.j + hseg.len) &&
        (vseg.i <= hseg.i) && (hseg.i < vseg.i + vseg.len))
    {
        const u_t ix = hseg.i, jx = vseg.j;
        const u_t left = jx + 1 - hseg.j;
        const u_t right = hseg.len + 1 - left;
        const u_t up = ix + 1 - vseg.i;
        const u_t down = vseg.len + 1 - up;
        n += add2(left, up);
        n += add2(right, up);
        n += add2(left, down);
        n += add2(right, down);
    }
    return n;
}

ull_t LShape::add2(int a, int b) const
{
    ull_t n = 0;
    if ((a >= 2) && (b >= 2))
    {
        int n1 = min(a - 1, b/2 - 1);
        int n2 = min(b - 1, a/2 - 1);
        if (n1 > 0)
        {
            n += n1;
        }
        if (n2 > 0)
        {
            n += n2;
        }
    }
    return n;
}

void LShape::print_solution(ostream &fo) const
{
    fo << ' ' << solution;
}

static int test(int argc, char ** argv)
{
    static const char *fn = "lshape-auto.in";
    static const char *fn_large = "lshape-auto-large.in";
    int rc = 0;
    int ai = 0;
    u_t n_tests = strtoul(argv[ai++], 0, 0);
    u_t max_rc = strtoul(argv[ai++], 0, 0);
    int dummy_nt;
    for (u_t ti = 0; (rc == 0) && (ti < n_tests); ++ti)
    {
        cerr << "Tested: " << ti << '/' << n_tests << '\n';
        u_t R = rand() % max_rc + 1;
        u_t C = rand() % max_rc + 1;
        bool large = (R > 40 || C > 40);
        const char *tfn = large ? fn_large : fn;
        {
            ofstream of(tfn);
            of << "1\n" << R << ' ' << C << '\n';
            for (u_t i = 0; i < R; ++i)
            {
                const char *sep = "";
                for (u_t j = 0; j < C; ++j)
                {
                    of << sep << (rand() % 2); sep = " ";
                }
                of << '\n';
            }
            of.close();
        }
        ull_t solutions[2];
        for (u_t rn = 0; rn < (large ? 1 : 2); ++rn)
        {
            ifstream fi(fn);
            fi >> dummy_nt;
            LShape lshape(fi);
            if (rn == 0)
            {
                lshape.solve();
            }
            else
            {
                lshape.solve_naive();
            }
            solutions[rn] = lshape.get_solution();
        }
        if ((!large) && ( solutions[0] != solutions[1]))
        {
            cerr << "Inconsistent solution, real: " << solutions[0] <<
                " != naive: " << solutions[1] << '\n';
            rc = 1;
        }
    }
    return rc;
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
        else if (opt == string("-test"))
        {
            rc = test(argc - (ai + 1), argv + (ai + 1));
            return rc;
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

    void (LShape::*psolve)() =
        (naive ? &LShape::solve_naive : &LShape::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        LShape lshape(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (lshape.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        lshape.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
