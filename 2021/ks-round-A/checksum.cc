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
typedef unordered_map<uint8_t, uint8_t> u2u_t;

typedef unordered_set<u_t> usetu_t;
typedef vector<usetu_t> vusetu_t;
typedef vector<vau2_t> vvau2_t;

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
    u_t count_unknowns(setu_t& rows, setu_t& cols) const;
    u_t reduce_by(setu_t& rows, setu_t& cols); // return # reduced
    void improve1(const vau3_t& bijs_used);
    void improve();

#if 0
    void solve_init();
    void get_reduction(vau2_t& reduction, u_t i, u_t j);
#endif
    void find_disjoint_sets();

    u_t N;
    i8mat_t *pA;
    umat_t *pB;
    i8mat_t *psA;
    vu8_t R, C;
    ull_t solution;

#if 0
    vusetu_t rows_unknwon_cols, cols_unknown_rows;
    setu_t active_rows, active_cols;
    vau3_t bijs; 
    vvau2_t reductions;
#endif
    typedef vector<DisjointSets::elementp_t> velemp_t;
    typedef unordered_set<DisjointSets::szlist_t*, u_t> szlist_to_u_t;

#if 0
    velemp_t r_elems(size_t(N), nullptr);
    velemp_t c_elems(size_t(N), nullptr);
    DisjointSets djs;
    ull_t b_total;
    szlist_to_u_t szlist_to_u;
    vvau2_t component_rc; 
#endif   
    
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

#if 0
void Checksum::solve1()
{
     typedef array<u_t, 3> u3_t;
     typedef vector<u3_t> vu3_t;
     psA = new i8mat_t(N, N);
     bijs.reserve(N*N);
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
     setu_t rows, cols; // unfilled
     u_t n_unknowns = count_unknowns(rows, cols);
     solution = 0;
     vu3_t bijs_used;
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
             u_t b = bij[0], i = bij[1], j = bij[2];
             if (dbg_flags & 0x2) { cerr << "@("<<i << ", "<<j << "): " <<
                 b <<'\n'; }
             rows.clear(); cols.clear();
             rows.insert(i);
             cols.insert(j);
             psA->put(i, j, 0); // arbitrary !?
             solution += b;
             --n_unknowns;
             bijs_used.push_back(bij);
         }
     }
     improve1(bijs_used);
}

void Checksum::improve1(const vau3_t& bijs_used)
{
    unordered_set<u_t> retracts;
    setu_t rows0, cols0;
    const u_t n_unknowns = count_unknowns(rows0, cols0);
    const u_t n_orig_queried = bijs_used.size();
    i8mat_t A_filled = *psA;
    for (u_t di = bijs_used.size(); di > 0; )
    {
        --di; // check if we can retract di;
        *psA = *pA;
        for (u_t k = bijs_used.size(); k > 0; )
        {
            --k;
            const au3_t& bij = bijs_used[k];
            if ((k != di) && (retracts.find(k) == retracts.end()))
            {
                const u_t i = bij[1], j = bij[2];
                psA->put(i, j, A_filled.get(i, j));
            }
        }
        setu_t rows(rows0), cols(cols0);
        const u_t n_reduced = reduce_by(rows, cols);
        const u_t n_retracts = retracts.size();
        const u_t n_curr_queried = n_orig_queried - n_retracts - 1;
        if (n_reduced + n_curr_queried == n_unknowns)
        {
            solution -= bijs_used[di][0];
            retracts.insert(di);
        }
    }
}
#endif

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

#if 0
void Checksum::find_disjoint_sets()
{
    setu_t rs, cs;
    for (u_t r = 0; r < N; ++r)
    {
        for (u_t c = 0; c < N; ++c)
        {
            u_t b = pB->get(r, c);
            if (b > 0)
            {
                b_total += b;
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
                djs.union_sets(r_elems[r], c_elems[c]);
            }
        }
    }
}

void Checksum::find_disjoint_sets()
{
    vector<DisjointSets::szlist_t*> u_to_szlist;
    for (const DisjointSets::szlist_t* szlist: djs.get_list_lists())
    {
         szlist_to_u.insert(szlist, szlist_to_u.size());
         u_to_szlist.push_back(szlist);
    }
    component_rc = vvau2_t(size_t(u_to_szlist.size(), vau2_t()));
    for (u_t r = 0; r < N; ++r)
    {
        u_t icomp(-1);
        if (r_elems[r])
        {
            icomp = szlist_to_u[r_elems[r]->mylist;
        }
        szlist_to_u_t::i
        for (u_t c = 0; c < N; ++c)
        {
            u_t b = pB->get(r, c);
            if (b > 0)1
            {
                component_rc[icomp].push_back(au3_t{r, c})    
            }
        }
    }
}
#endif

#if 0
// static bool bij_gt(const au3_t& b0, const au3_t& b1)
void Checksum::solve()
{
    // Let's ignre, A, B, C. only care about B.
    // assume known A, and B+C are all zero.
    solution = 0;
    solve_init();
    u_t n_unknowns = bijs.size();

    vau2_t trivial_reduction;
    get_reduction(trivial_reduction, N, N);
    n_unknowns -= trivial_reduction.size();

    u_t bi = 0;
    for (bi = 0; n_unknowns > 0; ++bi)
    {
        const au3_t& bij = bijs[bi];
        const u_t b = bij[0], i = bij[1], j = bij[2];
        vau2_t reduction;
        if (rows_unknwon_cols[i].find(j) != rows_unknwon_cols[i].end())
        {
            if (dbg_flags & 0x1) { 
                cerr << "b="<<b << " @ (" << i << ", "<<j << ")\n"; }
            solution += b;
            get_reduction(reduction, i, j);
            n_unknowns -= reduction.size();
        }
        reductions.push_back(reduction);
    }
    improve();
}

void Checksum::solve_init()
{
    rows_unknwon_cols = vusetu_t(N, usetu_t());
    cols_unknown_rows = vusetu_t(N, usetu_t());
    bijs.reserve(N*N);
    for (u_t i = 0; i < N; ++i)
    {
        active_rows.insert(i);
        for (u_t j = 0; j < N; ++j)
        {
            u_t b = pB->get(i, j);
            if (b > 0)
            {
                rows_unknwon_cols[i].insert(j);
                cols_unknown_rows[j].insert(i);
                bijs.push_back(au3_t{b, i, j});
            }
        }
    }
    active_cols = active_rows; // all
    sort(bijs.begin(), bijs.end());
}

void Checksum::get_reduction(vau2_t& reduction, u_t i, u_t j)
{
    if ((i < N) && (j < N))
    {
        rows_unknwon_cols[i].erase(j);
        cols_unknown_rows[j].erase(i);
        active_rows.insert(i);
        active_cols.insert(j);
        reduction.push_back(au2_t{i, j});
    }
    while (!(active_rows.empty() && active_cols.empty()))
    {
        for (u_t row: active_rows)
        {
            unordered_set<u_t>& row_unknwon_cols = rows_unknwon_cols[row];
            if (row_unknwon_cols.size() == 1)
            {
                u_t col = *row_unknwon_cols.begin();
                row_unknwon_cols.clear();
                cols_unknown_rows[col].erase(row);
                reduction.push_back(au2_t{row, col});
                active_cols.insert(col);
            }
        }
        active_rows.clear();

        for (u_t col: active_cols)
        {
            unordered_set<u_t>& col_unknown_rows = cols_unknown_rows[col];
            if (col_unknown_rows.size() == 1)
            {
                u_t row = *col_unknown_rows.begin();
                col_unknown_rows.clear();
                rows_unknwon_cols[row].erase(col);
                reduction.push_back(au2_t{row, col});
                active_rows.insert(row);
            }
        }
        active_cols.clear();
    }
}

void Checksum::improve()
{
    unordered_set<u_t> retracts;
    setu_t rows0, cols0;
    u_t n_reductions = reductions.size();
    for (u_t di = (n_reductions > 1 ? n_reductions - 1 : 0); di > 0; )
    {
        --di; // check if we can retract di;
        const vau2_t& redi = reductions[di];
        if (!redi.empty())
        {
            rows_unknwon_cols = vusetu_t(N, usetu_t());
            cols_unknown_rows = vusetu_t(N, usetu_t());
            active_rows.clear(); active_cols.clear();
            u_t n_retract_unkowns = 0;
            for (u_t k = 0; k < n_reductions; ++k)
            {
                const vau2_t& redk = reductions[k];
                if (!redk.empty())
                {
                    u_t t0 = 1, nr = redk.size();
                    // We keep buying redk[0] unless retracted
                    if ((k == di) || (retracts.find(k) != retracts.end()))
                    {
                        t0 = 0;
                        if (dbg_flags & 0x2) { 
                            const au2_t& ij = redk[0]; u_t i = ij[0], j = ij[1];
                            cerr<<"di="<<di <<" retract? ("<<i<<","<<j<<")\n"; }
                    }
                    for (u_t t = t0; t < nr; ++t)
                    {
                        const au2_t& ij = redk[t];
                        u_t i = ij[0], j = ij[1];
                        rows_unknwon_cols[i].insert(j);
                        cols_unknown_rows[j].insert(i);
                        active_rows.insert(i);
                        active_cols.insert(j);
                    }
                    n_retract_unkowns += (nr - t0);
                }
            }
            vau2_t tail_reduction;
            const au3_t& bij = bijs[di];
            const u_t b = bij[0], i = bij[1], j = bij[2];
            get_reduction(tail_reduction, N, N);
            const u_t tail_reduced= tail_reduction.size();
            if (dbg_flags & 0x2) { cerr << "di="<<di << ", reduced=" << 
               tail_reduced << ", unkowns="<<n_retract_unkowns << '\n'; }
            if (tail_reduced == n_retract_unkowns) // can retract!
            {
                if (dbg_flags & 0x1) { cerr << "retract: di="<<di << 
                    ", b="<<b << ", @ ("<<i << ", "<<j << ")\n"; }
                solution -= b;
                retracts.insert(di);
            }
        }
    }
}
#endif

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
                ++n_reduced;
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
                ++n_reduced;
            }
        }
        cols.clear();
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
