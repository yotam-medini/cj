// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <iterator>
#include <array>
#include <list>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include <unordered_map>
#include <unordered_set>

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

static unsigned dbg_flags;

static string n2hex(ull_t x)
{
    ostringstream os;
    os << hex << x;
    return os.str();
}

class BaseMatrix
{
  public:
    BaseMatrix(unsigned _m, unsigned _n) : m(_m), n(_n) {}
    unsigned m; // rows
    unsigned n; // columns
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
    typedef Matrix<T> self_t;
    Matrix(unsigned _m, unsigned _n) : BaseMatrix(_m, _n), _a(new T[m * n]) {}
    Matrix(const self_t& x) : BaseMatrix(x.m, x.n), _a(new T[m *n]) 
    {
        copy(x._a, x._a + m*n, _a);
    }
    Matrix& operator=(const self_t& rhs)
    {
        if (this != &rhs)
        {
            if (m*n != rhs.m * rhs.n)
            {
                delete [] _a;
                _a = new T(rhs.m * rhs.n);
            }
            m = rhs.m;
            n = rhs.n;
            copy(rhs._a, rhs._a + m*n, _a);
        }
        return *this;
    }
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
        if (dbg_flags & 0x4) { cerr << __func__ << " loops="<<loops << '\n';}
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
                cols.insert(col);
                reduced.push_back(au2_t{i, col});
            }
        }
        rows.clear();
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
                rows.insert(row);
                reduced.push_back(au2_t{row, j});
            }
        }
        cols.clear();
    }
}

template <typename T> class SizedList; // fwd

template <typename T>
using list_pszl_t = list<SizedList<T>*>;

template <typename T>
class DisjointSetElement
{
 public:
    DisjointSetElement(const T& _v, SizedList<T>* _p) : v(_v), mylist(_p) {}
    T v;
    SizedList<T>* mylist;
};

template <typename T>
class SizedList
{
 public:
    typedef DisjointSetElement<T> element_t;
    typedef element_t *elementp_t;
    typedef list<elementp_t> list_t;
    SizedList<T>() : sz(0) {}
    elementp_t push_back(const T &e)
    {
        elementp_t setp = new element_t(e, this);
        l.push_back(setp);
        ++sz;
        return setp;
    }
    list_t l;
    u_t sz;
    typename list_pszl_t<T>::iterator selfref;
};

class DisjointSets
{
 public:
    typedef u_t key_t;
    typedef SizedList<key_t> szlist_t;
    typedef szlist_t::elementp_t elementp_t;
    typedef list_pszl_t<key_t> lszlp_t;

    virtual ~DisjointSets()
    {
        for (szlist_t *pszl: list_szlistp)
        {
            for (elementp_t e: pszl->l)
            {
                delete e;
            }
            delete pszl;
        }
    }

    elementp_t make_set(const key_t& k)
    {
        szlist_t *pszl = new szlist_t();
        elementp_t elemp = pszl->push_back(k);
        pszl->selfref = list_szlistp.insert(list_szlistp.end(), pszl);
        return elemp;
    }

    szlist_t* find_set(const elementp_t& elemp)
    {
        return elemp->mylist;
    }

    void union_sets(elementp_t e0, elementp_t e1)
    {
        szlist_t* pszl0 = find_set(e0);
        szlist_t* pszl1 = find_set(e1);
        if (pszl0 != pszl1)
        {
            if (pszl0->sz > pszl1->sz)
            {
                swap(pszl0, pszl1);
            }
            for (elementp_t e: pszl1->l)
            {
                e->mylist = pszl0;
            }
            pszl0->l.splice(pszl0->l.end(), pszl1->l);
            pszl0->sz += pszl1->sz;
            list_szlistp.erase(pszl1->selfref);
            delete pszl1;
        }
    }

    const lszlp_t& get_list_lists() const { return list_szlistp; }

 private:
    lszlp_t list_szlistp;
};

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
    // void solve1();
    void solve();
    void print_solution(ostream&) const;
    ull_t get_solution() const { return solution; }

 private:
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
    u_t best_mask = 0;
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
                best_mask = mask;
            }
        }
    }
    if (dbg_flags & 0x1) { 
        cerr << "mask = 0x" << n2hex(best_mask) << ", unknowns:"; 
        for (u_t k = 0; k < n_unknowns; ++k)
        {
            if (best_mask & (1u << k))
            {
                const au2_t& ij = unknowns[k];
                cerr << "  ("<<ij[0] << ", "<<ij[1] << ")";
            }
        }
        cerr << '\n';
    }
}

void Checksum::solve()
{
    typedef vector<DisjointSets::elementp_t> velemp_t;
    ull_t b_total = 0;
    setu_t rs, cs;
    vau3_t brcs;
    velemp_t r_elems(size_t(N), nullptr);
    velemp_t c_elems(size_t(N), nullptr);
    DisjointSets djs;
    for (u_t r = 0; r < N; ++r)
    {
        for (u_t c = 0; c < N; ++c)
        {
            u_t b = pB->get(r, c);
            if (b > 0)
            {
                b_total += b;
                brcs.push_back(au3_t{b, r, c});
            }
        }
    }
    ull_t pay = b_total;
    sort(brcs.begin(), brcs.end());
    for (vau3_t::const_reverse_iterator
        iter = brcs.rbegin(), eiter = brcs.rend(); iter != eiter; ++iter)
    {
        const au3_t &brc = *iter;
        const u_t b = brc[0], r = brc[1], c = brc[2];
        pair<setu_t::iterator, bool> iternew;
        iternew = rs.insert(r);
        if (iternew.second)
        {
            r_elems[r] = djs.make_set(r);
        }
        iternew = cs.insert(c);
        if (iternew.second)
        {
            c_elems[c] = djs.make_set(c + N);
        }
        if (r_elems[r]->mylist != c_elems[c]->mylist)
        {
            djs.union_sets(r_elems[r], c_elems[c]);
            pay -= b;
        }
    }
    solution = pay;
}

void Checksum::print_solution(ostream &fo) const
{
    fo << ' ' << solution;
}

static void rand_data(i8mat_t &A, umat_t &B, vu8_t &R, vu8_t &C, u_t N, u_t bmax)
{
    for (u_t i = 0; i < N; ++i)
    {
        for (u_t j = 0; j < N; ++j)
        {
            A.put(i, j, rand() % 2);
        }
    }
    for (u_t x = 0; x < N; ++x)
    {
        u_t rx = 0, cx = 0;
        for (u_t k = 0; k < N; ++k)
        {
            rx ^= A.get(x, k);
            cx ^= A.get(k, x);
        }
        R.push_back(rx);
        C.push_back(cx);
    }
    for (u_t i = 0; i < N; ++i)
    {
        for (u_t j = 0; j < N; ++j)
        {
            if (rand() % 2)
            {
                A.put(i, j, -1);
                u_t b = rand() % bmax + 1;
                B.put(i, j, b);
            }
            else
            {
                B.put(i, j, 0);
            }
        }
    }
}

template<class T>
static void wmat(ostream& f, const Matrix<T>& mat)
{
    for (u_t i = 0; i < mat.m; ++i)
    {
        const char* sep = "";
        for (u_t j = 0; j < mat.n; ++j)
        {
            f << sep << int(mat.get(i, j));
            sep = " ";
        }
        f << '\n';
    }
}

static void wvec(ostream& f, const vu8_t& a)
{
    const char *sep = "";
    for (u_t x: a)
    {
        f << sep << x; sep = " ";
    }
    f << '\n';
}

static int test(int argc, char ** argv)
{
    int rc = 0;
    int ai = 0;
    u_t n_tests = strtoul(argv[ai++], 0, 0);
    u_t n_min = strtoul(argv[ai++], 0, 0);
    u_t n_max = strtoul(argv[ai++], 0, 0);
    u_t b_max = strtoul(argv[ai++], 0, 0);
    u_t n_diff = n_max - n_min;
    for (u_t ti = 0; (rc == 0) && (ti < n_tests); ++ti)
    {
        cerr << "Tested: " << ti << '/' << n_tests << '\n';
        u_t N = n_min + (n_diff > 0 ? rand() % n_diff : 0);
        i8mat_t A(N, N);
        umat_t B(N, N);
        vu8_t R, C;
        rand_data(A, B, R, C, N, b_max);
        bool small = N <= 5;
        const char *fn = small ? "checksum-auto.in" : "checksum-large-auto.in";
        ofstream f(fn);
        f << "1\n" << N << '\n';
        wmat(f, A);
        wmat(f, B);
        wvec(f, R);
        wvec(f, C);
        f.close();
        ull_t solution = ull_t(-1), solution_naive = ull_t(-1);
        {
             ifstream ifs(fn);
             int dum_T;  ifs >> dum_T;
             Checksum p(ifs);
             p.solve();
             solution = p.get_solution();
        }
        if (small)
        {
             ifstream ifs(fn);
             int dum_T;  ifs >> dum_T;
             Checksum p(ifs);
             p.solve_naive();
             solution_naive = p.get_solution();
             if (solution_naive != solution)
             {
                 cerr << "Inconsistent: solution="<<solution <<
                     ", naive="<<solution_naive << '\n';
                 rc = 1;
             }
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

    void (Checksum::*psolve)() =
        (naive ? &Checksum::solve_naive : &Checksum::solve);
    // if (solve_ver == 1) { psolve = &Checksum::solve1; } // solve1
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
