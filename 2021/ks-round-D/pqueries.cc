// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <utility>
#include <vector>
// #include <map>
// #include <set>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef long long ll_t;
typedef vector<ull_t> vull_t;
typedef vector<ll_t> vll_t;

static unsigned dbg_flags;

class SegmentTree
{
 public:
    typedef ll_t e_t;
    typedef vector<e_t> ve_t;
    SegmentTree(const ve_t& _a=ve_t());
    void set(const ve_t& _a);
    void update(u_t pos, e_t val);
    e_t query_sum(u_t b, u_t e) const;
 private:
    typedef vector<ve_t> vve_t;
    e_t level_sum(u_t level, u_t b, u_t e) const;
    void level_update(u_t level, u_t pos, e_t old_val, e_t val);
    u_t height() const { return a.size(); }
    u_t size() const { return a.empty() ? 0 : a[0].size(); }
    vve_t a;
};

SegmentTree::SegmentTree(const ve_t& _a)
{
    set(_a);
}

void SegmentTree::set(const ve_t& _a)
{
    a.clear();
    a.push_back(_a);
    while (a.back().size() > 1)
    {
        const ve_t& prev = a.back();
        const u_t sz = prev.size() / 2;
        const u_t sz2 = 2*sz;
        ve_t b; b.reserve(sz);
        for (u_t i = 0; i < sz2; i += 2)
        {
            b.push_back(prev[i] + prev[i + 1]);
        }
        a.push_back(ve_t());
        swap(a.back(), b);
    }
}

void SegmentTree::update(u_t pos, e_t val)
{
    level_update(0, pos, a[0][pos], val);
}

SegmentTree::e_t SegmentTree::query_sum(u_t b, u_t e) const
{
    return level_sum(0, b, e);
}

SegmentTree::e_t SegmentTree::level_sum(u_t level, u_t b, u_t e) const
{
    e_t total = 0;
    if (b < e)
    {
        if (b % 2 != 0)
        {
            total += a[level][b++];
        }
        if (e % 2 != 0)
        {
            total += a[level][--e];
        }
        total += level_sum(level + 1, b/2, e/2);
    }
    return total;
}

void SegmentTree::level_update(u_t level, u_t pos, e_t old_val, e_t val)
{
    ve_t& alevel = a[level];
    alevel[pos] -= old_val;
    alevel[pos] += val;
    if ((pos % 2 != 0) || (pos + 1 < alevel.size()))
    {
        level_update(level + 1, pos / 2, old_val, val);
    }
}

class Query1
{
 public:
    Query1(u_t p, ull_t v) : pos(p), val(v) {}
    u_t pos;
    ull_t val;
};

class Query2
{
 public:
    Query2(ull_t _s, u_t _l, u_t _r) : S(_s), L(_l), R(_r) {}
    ull_t S;
    u_t L, R;
};

union UQ
{
 public:
     UQ() {}
     ~UQ() {}
     Query1 query1;
     Query2 query2;
};

class Query
{
 public:
    Query() {}
    u_t qt;
    UQ u;
};
typedef vector<Query> vquery_t;

class PQueries
{
 public:
    PQueries(istream& fi);
    PQueries(const vull_t& _A, const vquery_t& _queries, ull_t _P) :
        N(_A.size()), Q(_queries.size()), P(_P), A(_A), queries(_queries) {}
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    const vull_t& get_solution() const { return solution; }
 private:
    ull_t sum_formula_naive(const Query2& query2) const;
    void init_seg_trees();
    void update_seg_trees(u_t pos, ull_t val);
    ull_t compute_query(const Query2& query2) const;
    ull_t vp(ull_t n) const;
    u_t N, Q;
    ull_t P;
    vull_t A;
    vquery_t queries;
    vull_t solution;
    u_t q2_count;
    // Non-naive
    SegmentTree sgt_divp;
    SegmentTree sgt_non_divp;
    SegmentTree sgt_non_divp_other;
    SegmentTree sgt_non_divp_count;
};

PQueries::PQueries(istream& fi) : q2_count(0)
{
    fi >> N >> Q >> P;
    A.reserve(N);
    copy_n(istream_iterator<ull_t>(fi), N, back_inserter(A));
    queries.reserve(Q);
    while (queries.size() < Q)
    {
        Query query;
        fi >> query.qt;
        switch (query.qt)
        {
         case 1:
            {
                u_t pos; ull_t val;
                fi >> pos >> val;
                query.u.query1 = Query1(pos, val);
            }
            break;
         case 2:
            {
                ull_t S; u_t L, R;
                fi >> S >> L >> R;
                query.u.query2 = Query2(S, L, R);
                ++q2_count;
            }
            break;
         default:
            cerr << "bad query type: " << query.qt << '\n';
            exit(1);
        }
        queries.push_back(query);
    }
}

void PQueries::solve_naive()
{
    solution.reserve(q2_count);
    for (const Query& query: queries)
    {
        switch (query.qt)
        {
         case 1:
            A[query.u.query1.pos - 1] = query.u.query1.val;
            break;
         case 2:
            solution.push_back(sum_formula_naive(query.u.query2));
            break;
        }
    }
}

ull_t PQueries::sum_formula_naive(const Query2& query2) const
{
    ull_t total = 0;
    for (u_t i = query2.L - 1; i <= query2.R - 1; ++i)
    {
        const u_t a = A[i];
        const u_t amod = a % P;
        ull_t a_ps = 1, amod_ps = 1;
        for (u_t s = 0; s < query2.S; ++s)
        {
            a_ps *= a;
            amod_ps *= amod;
        }
        ull_t n = a_ps - amod_ps;
        u_t padic = 0;
        if (n > 0)
        {
            while (n % P == 0)
            {
                ++padic;
                n /= P;
            }
        }
        total += padic;
    }
    return total;
}

void PQueries::solve()
{
    init_seg_trees();
    solution.reserve(q2_count);
    for (const Query& query: queries)
    {
        switch (query.qt)
        {
         case 1:
            update_seg_trees(query.u.query1.pos - 1, query.u.query1.val);
            break;
         case 2:
            solution.push_back(compute_query(query.u.query2));
            break;
        }
    }
}

void PQueries::init_seg_trees()
{
    vll_t 
        v_divp(size_t(N), 0), 
        v_nondivp(size_t(N), 0), 
        v_nondivp_other(size_t(N), 0), 
        count_nondivp(size_t(N), 0);
    for (u_t i = 0; i < N; ++i)
    {
        const ull_t n = A[i];
        if (n < P)
        {
            ;
        }
        else
        if (n % P == 0)
        {
            v_divp[i] = vp(n);
        }
        else
        {
            v_nondivp[i] = vp(n - (n % P));
            v_nondivp_other[i] = vp(n + (n % P)) - 1;
            count_nondivp[i] = 1;
        }
    }
    sgt_divp.set(v_divp); v_divp.clear();
    sgt_non_divp.set(v_nondivp); v_nondivp.clear();
    sgt_non_divp_other.set(v_nondivp_other); v_nondivp_other.clear();
    sgt_non_divp_count.set(count_nondivp); count_nondivp.clear();
}

void PQueries::update_seg_trees(u_t pos, ull_t n)
{
    if ((n % P == 0) || (n < P))
    {
        sgt_divp.update(pos, vp(n));
        sgt_non_divp.update(pos, 0);
        sgt_non_divp_other.update(pos, 0);
        sgt_non_divp_count.update(pos, 0);
    }
    else
    {
        sgt_divp.update(pos, 0);
        sgt_non_divp.update(pos, vp(n - (n % P)));
        sgt_non_divp_other.update(pos, ll_t(vp(n + (n % P))) - 1);
        sgt_non_divp_count.update(pos, 1);
    }
}

ull_t PQueries::compute_query(const Query2& query2) const
{
    const u_t L = query2.L - 1, R = query2.R;
    ull_t total_divp = query2.S * sgt_divp.query_sum(L, R);
    ull_t total_non_divp = sgt_non_divp.query_sum(L, R);
    ull_t total_non_divp_other = 0;
    if ((P == 2) && (query2.S % 2 == 0))
    {
        total_non_divp_other = sgt_non_divp_other.query_sum(L, R);
    }
    ull_t vps = vp(query2.S);
    ull_t total_non_divp_count = vps * sgt_non_divp_count.query_sum(L, R);
    ull_t total = total_divp +
        total_non_divp + total_non_divp_other + total_non_divp_count;
    return total;
}

ull_t PQueries::vp(ull_t n) const
{
    ull_t v = 0;
    if (n > 0)
    {
        vull_t p_2powers;
        p_2powers.push_back(P); // p&1, p^2, p^4, ... p&{2^n}
        ull_t pp;
        while (n % (pp = p_2powers.back()) == 0)
        {
            p_2powers.push_back(pp * pp);
        }
        for (u_t i = p_2powers.size() - 1; n % P == 0; --i)
        {
            pp = p_2powers[i];
            if (n % pp == 0)
            {
                n /= pp;
                v += (1ull << i);
            }
        }
    }
    return v;
}

void PQueries::print_solution(ostream &fo) const
{
    for (ull_t x: solution)
    {
        fo << ' ' << x;
    }
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

    void (PQueries::*psolve)() =
        (naive ? &PQueries::solve_naive : &PQueries::solve);
    if (solve_ver == 1) { psolve = &PQueries::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        PQueries pqueries(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (pqueries.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        pqueries.print_solution(fout);
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

static 
void save_case(const char* fn, const vull_t& A, const vquery_t& queries, ull_t P)
{
    u_t N = A.size();
    u_t Q = queries.size();
    ofstream f(fn);
    f << "1\n" << N << ' ' << Q << ' ' << P << '\n';
    const char *sep = "";
    for (ull_t x: A)
    {
        f << sep << x; sep = " ";
    }
    f << '\n';
    for (const Query& query: queries)
    {
        f << query.qt << ' ';
        switch (query.qt)
        {
         case 1:
            f << query.u.query1.pos << ' ' << query.u.query1.val << '\n';
            break;
         case 2:
            {
                const Query2& q2 = query.u.query2;
                f << q2.S << ' ' << q2.L << ' ' << q2.R << '\n';
            }
            break;
        }
    }
}

static
int test_case(const vull_t& A, const vquery_t& queries, ul_t P, ull_t Smax)
{
    int rc = 0;
    u_t N = A.size();
    u_t Q = queries.size();
    const bool small (Smax < 6);
    vull_t solution, solution_naive;
    if (small)
    {
        PQueries pqueries(A, queries, P);
        pqueries.solve_naive();
        solution_naive = pqueries.get_solution();
    }
    {
        PQueries pqueries(A, queries, P);
        pqueries.solve();
        solution = pqueries.get_solution();
    }
    if (small && (solution != solution_naive))
    {
        rc = 1;
        cerr << "Inconsistent\n";
        save_case("pqueries-fail.in", A, queries, P);
    }
    u_t ssz = solution.size();
    cerr << "  N="<<N << ", Q="<<Q << ", solution[" << ssz << "]:";
    for (u_t i = 0; (i < ssz) && (i < 4); ++i) { cerr << ' ' << solution[i]; }
    cerr << " ?...? \n";
    return rc;
}

static void comma_sep_get_numbers(vull_t& nums, const char* comma_sep)
{
    while (*comma_sep)
    {
        char* comma;
        ull_t p = strtoul(comma_sep, &comma, 0);
        nums.push_back(p);
        if (*comma)
        {
            comma_sep = comma + 1;
        }
        comma_sep = comma + (*comma ? 1 : 0);
    }
}

static int test_random(int argc, char **argv)
{
    int rc = 0;
    int ai = 0;
    u_t n_tests = strtoul(argv[ai++], 0, 0);
    u_t Nmin = strtoul(argv[ai++], 0, 0);
    u_t Nmax = strtoul(argv[ai++], 0, 0);
    u_t Qmin = strtoul(argv[ai++], 0, 0);
    u_t Qmax = strtoul(argv[ai++], 0, 0);
    vull_t primes;
    comma_sep_get_numbers(primes, argv[ai++]);
    ull_t Amax = strtoul(argv[ai++], 0, 0);
    ull_t Smax = strtoul(argv[ai++], 0, 0);
    cerr << ", n_tests="<<n_tests << 
        ". Nmin="<<Nmin << ", Nmax="<<Nmax <<
        ". Qmin="<<Qmin << ", Qmax="<<Qmax <<
        ". Amax="<<Amax << ", Smax="<<Smax <<
        ", primes:";
    for (ull_t prime: primes) { cerr << ' ' << prime; }
    cerr << '\n';
    for (u_t ti = 0; (rc == 0) && (ti < n_tests); ++ti)
    {
        cerr << "Tested: " << ti << '/' << n_tests << '\n';
        u_t N = rand_range(Nmin, Nmax);
        u_t Q = rand_range(Qmin, Qmax);
        ull_t P = primes[rand_range(0, primes.size() - 1)];
        vull_t A; A.reserve(N);
        while (A.size() < N)
        {
            A.push_back(rand_range(1, Amax));
        }
        vquery_t queries; queries.reserve(Q);
        ull_t Smax_actual = 0;
        while (queries.size() < Q)
        {
            Query query;
            query.qt = (queries.size() + 1 < Q ? rand_range(1, 2) : 2);
            switch (query.qt)
            {
             case 1:
                query.u.query1 = Query1(rand_range(1, N), rand_range(1, Amax));
                break;
             case 2:
                {
                    ull_t S = rand_range(1, Smax); 
                    u_t L = rand_range(1, N);
                    u_t R = rand_range(L, N);
                    query.u.query2 = Query2(S, L, R);
                    Smax_actual = max(Smax_actual, S);
                }
                break;
            }
            queries.push_back(query);
        }
        rc = test_case(A, queries, P, Smax_actual);
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
