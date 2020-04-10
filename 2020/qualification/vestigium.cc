// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <utility>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<bool> vb_t;

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

typedef Matrix<u_t> matu_t;

class Vestigium
{
 public:
    Vestigium(istream& fi);
    ~Vestigium() { delete pmat; }
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    u_t N;
    u_t solution_t, solution_r, solution_c;
    matu_t* pmat;
};

Vestigium::Vestigium(istream& fi) : 
    solution_t(u_t(-1)), solution_r(u_t(-1)), solution_c(u_t(-1))
{
    fi >> N;
    pmat = new matu_t(N, N);
    for (u_t r = 0; r < N; ++r)
    {
        for (u_t c = 0; c < N; ++c)
        {
            u_t x;
            fi >> x;
            pmat->put(r, c, x);
        }
    }
}

void Vestigium::solve_naive()
{
    solution_t = solution_r = solution_c = 0;

    for (u_t r = 0; r < N; ++r)
    {
        solution_t += pmat->get(r, r);
    }

    for (u_t r = 0; r < N; ++r)
    {
        vb_t have(size_t(N), false);
        bool dup = false;
        for (u_t c = 0; (c < N) && !dup; ++c)
        {
            u_t v = pmat->get(r, c);
            dup = have[v];
            have[v] = true;
        }
        solution_r += (dup ? 1 : 0);
    }

    for (u_t c = 0; c < N; ++c)
    {
        vb_t have(size_t(N), false);
        bool dup = false;
        for (u_t r = 0; (r < N) && !dup; ++r)
        {
            u_t v = pmat->get(r, c);
            dup = have[v];
            have[v] = true;
        }
        solution_c += (dup ? 1 : 0);
    }
}

void Vestigium::solve()
{
    solve_naive();
}

void Vestigium::print_solution(ostream &fo) const
{
    fo << ' ' << solution_t << ' ' << solution_r << ' ' << solution_c;
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

    void (Vestigium::*psolve)() =
        (naive ? &Vestigium::solve_naive : &Vestigium::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Vestigium vestigium(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (vestigium.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        vestigium.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
