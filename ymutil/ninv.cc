#include <algorithm>
#include <vector>
using namespace std;
typedef unsigned u_t;
typedef vector<u_t> vu_t;

u_t n_inversions_naive(const vu_t& a)
{
    u_t n = 0;
    const size_t sz = a.size();
    for (size_t i = 0; i < sz; ++i)
    {
        for (size_t j = i + 1; j < sz; ++j)
        {
            if (a[i] > a[j])
            {
                ++n;
            }
        }
    }
    return n;
}

static u_t n_inv_merge_sort_be(vu_t& a, size_t b, size_t e)
{
    u_t n = 0;
    if (b + 1 < e)
    {
        size_t mid = (b + e)/2;
        const u_t nl = n_inv_merge_sort_be(a, b, mid);
        const u_t nr = n_inv_merge_sort_be(a, mid, e);
        u_t nm = 0;
        // merge
        vu_t::iterator abeg = a.begin();
        for (size_t i = b, j = mid; (i < mid) && (j < e); )
        {
            while ((i < mid) && (a[i] <= a[j]))
            {
                ++i;
            }
            if (i < mid)
            {
                const u_t ai = a[i];
                const u_t lsz = mid - i;
                while ((j < e) && (a[j] < ai))
                {
                    ++j;
                    nm += lsz;
                }
                rotate(abeg + i, abeg + mid, abeg + j);
                mid = j;
            }
        }
        n = nl + nr + nm;
    }
    return n;
}

u_t n_inversions(const vu_t& a)
{
    vu_t c(a);
    u_t n = n_inv_merge_sort_be(c, 0, c.size());
    return n;
}

#include <algorithm>
#include <iostream>
#include <numeric>
#include <string>
ostream& vshow(ostream& os, const string& msg, const vu_t& a)
{
    os << msg;
    for (u_t x: a)
    {
        os << ' ' << x;
    }
    return os << '\n';
}

int test(const vu_t& a)
{
    int rc = 0;
    u_t n_inv_naive = n_inversions_naive(a);
    u_t n_inv = n_inversions(a);
    if (n_inv != n_inv_naive)
    {
        rc = 1;
        cerr << "n_inv=" << n_inv << " != n_inv_naive=" << n_inv_naive << '\n';
        vshow(cerr, "a:", a);
    }
    return rc;
}

int test_permutations()
{
    int rc = 0;
    for (size_t sz = 0; (rc == 0) && (sz <= 10); ++sz)
    {
        cerr << __func__ << " sz=" << sz << '\n';
        vu_t a(sz);
        iota(a.begin(), a.end(), 0);
        for (bool more = true; (rc == 0) && more;
            more = next_permutation(a.begin(), a.end()))
        {
            rc = test(a);
        }
    }
    return rc;
}

int main(int argc, char** argv)
{
    int rc = 0;
    if (argc > 1)
    {
        vu_t a;
        for (int ai = 1; ai < argc; ++ai)
        {
            a.push_back(stoi(argv[ai]));
        }
        rc = test(a);
    }
    else
    {
        rc = test_permutations();
    }
    return rc;
}
