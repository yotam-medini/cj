#include <algorithm>
#include <vector>
using namespace std;
typedef vector<int> vi_t;

size_t vpartition(vi_t& a, int threshold)
{
    const size_t sz = a.size();
    size_t i = 0;
    for ( ; (i < sz) && (a[i] < threshold); ++i) {}
    for (size_t j = i; j < sz; ++j)
    {
        if (a[j] < threshold)
        {
            swap(a[i], a[j]);
            ++i;
        }
    }
    return i;
}


#include <iostream>
#include <cstdlib>

static int test_vpartition(const vi_t& ca, int threshold)
{
    int rc = 0;
    vi_t a(ca);
    size_t lsz = vpartition(a, threshold);
    if (a.size() != ca.size())
    {
        rc = 1;
    }
    for (size_t i = 0; (rc == 0) && (i < lsz); ++i)
    {
        if (a[i] >= threshold)
        {
            rc = 1;
        }
    }
    for (size_t i = lsz; (rc == 0) && (i < a.size()); ++i)
    {
        if (a[i] < threshold)
        {
            rc = 1;
        }
    }
    if (rc == 0)
    {
        vi_t sa(ca);
        sort(sa.begin(), sa.end());
        sort(a.begin(), a.end());
        if (a != sa)
        {
            rc = 1;
        }
    }
    if (rc != 0)
    {
        cerr << "Failed: " << threshold << ' ';
        for (int x: ca) { cerr << ' ' << x; }
        cerr << '\n';
    }
    return rc;
}

static int test_permutations()
{
    int rc = 0;
    vi_t a{0, 2, 2, 4, 6};
    for (bool more = true; (rc == 0) && more; 
        more = next_permutation(a.begin(), a.end()))
    {
        for (int threshold = -1; (rc == 0) && (threshold < 8); ++threshold)
        {
            rc = test_vpartition(a, threshold);
        }
    }
    return rc;
}

int main(int argc, char** argv)
{
    int rc = 0;
    if (argc == 1)
    {
        rc = test_permutations();
    }
    else
    {
        int threshold = strtol(argv[1], nullptr, 0);
        vi_t a;
        for (int ai = 2; ai < argc; ++ai)
        {
            a.push_back(strtol(argv[ai], nullptr, 0));
        }
        rc = test_vpartition(a, threshold);
    }
    return rc;
}
