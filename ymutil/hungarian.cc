#include <functional>
#include <vector>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<int> vi_t;
typedef vector<u_t> vu_t;
typedef vector<vu_t> vvu_t;

void minimal_matching(vu_t& matching, const vvu_t& weight_matrix)
{
}

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

class Hungarian
{
 public:
    Hungarian(const vvu_t& weight_matrix) :
        w(weight_matrix),
        lp(w.size(), -1),
        rp(w.size(), -1)
        {}
 private:
    const vvu_t& w;
    vi_t lp, rp;
    vu_t lh, rh;
};

typedef bool (*_cmp_t)(u_t, u_t);

template<typename LG_Compare, typename LGEQ_Compare>
void best_matching_naive(
    vvu_t& matchings,
    const vvu_t& weight_matrix
    // _cmp_t lgeq,
    // _cmp_t lg)
    // const pointer_to_binary_function<u_t, u_t, bool> lgeq,
    // const pointer_to_binary_function<u_t, u_t, bool> lg)
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
    const u_t n = weight_matrix.size();
    ofstream f(fn);
    f << n << '\n';
    for (u_t r = 0; r < n; ++r)
    {
        const vu_t& row = weight_matrix[r];
        for (u_t c = 0; c < n; ++c)
        {
            f << (c == 0 ? "" : " ") << row[c];
        }
        f << '\n';
    }
}
