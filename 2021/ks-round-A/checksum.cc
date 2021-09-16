// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <iterator>
#include <array>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include <unordered_map>

#include <cstdlib>
#include <cstdint>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<uint8_t> vu8_t;
typedef set<u_t> setu_t;
typedef array<u_t, 2> au2_t;
typedef array<u_t, 3> au3_t;
typedef vector<au2_t> vau2_t;
typedef vector<au3_t> vau3_t;
typedef unordered_map<uint8_t, uint8_t> u2u_t;

static unsigned dbg_flags;

u_t u2u_get(const u2u_t& u2u, u_t k, u_t defval=0)
{
    u2u_t::const_iterator iter = u2u.find(k);
    u_t ret = (iter == u2u.end() ? defval : iter->second);
    return ret;
}

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

template <class T>
void print_mat(const string& msg, const Matrix<T>& mat)
{
    cerr << msg << '\n';
    for (u_t i = 0; i < mat.m; ++i)
    {
        const char *sep = "";
        for (u_t j = 0; j < mat.n; ++j)
        {
            cerr << sep << setw(3) << int(mat.get(i, j)); 
            sep = "  ";
        }
        cerr << '\n';
    }
}

typedef Matrix<int8_t> i8mat_t;
typedef Matrix<u_t> umat_t;

static void reduce(vau2_t& reduced, i8mat_t& mat, const vu8_t& R, const vu8_t& C)
{
    reduced.clear();
    bool reducing = true;
    setu_t rows, cols;
    for (u_t i = 0; i < mat.m; rows.insert(i++)) {}
    for (u_t j = 0; j < mat.n; cols.insert(j++)) {}
    u_t loops = 0;
    while (reducing)
    {
        setu_t new_rows, new_cols;
        reducing = false;
        for (u_t i: rows)
        {
            u_t col = mat.n; // undef
            u_t undef = 0;
            u_t new_val = 0;
            for (u_t j = 0; (j < mat.n) && (undef < 2); ++j)
            {
                int8_t v = mat.get(i, j);
                if (v == -1)
                {
                    ++undef;
                    col = j;
                }
                else
                {
                    new_val ^= v;
                }
            }
            if (undef == 1)
            {
                reducing = true;
                mat.put(i, col, new_val);
                new_cols.insert(col);
                reduced.push_back(au2_t{i, col});
            }
        }
        if (loops > 0) { swap(cols, new_cols); }
        for (u_t j: cols)
        {
            u_t row = mat.m; // undef
            u_t undef = 0;
            u_t new_val = 0;
            for (u_t i = 0; (i < mat.m) && (undef < 2); ++i)
            {
                int8_t v = mat.get(i, j);
                if (v == -1)
                {
                    ++undef;
                    row = i;
                }
                else
                {
                    new_val ^= v;
                }
            }
            if (undef == 1)
            {
                reducing = true;
                mat.put(row, j, new_val);
                new_rows.insert(row);
                reduced.push_back(au2_t{row, j});
            }
        }
        swap(rows, new_rows);
    }
}

class Checksum
{
 public:
    Checksum(istream& fi);
    ~Checksum()
    {
        delete pA;
        delete pB;
        delete psA;
    }
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    ull_t get_solution() const { return solution; }
 private:
    u_t count_unknowns(setu_t& rows, setu_t& cols) const;
    u_t reduce_by(setu_t& rows, setu_t& cols); // return # reduced
    u_t N;
    i8mat_t *pA;
    umat_t *pB;
    i8mat_t *psA;
    vu8_t R, C;
    ull_t solution;
};

Checksum::Checksum(istream& fi) : 
    pA(nullptr),
    pB(nullptr), 
    psA(nullptr), 
    solution(ull_t(-1))
{
    fi >> N;
    pA = new i8mat_t(N, N);
    pB = new umat_t(N, N);
    for (u_t i = 0; i < N; ++i)
    {
        for (u_t j = 0; j < N; ++j)
        {
            int x;
            fi >> x;
            pA->put(i, j, x);
        }
    }
    if (dbg_flags & 0x1) { print_mat("A:", *pA); }
    for (u_t i = 0; i < N; ++i)
    {
        for (u_t j = 0; j < N; ++j)
        {
            u_t x;
            fi >> x;
            pB->put(i, j, x);
        }
    }
    if (dbg_flags & 0x1) { print_mat("B:", *pB); }
    copy_n(istream_iterator<uint8_t>(fi), N, back_inserter(R));
    copy_n(istream_iterator<uint8_t>(fi), N, back_inserter(C));
}

void Checksum::solve_naive()
{
    vau2_t unknowns;
    for (u_t i = 0; i < N; ++i)
    {
        for (u_t j = 0; j < N; ++j)
        {
            if (pA->get(i, j) == -1)
            {
                unknowns.push_back(au2_t{i, j});
            }
        }
    }
    const u_t n_unknowns = unknowns.size();
    for (u_t mask = 0, mask_max = 1u << n_unknowns; mask < mask_max; ++mask)
    {
        i8mat_t mat(pA->m, pA->n);
        for (u_t i = 0; i < N; ++i)
        {
            for (u_t j = 0; j < N; ++j)
            {
                mat.put(i, j, pA->get(i, j));
            }
        }
        u_t cost = 0;
        u_t nq = 0;
        for (u_t k = 0; k < n_unknowns; ++k)
        {
            if (mask & (1u << k))
            {
                const au2_t& ij = unknowns[k];
                ++nq;
                mat.put(ij[0], ij[1], 0);
                cost += pB->get(ij[0], ij[1]);
            }
        }
        vau2_t reduced;
        reduce(reduced, mat, R, C);
        if (reduced.size() + nq == n_unknowns)
        {
            if (solution > cost)
            {
                solution = cost;
            }            
        }            
    }
}

void Checksum::solve()
{
     typedef array<u_t, 3> u3_t;
     typedef vector<u3_t> vu3_t;
     psA = new i8mat_t(N, N);
     vu3_t bijs; bijs.reserve(N*N);
     ull_t b_total = 0;
     for (u_t i = 0; i < N; ++i)
     {
         for (u_t j = 0; j < N; ++j)
         {
             psA->put(i, j, pA->get(i, j));
             u_t b = pB->get(i, j);
             if (b > 0)
             {
                 bijs.push_back(u3_t{b, i, j});
                 b_total += b;
             }
         }
     }
     sort(bijs.begin(), bijs.end());
#if 0
     vau3_t avoid; avoid.reserve(2*N);
     u2u_t rc_count[2];
     for (u_t bi = N*N - 1; avoid.size() < 2*N - 1; --bi)
     {
         const u3_t& bij = bijs[bi];
         u_t rc[2];
         for (u_t k: {0, 1})
         {
             rc[k] = u2u_get(rc_count[k], bij[1 + k], 0);
         }
         bool may_quad = (rc[0] >= 1) && (rc[1] >= 1) && (
            (rc[0] >= 2) || (rc[1] >= 2));
         if (!may_quad)
         {
             avoid.push_back(bij);
             b_total -= bij[0];
         }
     }
#endif
     setu_t rows, cols; // unfilled
     u_t n_unknowns = count_unknowns(rows, cols);
     solution = 0;
     while (n_unknowns > 0)
     {
         u_t n_reduced = reduce_by(rows, cols);
         n_unknowns -= n_reduced;
         if (n_unknowns > 0)
         {
             u_t bi = 0;
             for (; psA->get(bijs[bi][1], bijs[bi][2]) != -1; ++bi)
             {
                 ;
             }
             const u3_t& bij = bijs[bi];
             u_t i = bij[1], j = bij[2];
             rows.clear(); cols.clear();
             rows.insert(i);
             cols.insert(j);
             psA->put(i, j, 0); // arbitrary !?
             solution += bij[0];
             --n_unknowns;
         }
     }
}

void Checksum::print_solution(ostream &fo) const
{
    fo << ' ' << solution;
}

u_t Checksum::count_unknowns(setu_t& rows, setu_t& cols) const
{
    u_t n = 0;
    for (u_t i = 0; i < N; ++i)
    {
        for (u_t j = 0; j < N; ++j)
        {
            bool is_unknown_a = (pA->get(i, j) == -1);
            bool is_unknown_b = (pB->get(i, j) > 0);
            if (is_unknown_a != is_unknown_b)
            {
                cerr << "bad input @ij=("<<i << ", "<<j << ")\n";
                exit(1);
            }
            if (is_unknown_a)
            {
                rows.insert(i);
                cols.insert(j);
                ++n;
            }
        }
    }
    return n;
}

u_t Checksum::reduce_by(setu_t& rows, setu_t& cols)
{
    u_t n_reduced = 0;
    bool reducing = true;
    u_t loops = 0;
    i8mat_t& mat = *psA;
    while (reducing)
    {
        setu_t new_rows, new_cols;
        reducing = false;
        for (u_t i: rows)
        {
            u_t col = mat.n; // undef
            u_t undef = 0;
            u_t new_val = 0;
            for (u_t j = 0; (j < mat.n) && (undef < 2); ++j)
            {
                int8_t v = mat.get(i, j);
                if (v == -1)
                {
                    ++undef;
                    col = j;
                }
                else
                {
                    new_val ^= v;
                }
            }
            if (undef == 1)
            {
                reducing = true;
                mat.put(i, col, new_val);
                new_cols.insert(col);
                ++n_reduced;
            }
        }
        if (loops > 0) { swap(cols, new_cols); }
        for (u_t j: cols)
        {
            u_t row = mat.m; // undef
            u_t undef = 0;
            u_t new_val = 0;
            for (u_t i = 0; (i < mat.m) && (undef < 2); ++i)
            {
                int8_t v = mat.get(i, j);
                if (v == -1)
                {
                    ++undef;
                    row = i;
                }
                else
                {
                    new_val ^= v;
                }
            }
            if (undef == 1)
            {
                reducing = true;
                mat.put(row, j, new_val);
                new_rows.insert(row);
                ++n_reduced;
            }
        }
        swap(rows, new_rows);
    }
    return n_reduced;
}

static int research(int argc, char ** argv)
{
    int rc = 0;
    int ai = 0;
    u_t n_max = strtoul(argv[ai++], 0, 0);
    for (u_t N = 1; N <= n_max; ++N)
    {
        const vu8_t R(N, 0), C(N, 0);
        u_t solvable_unknown_min = N*N, solvable_unknown_max = 0;
        u_t unsolvable_unknown_min = N*N, unsolvable_unknown_max = 0;
        for (u_t mask = 0, mask_max = 1u << (N*N); mask < mask_max; ++mask)
        {
            i8mat_t mat(N, N);
            u_t n_unknowns = 0;
            for (u_t ij = 0; ij < N*N; ++ij)
            {
                u_t i = ij / N, j = ij % N;
                if (mask & (1u << ij))
                {
                    ++n_unknowns;
                    mat.put(i, j, -1);
                }
                else
                {
                    mat.put(i, j, 0);
                }
            }
            vau2_t reduced;
            reduce(reduced, mat, R, C);
            u_t n_reduced = reduced.size();
            if (dbg_flags & 0x2) { cerr << "n_unknowns="<<n_unknowns <<
                ", reduced="<<n_reduced << '\n'; }
            if (n_reduced == n_unknowns)
            {
                solvable_unknown_max = max(solvable_unknown_max, n_unknowns);
                solvable_unknown_min = min(solvable_unknown_min, n_unknowns);
            }
            else
            {
                unsolvable_unknown_max = max(unsolvable_unknown_max, n_unknowns);
                unsolvable_unknown_min = min(unsolvable_unknown_min, n_unknowns);
            }
        }
        cerr << "N="<<N << 
            ", Solvable: min="<<solvable_unknown_min <<
            ", max="<<solvable_unknown_max <<
            ", Unsolvable: min="<<unsolvable_unknown_min <<
            ", max="<<unsolvable_unknown_max << '\n';
    }
    return rc;
}

static int test(int argc, char ** argv)
{
    int rc = 0;
    int ai = 0;
    u_t n_tests = strtoul(argv[ai++], 0, 0);
    for (u_t ti = 0; (rc == 0) && (ti < n_tests); ++ti)
    {
        cerr << "Tested: " << ti << '/' << n_tests << '\n';
    }
    return rc;
}

int main(int argc, char ** argv)
{
    const string dash("-");

    bool naive = false;
    bool tellg = false;
    u_t solve_ver = 0;
    int rc = 0, ai;

    for (ai = 1; (rc == 0) && (ai < argc) && (argv[ai][0] == '-') &&
        argv[ai][1] != '\0'; ++ai)
    {
        const string opt(argv[ai]);
        if (opt == string("-naive"))
        {
            naive = true;
        }
        else if (opt == string("-solve1"))
        {
            solve_ver = 1;
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
        else if (opt == string("-research"))
        {
            rc = research(argc - (ai + 1), argv + (ai + 1));
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

    void (Checksum::*psolve)() =
        (naive ? &Checksum::solve_naive : &Checksum::solve);
    if (solve_ver == 1) { psolve = &Checksum::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Checksum checksum(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (checksum.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        checksum.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
