#include <iostream>
#include <algorithm>
#include <vector>
#include <utility>

using namespace std;

typedef unsigned u_t;
typedef vector<int> vi_t;
typedef vector<vi_t> vvi_t;

template<typename R, typename RE>
class NextBoundBase
{
 public:
    NextBoundBase(const vi_t &values);
    int prev_bound(int before) const 
    {
        return prev_bound_value(before, get(before));
    }
    int next_bound(int after) const
    {
        return next_bound_value(after, get(after));
    }
    int prev_bound_value(int before, int v) const;
    int next_bound_value(int after, int v) const;
    int get(int i) const { return binm[0][i]; }
    int prev_bound_naive(int before) const
    {
        return prev_bound_value_naive(before, get(before));
    }
    int next_bound_naive(int after) const
    {
        return next_bound_value_naive(after, get(after));
    }
    int prev_bound_value_naive(int before, int v) const;
    int next_bound_value_naive(int after, int v) const;
 private:
    static int M(int x, int y) { return R()(x, y) ? x : y; }
    void fill_bin(vvi_t &bmax);
    vvi_t binm;
};

template<typename R, typename RE>
NextBoundBase<R, RE>::NextBoundBase(const vi_t &values)
{
    u_t sz = values.size();
    int log2 = 0;
    while ((1ul << log2) < sz)
    {
        ++log2;
    }
    binm.reserve(log2);
    binm.push_back(values);
    fill_bin(binm);
}

template<typename R, typename RE>
void NextBoundBase<R, RE>::fill_bin(vvi_t &bmax)
{
    while (bmax.back().size() > 1)
    {
        bmax.push_back(vi_t());
        vi_t &next = bmax.back();
        const vi_t &prev = bmax[bmax.size() - 2];
        u_t prev_sz = prev.size();
        next.reserve((prev_sz + 1) / 2);
        for (u_t i = 0, j = 0; j + 1 < prev_sz; ++i, j += 2)
        {
            next.push_back(M(prev[j], prev[j + 1]));
        }
        if (prev_sz % 2 != 0)
        {
            next.push_back(prev.back());
        }
    }    
}

template<typename R, typename RE>
int NextBoundBase<R, RE>::prev_bound_value(int before, int v) const
{
    int ret = -1;
    u_t p2b = u_t(-1), bib = u_t(-1);
    for (int p2 = int(binm.size()) - 1, bi = 0; p2 >= 0; --p2)
    {
        if ((bi + 1)*(1u << p2) <= unsigned(before))
        {
            if (RE()(binm[p2][bi], v))
            {
                p2b = p2;
                bib = bi;
            }
            bi = 2*bi + 2;
        }
        else
        {
            bi = 2*bi;
        }
    }
    if (p2b != u_t(-1))
    {
        while (p2b > 0)
        {
            --p2b;
            bib = 2*bib + 1;
            if (R()(v, binm[p2b][bib]))
            {
                --bib;
            }
            
        }
        ret = bib;
    }
    return ret;
}

template<typename R, typename RE>
int NextBoundBase<R, RE>::next_bound_value(int after, int v) const
{
    u_t sz = binm[0].size();
    int ret = sz;
    u_t p2b = u_t(-1), bib = u_t(-1);
    auto re = RE();
    for (int p2 = (re(binm.back()[0], v) ? binm.size() : 0) - 2, bi = 0;
        p2 >= 0; --p2)
    {
        unsigned bi1 = bi + 1;
        if (bi1*(1u << p2) > unsigned(after))
        {
            const vi_t& binm_p2 = binm[p2];
            if ((bi1 < binm_p2.size()) && re(binm_p2[bi1], v))
            {
                p2b = p2;
                bib = bi1;
            }
            bi = 2*bi;
        }
        else
        {
            bi = 2*bi + 2;
        }
    }
    if (p2b != u_t(-1))
    {
        while (p2b > 0)
        {
            --p2b;
            bib = 2*bib;
            if (R()(v, binm[p2b][bib]))
            {
                ++bib;
            }
            
        }
        ret = bib;
    }
    return ret;
}

template<typename R, typename RE>
int NextBoundBase<R, RE>::prev_bound_value_naive(int before, int v) const
{
    int ret = -1;
    for (int i = before - 1; i >= 0; --i)
    {
        if (RE()(get(i), v))
        {
            ret = i;
            i = 0;
        }
    }
    return ret;
}

template<typename R, typename RE>
int NextBoundBase<R, RE>::next_bound_value_naive(int after, int v) const
{
    int sz = binm[0].size();
    int ret = sz;
    for (int i = after + 1; i < sz; ++i)
    {
        if (RE()(get(i), v))
        {
            ret = i;
            i = sz;
        }
    }
    return ret;
}

typedef NextBoundBase<less<int>, less_equal<int>> NextBoundMin;
typedef NextBoundBase<greater<int>, greater_equal<int>> NextBoundMax;

#include <cstdlib>
#include <string>

static int nb_test_special(const NextBoundMax& nb, int ab)
{
    int rc = 0;
    int p_naive = nb.prev_bound_naive(ab);
    int p = nb.prev_bound(ab);
    int n_naive = nb.next_bound_naive(ab);
    int n = nb.next_bound(ab);
    if ((p != p_naive) || (n != n_naive))
    {
        cerr << "Failed\n p="<<p << ", p_naive="<<p_naive << 
            ", n="<<n << ", n_naive="<<n_naive << "\n";
        rc = 1;
    }
    return rc;
}

static int test_special(int argc, char **argv)
{
    int ab = strtoul(argv[0], 0, 0);
    vi_t values;
    for (int ai = 1; ai < argc; ++ai)
    {
        values.push_back(strtoul(argv[ai], 0, 0));
    }
    NextBoundMax nb(values);
    return nb_test_special(nb, ab);
}

static int nb_rand_test(u_t sz)
{
    int rc = 0;
    vi_t values;
    for (u_t i = 0; i < sz; ++i)
    {
        u_t v = rand() % sz;
        values.push_back(v);
    }
    NextBoundMax nb(values);
    for (u_t ab = 0; (ab < sz) && (rc == 0); ++ab)
    {
        rc = nb_test_special(nb, ab);
        if (rc != 0)
        {
            cerr << "  -special " << ab;
            for (int v: values) { cerr << ' ' << v; }  cerr << '\n';
            rc = 1;
        }
    }
    return rc;
}

static int nb_rand_tests(u_t ntests, u_t sizeb, u_t sizee)
{
    int rc = 0;
    for (u_t sz = sizeb; (sz < sizee) && (rc == 0); ++sz)
    {
        for (u_t t = 0; (t != ntests) && (rc == 0); ++t)
        {
            rc = nb_rand_test(sz);
        }
    }
    return rc;
}

int main(int argc, char **argv)
{
    int rc = 0;
    bool special = (argc > 1) && (string(argv[1]) == string("-special"));
    if (!((argc == 4) || special))
    {
        rc = 1;
        cerr << "Usage: " << argv[0] << " <#Tests> <size-b> <size-e>\n";
        cerr << "Usage: " << argv[0] << " -special <ab> <values ...>\n";
    }
    else if (special)
    {
        rc = test_special(argc -2, argv + 2);
    }
    else
    {
        int ai = 0;
        u_t ntests = strtoul(argv[++ai], 0, 0);
        u_t sizeb = strtoul(argv[++ai], 0, 0);
        u_t sizee = strtoul(argv[++ai], 0, 0);
        rc = nb_rand_tests(ntests, sizeb, sizee);
    }
    return rc;
}
