// Following the the pseudo code of [Algorithms/CLRS]
// Introduction to Algorithms, fourth edition
// by Thomas H. Cormen, Charles E. Leiserson, Ronald L. Rivest
// and Clifford Stein.
// ISBN: 9780262046305

#include <iostream>
#include <array>
#include <deque>
#include <functional>
#include <unordered_set>
#include <vector>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef array<u_t, 2> au2_t;
typedef vector<int> vi_t;
typedef vector<u_t> vu_t;
typedef vector<vu_t> vvu_t;
typedef vector<au2_t> vau2_t;
typedef unordered_set<u_t> hsetu_t;
typedef vector<hsetu_t> vhsetu_t;

class Hash_AU2 {
 public:
     size_t operator()(const au2_t& a) const {
       u_t n = a[0] ^ (a[1] << 7);
       return hash_uint(n);
     }
 private:
     hash<u_t> hash_uint;
};
typedef unordered_set<au2_t, Hash_AU2> hset_au2_t;

u_t permutation_weight(const vu_t& permutation, const vvu_t& weight_matrix)
{
    u_t weight = 0;
    const u_t n = permutation.size();
    for (u_t i = 0; i < n; ++i)
    {
        weight += weight_matrix[i][permutation[i]];
    }
    return weight;
}

#include <algorithm>
#include <numeric>

class LRNode
{
 public:
    LRNode(u_t _i=0, bool _left=false) : i(_i), left(_left) {}
    u_t i;
    bool left;
};

class Hungarian
{
 public:
    Hungarian(const vvu_t& weight_matrix) :
        w(weight_matrix),
        NL(w.size()),
        NR(w.empty() ? 0 : w[0].size()),
        N(max(NL, NR)),
        lp(N, -1),
        rp(N, -1),
        matched(0),
        r_unmatched(N)
        {}
    void matching_get(vu_t& matching);
    static u_t dbg_flags;
 protected:
    typedef deque<LRNode> Q_t;
    void solve();
    bool lr_in_eq_sub_graph(u_t l, u_t r) const;
    bool rl_in_eq_sub_graph(u_t r, u_t l) const;
    bool is_feasible(u_t l, u_t r) const;
    void find_aug_path();
    int get_w(u_t l, u_t r) const
    {
        return ((l < NL) && (r < NR) ? w[l][r] : 0);
    }
    void init_feasible();
    void init_forest();
    void greedy_bipartite_match();
    u_t get_delta() const;
    void relabel(u_t delta);
    bool aug_path_found() const { return r_unmatched != N; }
    void handle_non_FR(Q_t& Q, u_t l, u_t r);
    void build_path();
    void FR_insert(u_t r)
    {
        FR.insert(r);
        FRc.erase(r);
    }
    const vvu_t& w;
    const u_t NL, NR, N;
    vi_t lp, rp; // preceding node in path
    vi_t lh, rh; // label
    // equality sub graph
    vhsetu_t eq_l2r, eqr2l;
    vi_t match_l2r, match_r2l;
    u_t matched;
    vau2_t P; // augmented path
    hsetu_t FL, FR; // BFS forest
    hsetu_t FRc; // complement of FR
    u_t r_unmatched;
};
u_t Hungarian::dbg_flags = 0;

void Hungarian::matching_get(vu_t& matching)
{
    solve();
    const u_t n = min(NL, NR);
    matching.clear();
    matching.reserve(n);
    for (u_t i = 0; i < n; ++i)
    {
        matching.push_back(match_l2r[i]);
    }
}

void Hungarian::solve()
{
    init_feasible();
    greedy_bipartite_match();
    while (matched < N)
    {
        find_aug_path();
        if (P.empty()) { cerr << "Error empty augmented path\n"; exit(1); }
        for (const au2_t lr: P)
        {
            const u_t  &l = lr[0], &r = lr[1];
            match_l2r[l] = r;
            match_r2l[r] = l;
        }
        ++matched;
    }
}

void Hungarian::init_forest()
{
    FL.clear();
    FR.clear();
    for (u_t r = 0; r < N; ++r) { FRc.insert(r); } // R - FR;
}

void Hungarian::init_feasible()
{
    lh.reserve(N);
    rh.assign(N, 0);
    for (u_t i = 0; i < N; ++i)
    {
        int h = w[i][0];
        for (u_t j = 1; j < N; ++j)
        {
            if (h < int(w[i][j]))
            {
                h = w[i][j];
            }
        }
        lh.push_back(h);
    }
}

u_t Hungarian::get_delta() const
{
    u_t l0 = *FL.begin();
    u_t r0 = *FRc.begin();
    u_t delta = (lh[l0] + rh[r0]) - w[l0][r0];
    for (u_t l: FL)
    {
        for (u_t r: FRc)
        {
            u_t v = (lh[l] + rh[r]) - w[l][r];
            if (delta > v) { delta = v; }
        }
    }
    return delta;
}

void Hungarian::relabel(u_t delta)
{
    for (u_t l: FL)
    {
        lh[l] -= delta;
    }
    for (u_t r: FR)
    {
        rh[r] += delta;
    }
}

bool Hungarian::lr_in_eq_sub_graph(u_t l, u_t r) const
{
    const bool ing = (match_l2r[l] != int(r)) && is_feasible(l, r);
    return ing;
}

bool Hungarian::rl_in_eq_sub_graph(u_t r, u_t l) const
{
    const bool ing = (match_r2l[r] == int(l));
    return ing;
}

bool Hungarian::is_feasible(u_t l, u_t r) const
{
    bool isf = (lh[l] + rh[r] == get_w(l, r));
    return isf;
}

void Hungarian::greedy_bipartite_match()
{
    match_l2r.assign(N, -1);
    match_r2l.assign(N, -1);
    for (u_t l = 0; l < N; ++l)
    {
        for (u_t r = 0; (match_l2r[l] == -1) && (r < N); ++r)
        {
            if ((match_r2l[r] == -1) && is_feasible(l, r))
            {
                match_l2r[l] = r;
                match_r2l[r] = l;
                ++matched;
            }
        }
    }
}

void Hungarian::find_aug_path()
{
    deque<LRNode> Q;
    init_forest();
    for (u_t l = 0; l < N; ++l)
    {
        if (match_l2r[l] == -1)
        {
            lp[l] = -1;
            Q.push_back(LRNode(l, true));
            FL.insert(l);
        }
    }
    r_unmatched = N; // augmented path not yet found
    while (!aug_path_found())
    {
        if (Q.empty())
        {
            u_t delta = get_delta();
            relabel(delta); // update lh, rh
            for (hsetu_t::const_iterator riter = FRc.begin();
                (riter != FRc.end()) && !aug_path_found(); )
            {
                const u_t r = *riter;
                ++riter;
                for (hsetu_t::const_iterator liter = FL.begin();
                    (liter != FL.end()) && !aug_path_found(); )
                {
                    const u_t l = *liter;
                    ++liter;
                    if (lr_in_eq_sub_graph(l, r))
                    {
                        handle_non_FR(Q, l, r);
                    }
                }
            }
        }
        if (!aug_path_found())
        {
            const LRNode u = Q.front();
            Q.pop_front();
            if (!u.left)
            {
                const u_t r = u.i;
                for (u_t l = 0; l < N; ++l)
                {
                    if (rl_in_eq_sub_graph(r, l))
                    {
                        lp[l] = r;
                        FL.insert(l);
                        Q.push_back(LRNode(l, true));
                    }
                }
            }
            else // u.left
            {
                const u_t l = u.i;
                for (hsetu_t::const_iterator iter = FRc.begin();
                    (iter != FRc.end()) && !aug_path_found(); )
                {
                    const u_t r = *iter;
                    ++iter;
                    if (lr_in_eq_sub_graph(l, r))
                    {
                        handle_non_FR(Q, l, r);
                    }
                }
            }
        }
    }
    build_path();
}

void Hungarian::handle_non_FR(Q_t& Q, u_t l, u_t r)
{
    rp[r] = l;
    if (match_r2l[r] == -1)
    {
        r_unmatched = r;
    }
    else
    {
        Q.push_back(LRNode(r, false));
        FR_insert(r);
    }
}

void Hungarian::build_path()
{
    P.clear();
    u_t r = r_unmatched;
    u_t l = rp[r]; // match_r2l[r];
    P.push_back(au2_t{l, r});
    while (lp[l] != -1)
    {
        r = lp[l];
        l = rp[r];
        P.push_back(au2_t{l, r});
    }
}

static ull_t sum_matching(const vu_t& matching, const vvu_t& weight_matrix)
{
    const u_t n = min(weight_matrix.size(),
        weight_matrix.empty() ? 0 : weight_matrix[0].size());
    ull_t sm = 0;
    for (u_t i = 0; i < n; ++i)
    {
        sm += weight_matrix[i][matching[i]];
    }
    return sm;
}

u_t minimal_matching(vu_t& matching, const vvu_t& weight_matrix)
{
    u_t wmin = (weight_matrix.empty() || weight_matrix[0].empty()
        ? 0 : weight_matrix[0][0]);
    u_t wmax = wmin;
    for (const vu_t& row: weight_matrix)
    {
        wmin = min(wmin, *min_element(row.begin(), row.end()));
        wmax = max(wmax, *max_element(row.begin(), row.end()));
    }
    const u_t delta = wmax - wmin;
    vvu_t wm_flipped; wm_flipped.reserve(weight_matrix.size());
    for (const vu_t& row: weight_matrix)
    {
        vu_t row_flipped; row_flipped.reserve(row.size());
        for (u_t x: row)
        {
            u_t y = delta - (x - wmin);
            row_flipped.push_back(y);
        }
        wm_flipped.push_back(row_flipped);
    }
    Hungarian(wm_flipped).matching_get(matching);
    ull_t mm = sum_matching(matching, weight_matrix);
    return mm;
}

u_t maximal_matching(vu_t& matching, const vvu_t& weight_matrix)
{
    Hungarian(weight_matrix).matching_get(matching);
    ull_t mm = sum_matching(matching, weight_matrix);
    return mm;
}

////////////////////////////////////////////////////////////////////////
#if defined(TEST_HUNGARIAN)
typedef bool (*_cmp_t)(u_t, u_t);

template<typename LG_Compare, typename LGEQ_Compare>
ull_t best_matching_naive(
    vvu_t& matchings,
    const vvu_t& weight_matrix
)
{
    matchings.clear();
    const u_t n = weight_matrix.size();
    vu_t p(n);
    iota(p.begin(), p.end(), 0);
    ull_t best_weight = permutation_weight(p, weight_matrix);
    matchings.push_back(p);
    for (bool more = next_permutation(p.begin(), p.end()); more;
        more = next_permutation(p.begin(), p.end()))
    {
        ull_t weight = permutation_weight(p, weight_matrix);
        if (LGEQ_Compare()(weight, best_weight))
        {
            if (LG_Compare()(weight, best_weight))
            {
                matchings.clear();
                best_weight = weight;
            }
            matchings.push_back(p);
        }
    }
    return best_weight;
}

ull_t minimal_matching_naive(vvu_t& matchings, const vvu_t& weight_matrix)
{
    return best_matching_naive<less<u_t>, less_equal<ull_t>>(
        matchings, weight_matrix);
}

ull_t maximal_matching_naive(vvu_t& matchings, const vvu_t& weight_matrix)
{
    return best_matching_naive<greater<u_t>, greater_equal<ull_t>>(
        matchings, weight_matrix);
}

#include <iostream>
#include <fstream>
#include <string>

void save_weight(const vvu_t& weight_matrix, const string& fn)
{
    const u_t nl = weight_matrix.size();
    const u_t nr = weight_matrix[0].size();
    ofstream f(fn);
    f << nl << ' ' << nr << '\n';
    for (u_t r = 0; r < nl; ++r)
    {
        const vu_t& row = weight_matrix[r];
        for (u_t c = 0; c < nr; ++c)
        {
            f << (c == 0 ? "" : " ") << row[c];
        }
        f << '\n';
    }
}

void read_weight(vvu_t& weight_matrix, const string& fn)
{
    ifstream f(fn);
    u_t nl, nr;
    f >> nl >> nr;
    weight_matrix.reserve(nl);
    weight_matrix.clear();
    while (weight_matrix.size() < nl)
    {
        weight_matrix.push_back(vu_t());
        vu_t& row = weight_matrix.back();
        row.reserve(nr);
        while (row.size() < nr)
        {
            u_t x;
            f >> x;
            row.push_back(x);
        }

    }
}

static int test_compare_max_case(const vvu_t& weight_matrix)
{
    int rc = 0;
    if (Hungarian::dbg_flags & 0x200) {
        save_weight(weight_matrix, "hungarian-max-curr.in");
    }
    vvu_t matchings;
    ull_t best_naive = maximal_matching_naive(matchings, weight_matrix);
    vu_t matching;
    const ull_t best = maximal_matching(matching, weight_matrix);
    if (best != best_naive)
    {
        rc = 1;
        cerr << __func__ << ": best="<<best << " != " <<
            best_naive << "=best_naive\n";
    }
    else if (find(matchings.begin(), matchings.end(), matching) ==
        matchings.end())
    {
        rc = 1;
        cerr << __func__ << ": matching not found in naive matchings\n";
    }
    if (rc == 0)
    {
        cerr << "   " << __func__ << " N=" << weight_matrix.size() <<
            ", best=" << best << '\n';
    }
    else
    {
        save_weight(weight_matrix, "hungarian-max-fail.in");
    }
    return rc;
}

static int test_compare_min_case(const vvu_t& weight_matrix)
{
    int rc = 0;
    if (Hungarian::dbg_flags & 0x200) {
        save_weight(weight_matrix, "hungarian-min-curr.in");
    }
    vvu_t matchings;
    ull_t best_naive = minimal_matching_naive(matchings, weight_matrix);
    vu_t matching;
    const ull_t best = minimal_matching(matching, weight_matrix);
    if (best != best_naive)
    {
        rc = 1;
        cerr << __func__ << ": best="<<best << " != " <<
            best_naive << "=best_naive\n";
    }
    else if (find(matchings.begin(), matchings.end(), matching) ==
        matchings.end())
    {
        rc = 1;
        cerr << __func__ << ": matching not found in naive matchings\n";
    }
    if (rc == 0)
    {
        cerr << "   " << __func__ << " N=" << weight_matrix.size() <<
            ", best=" << best << '\n';
    }
    else
    {
        save_weight(weight_matrix, "hungarian-min-fail.in");
    }
    return rc;
}

static int test_naive_max(const char* fn)
{
    vvu_t weight_matrix;
    read_weight(weight_matrix, fn);
    vvu_t matchings;
    ull_t best = maximal_matching_naive(matchings, weight_matrix);
    cout << best << '\n';
    for (const vu_t& matching: matchings)
    {
        for (u_t l = 0; l < matching.size(); ++l)
        {
            cout << (l > 0 ? ", " : "") << "[" << l << "]: " << matching[l];
        }
        cout << '\n';
    }
    return 0;
}

static int test_naive_min(const char* fn)
{
    vvu_t weight_matrix;
    read_weight(weight_matrix, fn);
    vvu_t matchings;
    ull_t best = minimal_matching_naive(matchings, weight_matrix);
    cout << best << '\n';
    for (const vu_t& matching: matchings)
    {
        for (u_t l = 0; l < matching.size(); ++l)
        {
            cout << (l > 0 ? ", " : "") << "[" << l << "]: " << matching[l];
        }
        cout << '\n';
    }
    return 0;
}

static int test_max(const char* fn)
{
    vvu_t weight_matrix;
    read_weight(weight_matrix, fn);
    vu_t matching;
    const u_t mm = maximal_matching(matching, weight_matrix);
    cout << mm << '\n';
    for (u_t l = 0; l < matching.size(); ++l)
    {
        cout << (l > 0 ? ", " : "") << "[" << l << "]: " << matching[l];
    }
    cout << '\n';
    return 0;
}

static int test_min(const char* fn)
{
    vvu_t weight_matrix;
    read_weight(weight_matrix, fn);
    vu_t matching;
    const u_t mm = minimal_matching(matching, weight_matrix);
    cout << mm << '\n';
    for (u_t l = 0; l < matching.size(); ++l)
    {
        cout << (l > 0 ? ", " : "") << "[" << l << "]: " << matching[l];
    }
    cout << '\n';
    return 0;
}

static int test_compare_max(const char* fn)
{
    vvu_t weight_matrix;
    read_weight(weight_matrix, fn);
    return test_compare_max_case(weight_matrix);
}

static int test_compare_min(const char* fn)
{
    vvu_t weight_matrix;
    read_weight(weight_matrix, fn);
    return test_compare_min_case(weight_matrix);
}

static u_t rand_range(u_t low, u_t high)
{
    const u_t ret = low + (rand() % ((high + 1) - low));
    return ret;
}

static int test_random(int argc, char **argv)
{
    int rc = 0;
    int ai = 0;
    const u_t n_tests = strtoul(argv[ai++], nullptr, 0);
    const u_t Nmin = strtoul(argv[ai++], nullptr, 0);
    const u_t Nmax = strtoul(argv[ai++], nullptr, 0);
    const u_t Wmax = strtoul(argv[ai++], nullptr, 0);

    for (u_t ti = 0; (rc == 0) && (ti < n_tests); ++ti)
    {
        cerr << "tested: " << ti << '/' << n_tests << '\n';
        u_t N = rand_range(Nmin, Nmax);
        vvu_t wm; wm.reserve(N);
        while (wm.size() < N)
        {
            vu_t row; row.reserve(N);
            while (row.size() < N)
            {
                row.push_back(rand_range(0, Wmax));
            }
            wm.push_back(row);
        }
        rc = test_compare_max_case(wm);
        rc = (rc ? : test_compare_min_case(wm));
    }
    return rc;
}

static void usage(const char* p0)
{
    cerr << "Usage: " << p0 << " [-debug <flags>] <subcmd> ...\n" <<
        "  Where <subcmd> can be:\n"
        "   " << p0 << " naive-max <filein>\n"
        "   " << p0 << " naive-min <filein>\n"
        "   " << p0 << " max <filein>\n"
        "   " << p0 << " min <filein>\n"
        "   " << p0 << " max-compare <filein>\n"
        "   " << p0 << " min-compare <filein>\n"
        "   " << p0 << " random <#-tests> <Nmin> <Nmax> <WeightMax>\n"
        ;
}

int main(int argc, char ** argv)
{
    int rc = 0;
    if (argc == 0)
    {
        usage(argv[0]);
        rc = 1;
    }
    else
    {
        u_t ai = 1;
        if ((argc > 2) && (string(argv[1]) == string("-debug")))
        {
            Hungarian::dbg_flags = strtoul(argv[2], nullptr, 0);
            ai = 3;
        }
        const string subcmd(argc > 1 ? argv[ai++] : "");
        if (subcmd == string("naive-max"))
        {
            rc = test_naive_max(argv[ai]);
        }
        else if (subcmd == string("naive-min"))
        {
            rc = test_naive_min(argv[ai]);
        }
        else if (subcmd == string("max"))
        {
            rc = test_max(argv[ai]);
        }
        else if (subcmd == string("min"))
        {
            rc = test_min(argv[ai]);
        }
        else if (subcmd == string("max-compare"))
        {
            rc = test_compare_max(argv[ai]);
        }
        else if (subcmd == string("min-compare"))
        {
            rc = test_compare_min(argv[ai]);
        }
        else if (subcmd == string("random"))
        {
            rc = test_random(argc - ai, argv + ai);
        }
        else
        {
            cerr << "Unsupported subcmd: " << subcmd << '\n';
            usage(argv[0]);
            rc = 1;
        }
    }
    return rc;
}

#endif /* TEST_HUNGARIAN */
