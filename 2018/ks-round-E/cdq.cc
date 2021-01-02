// Inspired by
// https://robert1003.github.io/2020/01/31/cdq-divide-and-conquer.html
#include <array>
#include <vector>
using namespace std;

typedef unsigned u_t;
typedef array<u_t, 3> au3_t;
typedef vector<u_t> vu_t;
typedef vector<au3_t> vau3_t;

class CDQ
{
 public:

};

#include <iostream>
#include <string>

static u_t count_below_naive(const au3_t& pt, const vau3_t& pts)
{
    u_t n = 0;
    return n;
}

static u_t count_below(const au3_t& pt, const vau3_t& pts)
{
    u_t n = 0;
    for (const au3_t& mpt: pts)
    {
        bool lt = true;
        for (size_t i = 0; lt && (i < 3); ++i)
        {
            lt = (mpt[i] < pt[i]);
            if (lt)
            {
                ++n;
            }
        }
    }
    return n;
}

static int test(const au3_t& pt, const vau3_t& pts)
{
    int rc = 0;
    u_t n_naive = count_below_naive(pt, pts);
    u_t n = count_below(pt, pts);
    if (n != n_naive)
    {
        rc = 1;
        cerr << "n=" << n << " != n_naive=" << n_naive << '\n';
        const char* sep = "";
        for (u_t x: pt) { cerr << sep << x; sep = " "; }
        cerr << '\n' << pts.size() << '\n';
        for (const au3_t& mpt: pts)
        {
            sep = "";
            for (u_t x: mpt) { cerr << sep << x; sep = " "; }
            cerr << '\n';
        }
    }
    return rc;
}

static int test_cin()
{
    au3_t pt;
    for (u_t& x: pt)
    {
        cin >> x;
    }
    size_t n_pt;
    cin >> n_pt;
    vau3_t pts; pts.reserve(n_pt);
    for (size_t pi = 0; pi < n_pt; ++pi)
    {
        au3_t mpt;
        for (u_t& x: mpt)
        {
            cin >> x;
        }
        pts.push_back(mpt);
    }
    return test(pt, pts);
}

static int test_random(int argc, char** argv)
{
    int rc = 0;
    int ai = 0;
    u_t nt = stoi(argv[ai++]);
    size_t npt = stoi(argv[ai++]);
    u_t vmax = stoi(argv[ai++]);
    for (u_t ti = 0; (rc == 0) && (ti < nt); ++ti)
    {
        au3_t pt;
        vau3_t pts; pts.reserve(npt);
        for (u_t& x: pt)
        {
            x = rand() % (vmax + 1);
        }
        while (pts.size() < npt)
        {
            au3_t mpt;
            for (u_t& x: mpt)
            {
                x = rand() % (vmax + 1);
            }
            pts.push_back(mpt);
        }
        rc = test(pt, pts);
    }
    return rc;
}

int main(int argc, char**argv)
{
    int rc = 0;
    if (argc == 1)
    {
        rc = test_cin();
    }
    else
    {
        rc = test_random(argc - 1, argv + 1);
    }
    return rc;
}

