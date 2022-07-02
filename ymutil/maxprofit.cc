#include <vector>
#include <numeric>

using namespace std;

typedef unsigned u_t;
typedef vector<int> vi_t;
typedef vector<u_t> vu_t;

int max_profit(const vi_t& prices, u_t k)
{
    const u_t n = prices.size();
    vi_t last_profits(n, 0);
    for (u_t trans = 0; trans < k; ++trans)
    {
        vi_t curr_profits; curr_profits.reserve(n);
        int max_diff = 0 - prices[0];
        curr_profits.push_back(0);
        for (u_t i = 1; i < n; ++i)
        {
            int profit = max(curr_profits.back(), prices[i] + max_diff);
            curr_profits.push_back(profit);
            max_diff = max(max_diff, last_profits[i] - prices[i]);
        }
        swap(last_profits, curr_profits);
    }
    int mp = last_profits.back();
    return mp;
}

void combination_first(vu_t &c, u_t n, u_t k)
{
    c = vu_t(k);
    iota(c.begin(), c.end(), 0);
}

bool combination_next(vu_t &c, u_t n)
{
    u_t j = 0, k = c.size();

    // sentinels (Knuth) (Why the second (0) ??!)
    c.push_back(n);  c.push_back(0);

    while ((j < k) && (c[j] + 1 == c[j + 1]))
    {
        c[j] = j;
        ++j;
    }
    bool ret = j < k;
    if (ret)
    {
        ++(c[j]);
    }

    c.pop_back(); c.pop_back(); // the sentinels

    return ret;
}

int max_profit_naive(const vi_t& prices, u_t k)
{
    int mp = 0;
    const u_t n = prices.size();
    for (u_t sk = 2; sk <= min(2*k, n); sk += 2)
    {
        vu_t c; // alwyas increasing
        combination_first(c, n, sk);
        for (bool more = true; more; more = combination_next(c, n))
        {
            int profit = 0;
            for (u_t bsi = 0; bsi < sk; bsi += 2)
            {
                const u_t ibuy = c[bsi], isell = c[bsi + 1];
                profit += prices[isell] - prices[ibuy];
            } 
            mp = max(mp, profit);
        }
    }
    return mp;
}

#include <iostream>
#include <string>
#include <cstdlib>

static int test_case(const vi_t& prices, u_t k)
{
    int rc = 0;
    const int mp_real = max_profit(prices, k);
    const int mp_naive = max_profit_naive(prices, k);
    if (mp_real != mp_naive)
    {
        rc = 1;
        cerr << "Inconsisternt: max_profit = " << mp_real << " != " << 
            mp_naive << " = naive\n  specific " << k << ' ';
        for (int p: prices)
        {
            cerr << ' ' << p;
        }
        cerr << '\n';
    }
    return rc;
}

static int test_specific(int argc, char **argv)
{
    int k = strtoul(argv[0], nullptr, 0);
    vi_t prices; prices.reserve(argc - 1);
    for (int ai = 1; ai < argc; ++ai)
    {
        prices.push_back(strtoul(argv[ai], nullptr, 0));
    }
    return test_case(prices, k);
}

static u_t rand_int(u_t low, u_t high)
{
    return low + (rand() % (high - low));
}

static int test_random(int argc, char **argv)
{
    int ai = -1;
    const u_t n_tests = strtoul(argv[++ai], nullptr, 0);
    const u_t n_min = strtoul(argv[++ai], nullptr, 0);
    const u_t n_max = strtoul(argv[++ai], nullptr, 0);
    const u_t k_min = strtoul(argv[++ai], nullptr, 0);
    const u_t k_max = strtoul(argv[++ai], nullptr, 0);
    const u_t p_max = strtoul(argv[++ai], nullptr, 0);
    cerr << 
        "n_tests=" << n_tests <<
        ", n_min" << n_min <<
        ", n_max" << n_max <<
        ", k_min" << k_min <<
        ", k_max" << k_max <<
        ", p_max" << p_max <<
        '\n';
    int rc = 0;
    for (u_t ti = 0; (ti < n_tests) && (rc == 0); ++ti)
    {
        cerr << "Tested: " << ti << '/' << n_tests << '\n';
        const int n = rand_int(n_min, n_max);
        const int k = rand_int(min<int>(k_min, n), min<int>(k_max, n));
        vi_t prices; prices.reserve(n);
        while (prices.size() < size_t(n))
        {
            prices.push_back(rand_int(0, p_max));
        }
        rc = test_case(prices, k);
    }
    return rc;
}

int main(int argc, char **argv)
{
    int rc = 0;
    if (string(argv[1]) == string("specific"))
    {
        rc = test_specific(argc - 2, argv + 2);
    }
    else
    {   
        rc = test_random(argc - 1, argv + 1);
    }
    return rc;
}
