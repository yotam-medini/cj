// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <utility>
#include <vector>
#include <array>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
typedef array<u_t, 2> au2_t;
typedef pair<u_t, au2_t> vij_t;

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

typedef Matrix<u_t> umat_t;



class Rabbit
{
 public:
    Rabbit(istream& fi);
    ~Rabbit() { delete pmat; }
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    ull_t get_solution() const { return solution; }
 private:
    bool increase_cell(u_t i, u_t j);
    u_t R, C;
    umat_t* pmat;
    ull_t solution;
};

Rabbit::Rabbit(istream& fi) : solution(0)
{
    fi >> R >> C;
    pmat = new umat_t(R, C);
    for (u_t i = 0; i < R; ++i)
    {
        vu_t row; row.reserve(C);
        copy_n(istream_iterator<u_t>(fi), C, back_inserter(row));
        for (u_t j = 0; j < C; ++j)
        {
            pmat->put(i, j, row[j]);
        }
    }
}

void Rabbit::solve_naive()
{
    bool adding = true;
    while (adding)
    {
        adding = false;
        for (u_t i = 0; i < R; ++i)
        {
            for (u_t j = 0; j < C; ++j)
            {
                adding = increase_cell(i, j) || adding;
            }
        }
    }
}

void Rabbit::solve()
{
    vector<vij_t> vijs; vijs.reserve(R*C);
    umat_t& mat = *pmat;
    for (u_t i = 0; i < R; ++i)
    {
        for (u_t j = 0; j < C; ++j)
        {
            vijs.push_back(make_pair(mat.get(i, j), au2_t{i, j}));
        }
    }
    sort(vijs.begin(), vijs.end());
    for (u_t i = 0; i < R; ++i)
    {
        for (u_t j = 0; j < C; ++j)
        {
            ull_t v = mat.get(i, j), vneed = v;
            for (int k = R*C - 1; k >= 0; --k)
            {
                const vij_t& vij = vijs[k];
                ull_t vbig = vij.first;
                const au2_t& ij = vij.second;
                ull_t di = i < ij[0] ? ij[0] - i : i - ij[0];
                ull_t dj = j < ij[1] ? ij[1] - j : j - ij[1];
                ull_t dist = di + dj;
                if (dist < vbig)
                {
                    vneed = max(vneed, vbig - dist);
                }
            }
            solution += (vneed - v);
        }
    }
}

bool Rabbit::increase_cell(u_t i, u_t j)
{
    umat_t& mat = *pmat;
    bool increased = false;
    u_t v = mat.get(i, j), neigbor_max = v;
    if (i > 0)
    {
        neigbor_max = max(neigbor_max, mat.get(i - 1, j));
    }
    if (i + 1 < R)
    {
        neigbor_max = max(neigbor_max, mat.get(i + 1, j));
    }
    if (j > 0)
    {
        neigbor_max = max(neigbor_max, mat.get(i, j - 1));
    }
    if (j + 1 < C)
    {
        neigbor_max = max(neigbor_max, mat.get(i, j + 1));
    }
    if (v + 1 < neigbor_max)
    {
        u_t vnew = neigbor_max - 1;
        u_t add = vnew - v;
        mat.put(i, j, vnew);
        solution += add;
        increased = true;
    }
    return increased;
}

void Rabbit::print_solution(ostream &fo) const
{
    fo << ' ' << solution;
}

static int test(int argc, char ** argv)
{
    static const char *fn = "rabbit-auto.in";
    static const char *fn_large = "rabbit-auto-large.in";
    int rc = 0;
    int ai = 0;
    u_t n_tests = strtoul(argv[ai++], 0, 0);
    u_t max_rc = strtoul(argv[ai++], 0, 0);
    u_t max_h = strtoul(argv[ai++], 0, 0);
    int dummy_nt;
    for (u_t ti = 0; (rc == 0) && (ti < n_tests); ++ti)
    {
        cerr << "Tested: " << ti << '/' << n_tests << '\n';
        u_t R = rand() % max_rc + 1;
        u_t C = rand() % max_rc + 1;
        bool large = (R > 50 || C > 50);
        const char *tfn = large ? fn_large : fn;
        {
            ofstream of(tfn);
            of << "1\n" << R << ' ' << C << '\n';
            for (u_t i = 0; i < R; ++i)
            {
                const char *sep = "";
                for (u_t j = 0; j < C; ++j)
                {
                    of << sep << (rand() % max_h); sep = " ";
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
            Rabbit rabbit(fi);
            if (rn == 0)
            {
                rabbit.solve();
            }
            else
            {
                rabbit.solve_naive();
            }
            solutions[rn] = rabbit.get_solution();
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

    void (Rabbit::*psolve)() =
        (naive ? &Rabbit::solve_naive : &Rabbit::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Rabbit rabbit(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (rabbit.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        rabbit.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
