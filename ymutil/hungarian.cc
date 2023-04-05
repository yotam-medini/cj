#include <iostream>
#include <array>
#include <deque>
#include <functional>
#include <limits>
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
typedef deque<u_t> dqu_t;

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

class HungarianBase
{
 public:
    HungarianBase(const vvu_t& weight_matrix, int wdef=0) :
        w_default(wdef),
        w(weight_matrix),
        NL(w.size()),
        NR(w[0].size()),
        N(max(NL, NR)),
        lp(N, -1),
        rp(N, -1),
        matched(0)
        {}
    void matching_get(vu_t& matching);
 protected:
    void solve();
    bool is_feasible(u_t l, u_t r) const;
    void find_aug_path();
    int get_w(u_t l, u_t r) const
    {
        return ((l < NL) && (r < NR) ? w[l][r] : w_default);
    }
    virtual void init_feasible() = 0;
    virtual u_t get_delta() const = 0;
    virtual void relabel(u_t delta) = 0;
    int w_default; // for _Maximize
    const vvu_t& w;
    const u_t NL, NR, N;
    vi_t lp, rp;
    vi_t lh, rh;
    // equality sub graph
    vhsetu_t eq_l2r, eqr2l;
    vi_t match_l2r, match_r2l;
    u_t matched;
    vu_t P; // augmented path
    hsetu_t FL, FR, FRc;
};

void HungarianBase::matching_get(vu_t& matching)
{
    solve();
}

void HungarianBase::solve()
{
    init_feasible();
    FL.clear();
    FR.clear();
    for (u_t r = 0; r < N; ++r) { FRc.insert(r); } // R - FR;
    while (matched < N)
    {
        find_aug_path();
        if (P.empty()) { cerr << "Error empty augmented path\n"; exit(1); }
    }
}

void HungarianBase::find_aug_path()
{
    dqu_t Q;
    for (u_t l = 0; l < N; ++l)
    {
        if (match_l2r[l] == -1)
        {
            lp[l] = -1;
            Q.push_back(l);
            FL.insert(l);
        }
    }
    bool found = false; // path found
    while (!found)
    {
        u_t delta = get_delta();
        relabel(delta);
    }
    P.clear();
}

template <bool _Maximize>
class Hungarian : public HungarianBase
{
 public:
    Hungarian(const vvu_t& weight_matrix) :
        HungarianBase(
            weight_matrix,
            _Maximize ? 0 : numeric_limits<int>::max())
    {}
    // void matching_get(vu_t& matching);
 private:
    void init_feasible();
    u_t get_delta() const;
    void relabel(u_t delta);
};

template <bool _Maximize>
void Hungarian<_Maximize>::init_feasible()
{
    for (u_t i = 0; i < N; ++i)
    {
        int h = w[i][0];
        for (u_t j = 1; j < N; ++j)
        {
            if (_Maximize)
            {
                 if (h < int(w[i][j]))
                 {
                     h = w[i][j];
                 }
            }
            else
            {
                 if (h > int(w[i][j]))
                 {
                     h = w[i][j];
                 }
            }
        }
        lh[i] = h;
        rh[i] = 0;
    }
}

template <bool _Maximize>
u_t Hungarian<_Maximize>::get_delta() const
{
    u_t l0 = *FL.begin();
    u_t r0 = *FRc.begin();
    u_t delta = (_Maximize
        ? (lh[l0] + rh[r0]) - w[l0][r0]
        : w[l0][r0] - (lh[l0] + rh[r0]));
    for (u_t l: FL)
    {
        for (u_t r: FRc)
        {
            u_t v = (_Maximize
                ? (lh[l] + rh[r]) - w[l][r]
                : w[l][r] - (lh[l] + rh[r]));
            if (_Maximize)
            {
                if (delta > v) { delta = v; }
            }
            else
            {
                if (delta < v) { delta = v; }
            }
        }
    }
    return delta;
}

template <bool _Maximize>
void Hungarian<_Maximize>::relabel(u_t delta)
{
}

void minimal_matching(vu_t& matching, const vvu_t& weight_matrix)
{
    Hungarian<false>(weight_matrix).matching_get(matching);
}

void maximal_matching(vu_t& matching, const vvu_t& weight_matrix)
{
    Hungarian<true>(weight_matrix).matching_get(matching);
}

////////////////////////////////////////////////////////////////////////
#if defined(TEST_HUNGARIAN)
typedef bool (*_cmp_t)(u_t, u_t);

template<typename LG_Compare, typename LGEQ_Compare>
u_t best_matching_naive(
    vvu_t& matchings,
    const vvu_t& weight_matrix
)
{
    matchings.clear();
    const u_t n = weight_matrix.size();
    vu_t p(n);
    iota(p.begin(), p.end(), 0);
    u_t best_weight = permutation_weight(p, weight_matrix);
    matchings.push_back(p);
    for (bool more = next_permutation(p.begin(), p.end()); more;
        more = next_permutation(p.begin(), p.end()))
    {
        u_t weight = permutation_weight(p, weight_matrix);
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

u_t minimal_matching_naive(vvu_t& matchings, const vvu_t& weight_matrix)
{
    return best_matching_naive<less<u_t>, less_equal<u_t>>(
        matchings, weight_matrix);
}

u_t maximal_matching_naive(vvu_t& matchings, const vvu_t& weight_matrix)
{
    return best_matching_naive<greater<u_t>, greater_equal<u_t>>(
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

static void usage(const char* p0)
{
    cerr << "Usage: " << p0 << " <subcmd> ...\n" << 
        "  Where <subcmd> can be\n:"
        "   " << p0 << " naive-max <filein>\n"
        "   " << p0 << " test <filein>\n";
}

static int test_naive_max(const char* fn)
{
    vvu_t weight_matrix;
    read_weight(weight_matrix, fn);
    vvu_t matchings;
    u_t best = maximal_matching_naive(matchings, weight_matrix);
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
        const string subcmd(argv[1]);
        if (subcmd == string("naive-max"))
        {
            rc = test_naive_max(argv[2]);
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
