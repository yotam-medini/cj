#include <functional>
#include <vector>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<int> vi_t;
typedef vector<u_t> vu_t;
typedef vector<vu_t> vvu_t;

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

template <bool _Maximize>
class Hungarian
{
 public:
    Hungarian(const vvu_t& weight_matrix) :
        w(weight_matrix),
        NL(w.size()),
        NR(w[0].size()),
        lp(max(NL, NR), -1),
        rp(max(NL, NR), -1)
        {}
    void matching_get(vu_t& matching);
 private:
    const vvu_t& w;
    const u_t NL, NR;
    vi_t lp, rp;
    vu_t lh, rh;
};

template<bool _Maximize>
void Hungarian<_Maximize>::matching_get(vu_t& matching)
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
void best_matching_naive(
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
        next_permutation(p.begin(), p.end()))
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
}

void minimal_matching_naive(vvu_t& matchings, const vvu_t& weight_matrix)
{
    best_matching_naive<less<u_t>, less_equal<u_t>>(
        matchings, weight_matrix);
}

void maximal_matching_naive(vvu_t& matchings, const vvu_t& weight_matrix)
{
    best_matching_naive<greater<u_t>, greater_equal<u_t>>(
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
        "   " << p0 << " naive <filein>\n"
        "   " << p0 << " test <filein>\n";
}

static int test_naive(const char* fn)
{
    vvu_t weight_matrix;
    read_weight(weight_matrix, fn);
    vvu_t matchings;
    maximal_matching_naive(matchings, weight_matrix);
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
        if (subcmd == string("naive"))
        {
            rc = test_naive(argv[2]);
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
