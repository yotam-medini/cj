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
            ++n;
        }
    }
    return n;
}

class UBF
{
 public:
    UBF(ull_t _u=0, const Frac& _bf=Frac::zero) : u(_u), bf(_bf) {}
    ull_t u;
    Frac bf;
};
typedef unordered_map<u_t, UBF> um_u2ubf_t;

static ull_t probability_mk_components(u_t m, u_t k, Frac& f_prob)
{
    static um_u2ubf_t memo_prob_jk; // indexed by j-verices k intranets
    const u_t key ((m << 8) | k);
    um_u2ubf_t::iterator iter = memo_prob_jk.find(key);
    if (iter == memo_prob_jk.end())
    {
        ull_t prob = 0;
        f_prob = Frac::zero;
        if (m <= 3)
        {
            if (k == 1)
            {
                prob = 1;
                f_prob = Frac::one;
            }
        }
        else
        {
            Frac fm1k, fm2k1;
            const ull_t prob_mm1_k = probability_mk_components(m - 1, k, fm1k);
            const ull_t prob_mm2_km1 =
                probability_mk_components(m - 2, k - 1, fm2k1);
            // const ull_t m_choose_2 = (m*(m - 1)) / 2;
            const ull_t mm2_choose_2 = ((m - 2)*(m - 3)) / 2;
            const ull_t prob_add_denom = mm2_choose_2 + 2*(m - 2);
            const ull_t inv_prob_add_denom = invmod(prob_add_denom, MOD_BIG);
            const ull_t prob_add = (mm2_choose_2 * inv_prob_add_denom) % MOD_BIG;
            const ull_t prob_add_comp = (MOD_BIG + 1 - prob_add) % MOD_BIG;
            prob = prob_add_comp * prob_mm1_k + prob_add * prob_mm2_km1;
            prob %= MOD_BIG;
            if (dbg_flags & 0x1) { 
                cerr << "m="<<m << ", k="<<k << ": " <<
                    "mm2_choose_2 = "<< mm2_choose_2 << 
                    ", mm2_choose_2 = "<< mm2_choose_2 << '\n';
                Frac f_prob_add, f_prob_add_comp;
                Frac::div(f_prob_add, Frac(mm2_choose_2), Frac(prob_add_denom));
                Frac::sub(f_prob_add_comp, Frac::one, f_prob_add);
                { 
                    Frac mult1, mult2;
                    Frac::add(f_prob, 
                        Frac::mult(mult1, f_prob_add_comp, fm1k),
                        Frac::mult(mult2, f_prob_add, fm2k1));
                }
                cerr << "   Pequ="<<f_prob_add_comp.str() << 
                    ", Padd="<<f_prob_add.str() << ", P="<<f_prob.str() << "\n";
            }
        }
        UBF ubf_prob{prob, f_prob};
        iter = memo_prob_jk.insert(iter, um_u2ubf_t::value_type{key, ubf_prob});
    }
    f_prob = iter->second.bf;
    return iter->second.u;
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
    Frac dum;
    solution = probability_mk_components(M, K, dum);
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
