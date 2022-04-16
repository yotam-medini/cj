#include <vector>
#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;

class SegmentTree
{
 public:
    typedef ull_t e_t;
    typedef vector<e_t> ve_t;
    SegmentTree(const ve_t& _a);
    void update(u_t pos, e_t val);
    e_t queru_sum(u_t b, u_t e) const;
 private:
    typedef vector<ve_t> vve_t;
    u_t height() const { return a.size(); }
    u_t size() const { return a.empty() ? 0 : a[0].size(); }
    vve_t a;
};

SegmentTree::SegmentTree(const ve_t& _a)
{
}

void SegmentTree::update(u_t pos, e_t val)
{
}

SegmentTree::e_t SegmentTree::queru_sum(u_t b, u_t e) const
{
    e_t total = 0;
    return total;
}

// ================================ Test ================================
#include <iostream>
#include <algorithm>
#include <numeric>

typedef vector<ull_t> vull_t;

static int verdict(const vull_t& a, u_t b, u_t e, ull_t st_sum, ull_t naive_sum)
{
    int rc = 0;
    if (st_sum != naive_sum)
    {
        rc = 1;
        cerr << "Failure: test " << b << ' ' << e;
        for (ull_t x: a) { cerr << ' ' << x; }
        cerr << '\n';
    }
    return rc;
}

static int test_case(const vull_t& a, u_t b, u_t e)
{
    ull_t naive_sum = accumulate(a.begin() + b, a.begin() + e, 0ull);
    SegmentTree segtree(a);
    ull_t st_sum = segtree.queru_sum(b, e);
    return verdict(a, b, e, st_sum, naive_sum);
}

static int test_cases(const vull_t& a)
{
    int rc = 0;
    SegmentTree segtree(a);
    for (u_t b = 0; (rc == 0) && (b < a.size()); ++b)
    {
        for (u_t e = b; (rc == 0) && (e <= a.size()); ++e)
        {
            ull_t naive_sum = accumulate(a.begin() + b, a.begin() + e, 0ull);
            ull_t st_sum = segtree.queru_sum(b, e);
            rc = verdict(a, b, e, st_sum, naive_sum);
        }
    }
    return rc;
}

static u_t rand_range(u_t nmin, u_t nmax)
{
    u_t r = nmin + rand() % (nmax + 1 - nmin);
    return r;
}

static int random_test(int argc, char **argv)
{
    int rc = 0;
    int ai = 0;
    u_t n_tests = strtoul(argv[ai++], 0, 0);
    u_t szmin = strtoul(argv[ai++], 0, 0);
    u_t szmax = strtoul(argv[ai++], 0, 0);
    u_t nmax = strtoul(argv[ai++], 0, 0);
    cerr << "n_tests="<<n_tests <<
        ", szmin="<<szmin << ", szmax="<<szmax << ", nmax="<<nmax << '\n';
    for (u_t ti = 0; (rc == 0) && (ti < n_tests); ++ti)
    {
        cerr << "Tested: " << ti << '/' << n_tests << '\n';
        u_t sz = rand_range(szmin, szmax);
        vull_t a;  a.reserve(sz);
        while (a.size() < sz)
        {
            a.push_back(rand_range(0, nmax));
        }
        rc = test_cases(a);
    }
    return rc;
}

int main(int argc, char **argv)
{
    int rc = 0;
    if (string(argv[1]) == string("specific"))
    {
        int ai = 1;
        u_t b = strtoul(argv[++ai], 0, 0);
        u_t e = strtoul(argv[++ai], 0, 0);
        vull_t a;
        for (; ai < argc; ++ai)
        {
            a.push_back(strtoul(argv[++ai], 0, 0));
        }
        rc = test_case(a, b, e);
    }
    else
    {
        rc = random_test(argc - 1, argv + 1);
    }
    return rc;
}
