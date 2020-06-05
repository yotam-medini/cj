#include <vector>
#include <algorithm>
using namespace std;
typedef unsigned u_t;
typedef vector<int> vi_t;

int n_near_pairs(const vi_t& a, const vi_t& b, int bound_ab, int bound_ba)
{
    int n = 0;
    if (!(a.empty() || b.empty()))
    {
        vi_t sa(a);
        vi_t sb(b);
        sort(sa.begin(), sa.end());
        sort(sb.begin(), sb.end());
        const u_t asz = sa.size(), bsz = sb.size();
        const int x0 = sa[0];
        u_t jmin = 0, jmax = 0;
        for (; (jmin < bsz) && (sb[jmin] < x0 - bound_ab); ++ jmin) {}
        for (; (jmax < bsz) && (sb[jmax] <= x0 + bound_ba); ++ jmax) {}
        if (jmin < jmax)
        {
            n = jmax - jmin;
        }
        for (u_t i = 1; i < asz; ++i)
        {
            const int x = sa[i];
            for (; (jmin < bsz) && (sb[jmin] < x - bound_ab); ++ jmin) {}
            for (; (jmax < bsz) && (sb[jmax] <= x + bound_ba); ++ jmax) {}
            if (jmin < jmax)
            {
                n += jmax - jmin;
            }
        }
    }
    return n;
}

int n_near_pairs_naive(const vi_t& a, const vi_t& b, int bound_ab, int bound_ba)
{
    int n = 0;
    for (int x: a)
    {
        for (int y: b)
        {
            if ((x - y <= bound_ab) && (y - x <= bound_ba))
            {
                ++n;
            }
        }
    }
    return n;
}

#include <iostream>
#include <cstdlib>

static int test(const vi_t& a, const vi_t& b, int bound_ab, int bound_ba)
{
    int rc = 0;
    int n = n_near_pairs(a, b, bound_ab, bound_ba);
    int n_naive = n_near_pairs_naive(a, b, bound_ab, bound_ba);
    if (n != n_naive)
    {
        rc = 1;
        cerr << "Failed: n="<<n << ", n_naive=" << n_naive << '\n';
        cerr << " special";
        for (int x: a) { cerr << ' ' << x; }
        cerr << " :";
        for (int y: b) { cerr << ' ' << y; }
        cerr << " : " << bound_ab << ' ' << bound_ba << '\n';
    }
    return rc;
}

static int range_rand(int nmin, int nmax)
{
    int ret = nmin;
    if (nmin < nmax)
    {
        int delta = nmax - nmin;
        ret += rand() % delta;
    }
    return ret;
}

static int test_rand(
    u_t n_tests,
    int asz_min, int asz_max, int amin, int amax,
    int bsz_min, int bsz_max, int bmin, int bmax,
    int bound_ab_min, int bound_ab_max, 
    int bound_ba_min, int bound_ba_max)
{
    int rc = 0;
    for (u_t ti = 0; (rc == 0) && (ti < n_tests); ++ti)
    {
        u_t asz = range_rand(asz_min, asz_max);
        u_t bsz = range_rand(asz_min, asz_max);
        vi_t a, b;
        a.reserve(asz);
        b.reserve(bsz);
        for (u_t i = 0; i < asz; ++i) { a.push_back(range_rand(amin, amax)); }
        for (u_t i = 0; i < bsz; ++i) { b.push_back(range_rand(bmin, bmax)); }
        int bound_ab = range_rand(bound_ab_min, bound_ab_max);
        int bound_ba = range_rand(bound_ba_min, bound_ba_max);
        rc = test(a, b, bound_ab, bound_ba);
    }
    return rc;
}

#include <string>

int main(int argc, char** argv)
{
    int rc = 0;
    if (string(argv[1]) == "special")
    {
        vi_t a, b;
        int ai;
        for (ai = 2; argv[ai][0] != ':'; ++ai)
        {
            a.push_back(stoi(argv[ai]));
        }
        for (++ai; argv[ai][0] != ':'; ++ai)
        {
            b.push_back(stoi(argv[ai]));
        }
        int bound_ab = stoi(argv[++ai]);
        int bound_ba = stoi(argv[++ai]);
        rc = test(a, b, bound_ab, bound_ba);
    }
    else
    {
        int ai = 0;
        u_t n_tests = stoi(argv[++ai]);
        int asz_min = stoi(argv[++ai]);
        int asz_max = stoi(argv[++ai]);
        int amin = stoi(argv[++ai]);
        int amax = stoi(argv[++ai]);
        int bsz_min = stoi(argv[++ai]);
        int bsz_max = stoi(argv[++ai]);
        int bmin = stoi(argv[++ai]);
        int bmax = stoi(argv[++ai]);
        int bound_ab_min = stoi(argv[++ai]);
        int bound_ab_max = stoi(argv[++ai]);
        int bound_ba_min = stoi(argv[++ai]);
        int bound_ba_maxn = stoi(argv[++ai]);
        rc = test_rand(n_tests,
            asz_min, asz_max, amin, amax,
            bsz_min, bsz_max, bmin, bmax,
            bound_ab_min, bound_ab_max, 
            bound_ba_min, bound_ba_maxn);
    }
    return rc;
}
