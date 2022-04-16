#include <iostream>
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
    e_t query_sum(u_t b, u_t e) const;
    void print(ostream& os) const;
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
    print(cerr);
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
    alevel[pos] -= val;
    if ((pos % 2 != 0) || (pos + 1 < alevel.size()))
    {
        level_update(level + 1, pos / 2, old_val, val);
    }
}

void SegmentTree::print(ostream& os) const
{
    for (u_t level = 0; level < a.size(); ++level)
    {
        const ve_t& alevel = a[level];
        os << "[" << level << "]";
        for (e_t x: alevel)
        {
            os << ' ' << x;
        }
        os << '\n';
    }
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
        cerr << "Failure: specific " << b << ' ' << e;
        for (ull_t x: a) { cerr << ' ' << x; }
        cerr << '\n';
    }
    return rc;
}

static int test_case(const vull_t& a, u_t b, u_t e)
{
    ull_t naive_sum = accumulate(a.begin() + b, a.begin() + e, 0ull);
    SegmentTree segtree(a);
    ull_t st_sum = segtree.query_sum(b, e);
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
            ull_t st_sum = segtree.query_sum(b, e);
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
        int ai = 2;
        u_t b = strtoul(argv[ai++], 0, 0);
        u_t e = strtoul(argv[ai++], 0, 0);
        vull_t a;
        for (; ai < argc; ++ai)
        {
            a.push_back(strtoul(argv[ai++], 0, 0));
        }
        rc = test_case(a, b, e);
    }
    else
    {
        rc = random_test(argc - 1, argv + 1);
    }
    return rc;
}
