// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <numeric>
#include <set>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include <cstdlib>

#include "bigfrac.cc" // TEMPORARY !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef long long ll_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
typedef vector<vu_t> vvu_t;
typedef vector<ull_t> vull_t;
typedef array<u_t, 2> au2_t;
typedef vector<au2_t> vau2_t;
// typedef unordered_map<u_t, ull_t> um_u2ull_t;
typedef set<u_t> setu_t;
typedef vector<setu_t> vsetu_t;

static unsigned dbg_flags;

static const ull_t MOD_BIG = 1000000000 + 7; // 1000000007

// See: https://en.wikipedia.org/wiki/Extended_Euclidean_algorithm
ll_t extended_gcd(ll_t& bezout_x, ll_t& bezout_y, const ll_t a, const ll_t b)
{
    
    ll_t s = 0, old_s = 1;
    ll_t r = b, old_r = a;
         
    while (r != 0)
    {
        lldiv_t div_result = div(ll_t(old_r), ll_t(r));
        ll_t quotient = div_result.quot;
        ll_t smod = old_s - quotient * s;
        old_r = r;  r = div_result.rem;
        old_s = s;  s = smod;
    }

    bezout_x = old_s;
    bezout_y = (b ? (old_r - old_s * a) / b : 0);
    return ((a == 0) && (b == 0) ? 0 : old_r);
}

ull_t invmod(ull_t a, ull_t m)
{
    ll_t x, y;
    extended_gcd(x, y, a, m);
    ull_t inv = (x + m) % m;
    return inv;
}

ull_t n_choose_2(ull_t n)
{
    int c = 0;
    if (n >= 2)
    {
        c = (n*(n - 1))/2;
    }
    return c;
}
    
class KCount
{
 public:
    KCount(u_t _M): M(_M) { run(); }
    u_t M;
    vull_t counters;
 private:
    void run();
    ull_t components_count() const;
    vvu_t edge_priorities;
    vvu_t active;
};

void KCount::run()
{
    counters = vull_t(M/2 + 1, 0);
    u_t E = (M*(M - 1))/2;
    vau2_t ijs; ijs.reserve(E);
    for (u_t i = 0; i < M; ++i)
    {
        for (u_t j = i + 1; j < M; ++j)
        {
            ijs.push_back(au2_t{i, j});
        }
    }
    edge_priorities = vvu_t(M, vu_t(M, 0));
    active = vvu_t(M, vu_t());
    vu_t perm(E, 0);
    iota(perm.begin(), perm.end(), 0);
    ull_t Efact = 1;
    for (ull_t n = 1; n <= E; ++n) { Efact *= n; }
    if (dbg_flags & 0x1) { cerr << "M="<<M<<", E="<<E<<", Efact="<<Efact<<'\n'; }
    ull_t n = 0;
    for (bool more = true; more; 
        more = next_permutation(perm.begin(), perm.end()), ++n)
    {
        if ((dbg_flags & 0x1) && ((n & (n - 1)) == 0)) {
             cerr << "Ran " << n << " / " << Efact << " permutations\n"; }
        for (u_t p = 0; p < E; ++p) // both perm index & priority
        {
            const au2_t& ij = ijs[perm[p]];
            u_t i = ij[0], j = ij[1];
            edge_priorities[i][j] = p;
            edge_priorities[j][i] = p;
        }
        vu_t vset_adjs = vu_t(M, 0);
        for (u_t i = 0; i < M; ++i)
        {
            u_t jlink = i; // undef
            u_t pbest = 0;
            for (u_t j = 0; j < M; ++j)
            {
                if (i != j)
                {
                    if (pbest <= edge_priorities[i][j])
                    {
                         pbest = edge_priorities[i][j];
                         jlink = j;
                    }
                }
            }
            vset_adjs[i] |= (1u << jlink);
            vset_adjs[jlink] |= (1u << i);
        }
        for (u_t i = 0; i < M; ++i)
        {
            vu_t a;
            for (u_t j = 0; j < M; ++j)
            {
                if ((vset_adjs[i] & (1u << j)) != 0)
                {
                    a.push_back(j);
                }
            }
            swap(active[i], a);
        }
        ull_t cc = components_count();
        ++counters[cc];
    }
}

ull_t KCount::components_count() const
{
    ull_t n = 0;
    vector<bool> visited(M, false);
    vu_t node2c(M, 0);
    for (u_t v = 0; v < M; ++v)
    {
        if (!visited[v])
        {
            node2c[v] = n;
            setu_t q;
            q.insert(v);
            while (!q.empty())
            {
                u_t node = *q.begin();
                q.erase(q.begin());
                for (u_t a: active[node])
                {
                    if (!visited[a])
                    {
                        visited[a] = true;
                        node2c[a] = n;
                        q.insert(a);
                    }
                }
            }
            if (!active[v].empty())
            {
                ++n;
            }
        }
    }
    return n;
}

u_t sqroot(u_t n)
{
    u_t r = 0;
    if (n > 0)
    {
        r = 1;
        for (bool iter = true; iter; )
        {
            u_t d = n / r;
            u_t rmin = min(r, d);
            u_t rmax = max(r, d);
            if (rmax - rmin > 1)
            {
                u_t rnext = (r + d)/2;
                r = rnext;
            }
            else
            {
                iter = false;
                r =  (rmax * rmax <= n) ? rmax : rmin;
            }
        }
    }
    return r;
}

u_t ceil_sqroot(u_t n)
{
    u_t r = sqroot(n);
    if (r*r < n)
    {
        ++r;
    }
    return r;
}

u_t edges_get_min_vertices(u_t e)
{
    // V >= max({v: v(v-1)/2 < e}) + 1
    // solving   v^2 -v -2e < 0
    u_t x = (1 + sqroot(1 + 8*e))/2;
    if (x*(x - 1) < 2*e)
    {
        ++x;
    }
    return x;
}

class UBF
{
 public:
    UBF(ull_t _u=0, const Frac& _f=Frac::zero) : u(_u), f(_f) {}
    void nd_set(ull_t n, ull_t d)
    {
        u = (n * invmod(d, MOD_BIG)) % MOD_BIG;
        f = Frac(n, d);
    }
    ull_t u;
    Frac f; // for debug
};

UBF operator+(const UBF& uf0, const UBF& uf1)
{
    UBF ret{(uf0.u + uf1.u) % MOD_BIG};
    Frac::add(ret.f, uf0.f, uf1.f);
    return ret;
}

UBF operator*(const UBF& uf0, const UBF& uf1)
{
    UBF ret{(uf0.u * uf1.u) % MOD_BIG};
    Frac::mult(ret.f, uf0.f, uf1.f);
    return ret;
}

void swap(UBF& uf0, UBF& uf1)
{
    swap(uf0.u, uf1.u);
    swap(uf0.f, uf1.f);
}

typedef unordered_map<ul_t, UBF> um_u2ubf_t;


class CompCountSolver
{
 public:
    CompCountSolver(u_t m, u_t k) :
        M(m), K(k), E((M*(M - 1))/2), invE(invmod(E, MOD_BIG))
    {}
    ull_t result();
    const u_t M, K, E, invE;
 private:
    void get_probability(UBF& prob, u_t i, u_t j, u_t k);
    void compute_probability(UBF& prob, u_t i, u_t j, u_t k);
    um_u2ubf_t ijk_memo;
};

ull_t CompCountSolver::result()
{
   UBF r;
   get_probability(r, E, M, K);
   if (dbg_flags & 0x1) { cerr << __func__ << ", M="<<M << ", K="<<K <<
       " P=" << r.f.str() << '\n'; }
   return r.u;
}

void CompCountSolver::get_probability(UBF& prob, u_t i, u_t j, u_t k)
{
    const ul_t key = (i << 16) | (j << 8) | k;
    um_u2ubf_t::iterator iter = ijk_memo.find(key);
    if (iter == ijk_memo.end())
    {
        prob.u = 0;
        prob.f = Frac::zero;
        if (!((i <= (j*(j - 1))/2) && (j <= 2*i)))
        {
            ; // impossible
        }
        else if (k == 0)
        {
            ; // zero
        }
        else if (i <= 1)
        {
            prob.u = ((j == 2*i) && (k == i) ? 1 : 0);
            prob.f = Frac(prob.u, 1);
        }
        else if (i > 1)
        {
            compute_probability(prob, i, j, k);
        }
        if (dbg_flags & 0x2) { cerr << __func__ <<
            "(i="<<i << ", j="<<j << ", k="<<k << ") = "<<prob.f.str() << '\n'; }
        iter = ijk_memo.insert(iter, um_u2ubf_t::value_type(key, prob));
    }
    prob = iter->second;
}


void CompCountSolver::compute_probability(UBF& prob, u_t i, u_t j, u_t k)
{
    // u_t n_edges = 0;
    UBF total; // zero initialized
    const u_t isub = i - 1;
    // const u_t jmin = (2 + ceil_sqroot(1 + 8*isub))/2;
    // const u_t jmax = min(M, 2*isub);
    // for (u_t jsub = jmin; jsub <= jmax; ++jsub)
    {
        typedef array<u_t, 4> au4_t;
        u_t ne1 = j > 2 ? n_choose_2(M - (j - 2)) : 0;
        u_t ne2 = j > 1 ? (j - 1)*(M - (j - 1)) : 0;
        u_t ne3 = n_choose_2(j) - isub;
        if (dbg_flags & 0x2) { cerr << "ijk=("<<i<<", "<<j<<", "<<k<<")" 
            ", ne1="<<ne1<<", ne2="<<ne2<<", ne3="<<ne3<<'\n'; }
        // n_edges += ne1 + ne2 + ne3;
        u_t ichoice = 0;
        for (const au4_t& nijk: {
            au4_t{ne1, isub, j - 2, k - 1},
            au4_t{ne2, isub, j - 1, k},
            au4_t{ne3, isub, j, k}})
        {
            ++ichoice;
            const u_t nsub = nijk[0], jsub = nijk[2], ksub = nijk[3];
            if (nsub > 0)
            {
                UBF prob_sub;
                get_probability(prob_sub, isub, jsub, ksub);
                if (dbg_flags & 0x4) { cerr << 
                    "ijk=("<<i<<", "<<j<<", "<<k<<") " << "ichoice="<<ichoice <<
                    ", nsub="<<nsub << ", prob_sub="<< prob_sub.f.str() <<'\n'; }
                total = total + UBF(nsub, Frac(nsub)) * prob_sub;
            }
            
        }
    }
    {
        UBF div_n_edges;
        div_n_edges.nd_set(ull_t(1), ull_t(E - isub));
        prob = total * div_n_edges;
    }
    if (dbg_flags & 0x2) { cerr << "ijk=("<<i<<", "<<j<<", "<<k<<")"
        " prob=" << prob.f.str() << '\n'; }
}

class Intranets
{
 public:
    Intranets(istream& fi);
    Intranets(const vu_t&) {}; // TBD for test_case
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    ull_t get_solution() const { return 0; }
 private:
    u_t M, K;
    ull_t solution;
};

Intranets::Intranets(istream& fi) : solution(0)
{
    fi >> M >> K;
}

void Intranets::solve_naive()
{
    UBF u_solution;
    CompCountSolver ccs(M, K);
    solution = ccs.result();
}

void Intranets::solve()
{
    solve_naive();
}

void Intranets::print_solution(ostream &fo) const
{
    fo << ' ' << solution;
}

static int real_main(int argc, char ** argv)
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

    void (Intranets::*psolve)() =
        (naive ? &Intranets::solve_naive : &Intranets::solve);
    if (solve_ver == 1) { psolve = &Intranets::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Intranets intranets(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (intranets.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        intranets.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}

static u_t rand_range(u_t nmin, u_t nmax)
{
    u_t r = nmin + rand() % (nmax + 1 - nmin);
    return r;
}

static int test_case(int argc, char ** argv)
{
    int rc = rand_range(0, 1);
    ull_t solution(-1), solution_naive(-1);
    bool small = rc == 0;
    if (small)
    {
        Intranets p{vu_t()};
        p.solve_naive();
        solution_naive = p.get_solution();
    }
    {
        Intranets p{vu_t()};
        p.solve();
        solution = p.get_solution();
    }
    if (small && (solution != solution_naive))
    {
        rc = 1;
        cerr << "Failed: solution="<<solution << " != " <<
            solution_naive << " = solution_naive\n";
        ofstream f("intranets-fail.in");
        f << "1\n";
        f.close();
    }
    return rc;
}

static int test_random(int argc, char ** argv)
{
    int rc = test_case(argc, argv);
    // dbg_flags |= 0x1;
    for (u_t M : {2, 3, 4, 5})
    {
        KCount kc(M);
        cout << "M = " << M << '\n';
        for (u_t i = 0; i < kc.counters.size(); ++i)
        {
            cout << "  #(" << i << " components) = " << kc.counters[i] << '\n';
        }
    }
    return rc;
}

int main(int argc, char **argv)
{
    int rc = ((argc > 1) && (string(argv[1]) == string("test"))
        ? test_random(argc - 2, argv + 2)
        : real_main(argc, argv));
    return rc;
}
