#include <algorithm>
#include <vector>
using namespace std;

typedef unsigned u_t;
typedef vector<u_t> vu_t;

class CycleDist
{
 public:
    CycleDist(const vu_t& _fwd, const vu_t& _bwd); // _fwd.size() == _bwd.size()
    void set(const vu_t& _fwd, const vu_t& _bwd); // _fwd.size() == _bwd.size()
    u_t dist(u_t i, u_t j) const;
    u_t dist_naive(u_t i, u_t j) const;
 private:
    void set_sums();
    vu_t fwd, bwd;
    vu_t fwd_sum, bwd_sum;
};

CycleDist::CycleDist(const vu_t& _fwd, const vu_t& _bwd) : fwd(_fwd), bwd(_bwd)
{
    set_sums();
}

void CycleDist::set(const vu_t& _fwd, const vu_t& _bwd)
{
    fwd = _fwd;
    bwd = _bwd;
    set_sums();
}

void CycleDist::set_sums()
{
    const size_t n = fwd.size();
    fwd_sum.clear();
    bwd_sum.clear();
    fwd_sum.reserve(n);
    bwd_sum.reserve(n);
    fwd_sum.push_back(0);
    bwd_sum.push_back(0);
    for (size_t i = 0; i < n; ++i)
    {
        fwd_sum.push_back(fwd_sum.back() + fwd[i]);
        bwd_sum.push_back(bwd_sum.back() + bwd[i]);
    }
}

u_t CycleDist::dist(u_t i, u_t j) const
{
    u_t d = 0;
    if (i != j)
    {
        u_t d_fwd = 0, d_bwd = 0;
        if (i < j)
        {
            d_fwd = fwd_sum[j] - fwd_sum[i];
            d_bwd = (bwd_sum.back() - bwd_sum[j]) + bwd_sum[i];
        }
        else // if (j < i)
        {
            d_fwd = (fwd_sum.back() - fwd_sum[i]) + fwd_sum[j];
            d_bwd = bwd_sum[i] - bwd_sum[j];
        }
        d = min(d_fwd, d_bwd);
    }
    return d;
}

u_t CycleDist::dist_naive(u_t i, u_t j) const
{
    const u_t n = fwd.size();
    u_t d_fwd = 0, d_bwd = 0;
    for (u_t c = i; c != j; c = (c + 1) % n)
    {
        d_fwd += fwd[c];
    }
    const u_t m1 = n - 1;
    for (u_t c = i, c1 = (c + m1) % n; c != j; c = c1, c1 = (c1 + m1) % n)
    {
        d_bwd += bwd[c1];
    }
    u_t d = min(d_fwd, d_bwd);
    return d;
}

#include <iostream>
#include <cstdlib>

int test(const vu_t& fwd, const vu_t& bwd, u_t i0, u_t j0)
{
    int rc = 0;
    const size_t n = fwd.size();
    CycleDist cycle_dist(fwd, bwd);
    for (size_t i = 0; (i < n) && (rc == 0); ++i)
    {
        for (size_t j = 0; (j < n) && (rc == 0); ++j)
        {
            u_t ii = (i + i0) % n;
            u_t jj = (j + j0) % n;
            u_t d = cycle_dist.dist(ii, jj);
            u_t d_naive = cycle_dist.dist_naive(ii, jj);
            if (d != d_naive)
            {
                rc = 1;
                cerr << "Failed: d="<<d << " != d_naive="<<d_naive << "\n";
                cerr << "special";
                for (u_t x: fwd) { cerr << ' ' << x; } 
                for (u_t x: bwd) { cerr << ' ' << x; } 
                cerr << ' ' << ii << ' ' << jj << '\n';
            }
        }
    }
    return rc;
}

int test_special(int argc, char **argv)
{
    size_t n = (argc - 2 - 2) / 2;
    vu_t fwd, bwd; fwd.reserve(n); bwd.reserve(n);
    for (size_t i = 0; i < n; ++i)
    {
        fwd.push_back(stoi(argv[i + 2]));
        bwd.push_back(stoi(argv[i + n + 2]));
    }
    u_t i0 = stoi(argv[argc - 2]);
    u_t j0 = stoi(argv[argc - 1]);
    return test(fwd, bwd, i0, j0);
}

int test_random(int argc, char **argv)
{
    int rc = 0, ai = 1;
    size_t n_tests = stoi(argv[++ai]);
    size_t n_min = stoi(argv[++ai]);
    size_t n_max = stoi(argv[++ai]);
    size_t v_min = stoi(argv[++ai]);
    size_t v_max = stoi(argv[++ai]);
    for (u_t ti = 0; (ti < n_tests) && (rc == 0); ++ti)
    {
        cerr << "tested: " << ti << '/' << n_tests << '\n';
        const u_t n = n_min + rand() % (n_max + 1 - n_min);
        vu_t fwd, bwd; fwd.reserve(n); bwd.reserve(n);
        const u_t dv = v_max + v_min;
        for (size_t i = 0; i < n; ++i)
        {
            fwd.push_back(v_min + rand() % dv);
            bwd.push_back(v_min + rand() % dv);
        }
        u_t i0 = rand() % n;
        u_t j0 = rand() % n;
        rc = test(fwd, bwd, i0, j0);
    }
    return rc;
}

int main(int argc, char** argv)
{
    int rc = 0;
    if (string(argv[1]) == string("special"))
    {
        rc = test_special(argc, argv);
    }
    else if (string(argv[1]) == string("random"))
    {
        rc = test_random(argc, argv);
    }
    else
    {
        rc = 1;
    }
    return rc;
}
