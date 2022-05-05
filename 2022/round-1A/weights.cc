// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <numeric>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
typedef vector<vu_t> vvu_t;
typedef vector<vvu_t> vvvu_t;
typedef unordered_multiset<u_t> msu_t;
typedef vector<msu_t> vmsu_t;
typedef vector<vmsu_t> vvmsu_t;

static unsigned dbg_flags;

class Weights
{
 public:
    Weights(istream& fi);
    Weights(const vvu_t& _X) :
        E(_X.size()), W(_X[0].size()), X(_X), solution(0)
        {}
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    u_t get_solution() const { return solution; }
 private:
    void naive_next(vu_t& combs);
    void naive_next_moves(vu_t& combs, u_t moves);
    u_t naive_count(const vu_t& combs);
    void gen_combs(const vu_t& exer, vvu_t& combs) const;
    // non-naive
    u_t get_common(vu_t& over, const vu_t& base, u_t ebi, u_t eei) const;
    bool disjoint(vu_t& wc, vu_t& w1) const;
    u_t get_cost(const vu_t& base, u_t ebi, u_t eei);
    void init_common_base();
    void init_cost();
    u_t get_cost(u_t b, u_t e);

    u_t E, W;
    vvu_t X;
    u_t solution;
    // for naive
    u_t bcount;
    u_t ecount;
    vvvu_t exs_combs;
    vu_t icombs_best;
    // for non-naivr
    // vvmsu_t be_common_base; // indxed by [b][e - b - 1]
    vvvu_t be_common_base; // indxed by [b][e - b - 1]
    vvu_t be_common_base_size; // indxed by [b][e - b - 1]
    vvu_t  be_cost; // indxed by [b][e - b - 1]
    enum {COST_INFINITY = 0x200000};
};

Weights::Weights(istream& fi) : solution(0)
{
    fi >> E >> W;
    X.reserve(E);
    for (u_t ei = 0; ei < E; ++ei)
    {
        vu_t ws;
        copy_n(istream_iterator<u_t>(fi), W, back_inserter(ws));
        X.push_back(ws);
    }
}

void Weights::solve_naive()
{
    solution = u_t(-1);
    bcount = accumulate(X.front().begin(), X.front().end(), 0u);
    ecount = accumulate(X.back().begin(), X.back().end(), 0u);
    for (const vu_t& x: X)
    {
        vvu_t xcomb;
        gen_combs(x, xcomb);
        exs_combs.push_back(xcomb);
    }
    vu_t combs;
    // naive_next(combs);
    naive_next_moves(combs, 0);
    if (dbg_flags & 0x1) {
        cerr << "Best:";
        for (u_t ei = 0; ei < E; ++ei) {
            cerr << " E["<<ei<<"]:";
            const vvu_t& xcomb = exs_combs[ei];
            const vu_t& comb = xcomb[icombs_best[ei]];
            for (u_t w: comb) { cerr << ' ' << w; }
        }
        cerr << '\n';
    }
}

#if 1
void Weights::naive_next(vu_t& icombs)
{
    const u_t sz = icombs.size();
    if (sz == E)
    {
        u_t c = naive_count(icombs);
        if (solution > c)
        {
            icombs_best = icombs;
            solution = c;
        }
    }
    else
    {
        icombs.push_back(0);
        for (u_t i = 0, csz = exs_combs[sz].size(); i < csz; ++i)
        {
            icombs.back() = i;
            naive_next(icombs);
        }
        icombs.pop_back();
    }
}
#endif

void Weights::naive_next_moves(vu_t& icombs, u_t moves)
{
    const u_t sz = icombs.size();
    if (sz == E)
    {
        moves += ecount;
        if (solution > moves)
        {
            icombs_best = icombs;
            solution = moves;
        }
    }
    else
    {
        const u_t i_pre = (icombs.empty() ? 0 : icombs.back());
        icombs.push_back(0);
        for (u_t i = 0, csz = exs_combs[sz].size(); i < csz; ++i)
        {
            u_t sub_moves = bcount;
            icombs.back() = i;
            if (sz > 0)
            {
                const vu_t& a = exs_combs[sz - 1][i_pre];
                const vu_t& b = exs_combs[sz][i];
                u_t sz_min = min(a.size(), b.size());
                u_t i_eq;
                for (i_eq = 0; (i_eq < sz_min) && (a[i_eq] == b[i_eq]); ++i_eq)
                {
                    ;
                }
                u_t rem_count = a.size() - i_eq;
                u_t add_count = b.size() - i_eq;
                sub_moves = moves + rem_count + add_count;
            }
            naive_next_moves(icombs, sub_moves);
        }
        icombs.pop_back();
    }
}

void tuple_next(vu_t& t, const vu_t& bound)
{
    u_t i, sz = bound.size();
    bool done = false;
    for (i = 0; (i < sz) && !done; ++i)
    {
        ++t[i];
        done = (t[i] < bound[i]);
        if (!done)
        {
            fill_n(t.begin(), i + 1, 0);
        }
    }
    if (!done)
    {
        t.clear();
    }
}

void Weights::gen_combs(const vu_t& exer, vvu_t& combs) const
{
    combs.clear();
    const u_t n = accumulate(exer.begin(), exer.end(), 0u);
    vu_t bound(n, W);
    vu_t t(n, 0);
    for (; !t.empty(); tuple_next(t, bound))
    {
        vu_t counts(W, 0);
        for (u_t w: t)
        {
            ++counts[w];
        }
        if (counts == exer)
        {
            combs.push_back(t);
        }
    }
}

u_t Weights::naive_count(const vu_t& combs)
{
    u_t count = bcount;
    for (u_t ei = 0; ei + 1 < E; ++ei)
    {
        const u_t ia = combs[ei];
        const u_t ib = combs[ei + 1];
        const vu_t& a = exs_combs[ei][ia];
        const vu_t& b = exs_combs[ei + 1][ib];
        u_t sz_min = min(a.size(), b.size());
        u_t i_eq;
        for (i_eq = 0; (i_eq < sz_min) && (a[i_eq] == b[i_eq]); ++i_eq)
        {
            ;
        }
        u_t rem_count = a.size() - i_eq;
        u_t add_count = b.size() - i_eq;
        count += rem_count + add_count;
    }
    count += ecount;
    return count;
}

void Weights::solve()
{
#if 1
    init_common_base();
    init_cost();
    u_t move_cost = get_cost(0, E);
    u_t common_base_size = be_common_base_size[0].back();
    solution = move_cost + 2*common_base_size;
#else
    vu_t empty_base(W, 0);
    vu_t base;
    u_t base_size = get_common(base, empty_base, 0, E);
    u_t over_cost = get_cost(base, 0, E);
    solution = 2*base_size + over_cost;
#endif
}

u_t Weights::get_common(vu_t& over, const vu_t& base, u_t ebi, u_t eei) const
{
    over.clear();
    for (u_t ei = ebi; ei < eei; ++ei)
    {
        for (u_t wi = 0; wi < W; ++wi)
        {
            u_t x = X[ei][wi] - base[wi];
            if (ei == ebi)
            {
                over.push_back(x);
            }
            else
            {
                over[wi] = min(over[wi], x);
            }
        }
    }
    return accumulate(over.begin(), over.end(), 0u);
}

void vsum(vu_t& r, const vu_t& x0, const vu_t& x1)
{
    r.clear(); r.reserve(x0.size());
    for (u_t i = 0; i < x0.size(); ++i)
    {
        r.push_back(x0[i] + x1[i]);
    }
}

u_t Weights::get_cost(const vu_t& base, u_t ebi, u_t eei)
{
    u_t cost = 0;
    if (ebi < eei)
    {
        if (ebi + 1 == eei)
        {
            vu_t over;
            u_t over_size = get_common(over, base, ebi, eei);
            cost = 2*over_size;
        }
        else
        {
            cost = u_t(-1); // infinity
            const u_t mid = (ebi + eei)/2;
            u_t best_cut = 2*eei;
            u_t best_distance_mid = E;
            for (u_t cut = ebi + 1; cut < eei; ++cut)
            {
                vu_t over_left, over_right;
                (void)get_common(over_left, base, ebi, cut);
                (void)get_common(over_right, base, cut, eei);
                if (disjoint(over_left, over_right))
                {
                    u_t distance = cut < mid ? mid - cut : cut - mid;
                    if (best_distance_mid > distance)
                    {
                        best_distance_mid = distance;
                        best_cut = cut;
                    }
                }
            }
            {
                vu_t over_left, over_right;
                u_t size_left = get_common(over_left, base, ebi, best_cut);
                u_t size_right = get_common(over_right, base, best_cut, eei);
                // if (disjoint(over_left, over_right))
                {
                    vu_t base_left, base_right;
                    vsum(base_left, base, over_left);
                    vsum(base_right, base, over_right);
                    u_t cost_left = get_cost(base_left, ebi, best_cut);
                    u_t cost_right = get_cost(base_right, best_cut, eei);
                    u_t cut_cost = 2*(size_left + size_right) +
                        cost_left + cost_right;
                    if (dbg_flags & 0x2) {
                        cerr << "ebi="<<ebi << ", eei="<<eei <<
                        ", best_cut="<<best_cut << 
                        ", cost_left="<<cost_left<<", cost_right="<<cost_right<<
                        '\n'; }
                        
                    if (cost > cut_cost)
                    {
                        cost = cut_cost;
                    }
                }
            }
        }
    }
    return cost;
}

bool Weights::disjoint(vu_t& wc0, vu_t& wc1) const
{
    bool disj = true;
    for (u_t wi = 0; disj && (wi < W); ++wi)
    {
        disj = (wc0[wi] == 0) ||  (wc1[wi] == 0);
    }
    return disj;
}

void Weights::init_common_base()
{
    be_common_base.reserve(E);
    be_common_base_size.reserve(E);
    for (u_t b = 0; b < E; ++b)
    {
        vvu_t b_bases; b_bases.reserve(E - b);
        vu_t b_sizes;   b_sizes.reserve(E - b);
        b_bases.push_back(X[b]);
        b_sizes.push_back(accumulate(X[b].begin(), X[b].end(), 0));
        for (u_t i = b + 1; i < E; ++i)
        {
            vu_t last = b_bases.back();
            vu_t next; next.reserve(W);
            u_t be_size = 0;
            for (u_t w = 0; w < W; ++w)
            {
                u_t nw = min(last[w], X[i][w]);
                next.push_back(nw);
                be_size += nw;
            }
            b_bases.push_back(next);
            b_sizes.push_back(be_size);
        }
        be_common_base.push_back(b_bases);
        be_common_base_size.push_back(b_sizes);
    }
}

void Weights::init_cost()
{
    be_cost.reserve(E);
    for (u_t b = 0; b < E; ++b)
    {
        be_cost.push_back(vu_t(E - b, COST_INFINITY));
    }    
}

u_t Weights::get_cost(u_t b, u_t e)
{
    u_t cost = 0;
    if (b + 1 < e)
    {
        const u_t ebm1 = e - b - 1;
        cost = be_cost[b][ebm1];
        if (cost == COST_INFINITY)
        {
            u_t min_cut_cost = COST_INFINITY;
            for (u_t cut = b + 1; cut < e; ++cut)
            {
                const u_t cost_left = get_cost(b, cut);
                const u_t cost_right = get_cost(cut, e);
                const u_t size_left = be_common_base_size[b][cut - b - 1];
                const u_t size_right = be_common_base_size[cut][e - cut - 1];
                const u_t size_glue = 2*(size_left + size_right);
                const u_t cut_cost = cost_left + cost_right + size_glue;
                    if (dbg_flags & 0x2) {
                        cerr << "bi="<<b << ", e="<<e << ", cut="<<cut <<
                        ", cost_L="<<cost_left<<", cost_R="<<cost_right<<
                        ", size_L="<<size_left<<", size_R="<<size_right<<
                        ", size_glue="<<size_glue << ", cut_cost="<<cut_cost <<
                        "\n"; }
                if (min_cut_cost > cut_cost)
                {
                    min_cut_cost = cut_cost;
                }
            }
            const u_t be_size = be_common_base_size[b][ebm1];
            cost = min_cut_cost - 4*be_size; // 4=2*2 factor-out from Analysis ??
            if (dbg_flags & 0x2) {
                cerr << "bi="<<b << ", e="<<e << ", cost="<<cost << '\n'; }
            be_cost[b][e - b - 1] = cost;
        }
    }
    return cost;
}

void Weights::print_solution(ostream &fo) const
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

    void (Weights::*psolve)() =
        (naive ? &Weights::solve_naive : &Weights::solve);
    if (solve_ver == 1) { psolve = &Weights::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Weights weights(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (weights.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        weights.print_solution(fout);
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

static void save_case(const char* fn, const vvu_t& X)
{
    ofstream f(fn);
    const u_t E = X.size();
    const u_t W = X[0].size();
    f << "1\n" << E << ' ' << W << '\n';
    for (const vu_t& exercise: X)
    {
        const char* sep = "";
        for (u_t x: exercise)
        {
            f << sep << x; sep = " ";
        }
        f << '\n';
    }
    f.close();
}

static u_t test_case(const vvu_t& X)
{
    int rc = 0;
    const u_t E = X.size();
    const u_t W = X[0].size();
    const bool small = (E <= 10) && (W <= 3);
    u_t solution(-1), solution_naive(-1);
    if (dbg_flags & 0x100) { save_case("weights-curr.in", X); }
    if (small)
    {
        Weights p{X};
        p.solve_naive();
        solution_naive = p.get_solution();
    }
    {
        Weights p{X};
        p.solve();
        solution = p.get_solution();
    }
    if (small && (solution != solution_naive))
    {
        rc = 1;
        cerr << "Failed: solution="<<solution << " != " <<
            solution_naive << " = solution_naive\n";
        save_case("weights-fail.in", X);
    }
    cerr << "  E="<<E << ", W="<<W << ", solution="<<solution << '\n';
    return rc;
}

static int test_random(int argc, char ** argv)
{
    int rc = 0;
    int ai = 0;
    if (string(argv[ai]) == string("-debug"))
    {
        ++ai;
        dbg_flags = strtoul(argv[ai++], 0, 0);
    }
    u_t n_tests = strtoul(argv[ai++], 0, 0);
    u_t Emin = strtoul(argv[ai++], 0, 0);
    u_t Emax = strtoul(argv[ai++], 0, 0);
    u_t Wmin = max<u_t>(1, strtoul(argv[ai++], 0, 0));
    u_t Wmax = strtoul(argv[ai++], 0, 0);
    u_t Xmax = strtoul(argv[ai++], 0, 0);
    cerr << "n_tests="<<n_tests <<
        ", Emin="<<Emin << ", Emax="<<Emax <<
        ", Wmin="<<Wmin << ", Wmax="<<Wmax <<
        ", Xmax="<<Xmax <<
        '\n';
    for (u_t ti = 0; (rc == 0) && (ti < n_tests); ++ti)
    {
        cerr << "Tested: " << ti << '/' << n_tests << '\n';
        u_t E = rand_range(Emin, Emax);
        u_t W = rand_range(Wmin, Wmax);
        vvu_t X; X.reserve(E);
        while (X.size() < E)
        {
            vu_t exercise; exercise.reserve(E);
            while (accumulate(exercise.begin(), exercise.end(), 0) == 0)
            {
                exercise.clear();
                while (exercise.size() < W)
                {
                    exercise.push_back(rand_range(0, Xmax));
                }
            }
            X.push_back(exercise);
        }
        rc = test_case(X);
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
