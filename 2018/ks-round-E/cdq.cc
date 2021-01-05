// Inspired by
// https://robert1003.github.io/2020/01/31/cdq-divide-and-conquer.html
#include <algorithm>
#include <array>
#include <vector>
using namespace std;

typedef unsigned u_t;
typedef array<u_t, 3> au3_t;
typedef vector<int> vi_t;
typedef vector<u_t> vu_t;
typedef vector<au3_t> vau3_t;

class BIT
{
 public:
    BIT(u_t _max_idx) : max_idx(_max_idx), tree(_max_idx + 1, 0) {}
    void update(u_t idx, int delta)
    {
        if (idx > 0)
        {
            while (idx <= max_idx)
            {
                tree[idx] += delta;
                idx += (idx & -idx);
            }
        }
    }
    int query(int idx) const
    {
        int n = 0;
        while (idx > 0)
        {
            n += tree[idx];
            idx -= (idx & -idx); // removing low bit
        }
        return n;
    }
 private:
    u_t max_idx;
    vi_t tree;
};

class CDQ
{
 public:
    CDQ(size_t _sz): sz(_sz), bit(_sz + 1)
    {
    }
    void add(const au3_t& pt);
    u_t n_below(const au3_t& pt); //  const;
    u_t n_eq_above(const au3_t& pt) { return pts.size() - n_eq_above(pt); }
 private:
    u_t cdq(const au3_t& pt, size_t l, size_t r);
    size_t sz;
    vau3_t pts;
    BIT bit;
};

void CDQ::add(const au3_t& pt)
{
    pts.push_back(pt); // increasing x
}

u_t CDQ::n_below(const au3_t& pt) //  const
{
    u_t n = cdq(pt, 0, pts.size());
    return n;
}

u_t CDQ::cdq(const au3_t& pt, size_t l, size_t r)
{
    u_t n = 0;
    if (l < r)
    {
        if (l + 1 == r)
        {
            const au3_t& mpt = pts[l];
            if ((mpt[1] < pt[1]) && (mpt[2] < pt[2]))
            {
                ++n;
            }
        }
        else
        {
            size_t mid = (l + r)/2;
            u_t nl = cdq(pt, l, mid);
            u_t nr = cdq(pt, mid, r);
            u_t nm = 0;
            n = nl + nr + nm;
        }
    }
    return n;
}

static void count_below(vu_t& res, const vau3_t& upts, const vau3_t& pts)
{
    vau3_t cpts(pts);
    sort(cpts.begin(), cpts.end(),
        [](const au3_t& pt0, const au3_t& pt1) -> bool
        {
            bool lt = (pt0[0] < pt1[0]);
            return lt;
        });
#if 0
    CDQ cdq(pts.size());
    for (size_t pi = 0, psz = cpts.size(); (pi < psz) && (cpts[pi][0] < pt[0]);
        ++pi)
    {
        cdq.add(cpts[pi]);
    }
    return cdq.n_below(pt);
#else
#endif
}

static void count_below_naive(vu_t& res, const vau3_t& upts, const vau3_t& pts)
{
    res.clear(); res.reserve(upts.size());
    for (const au3_t& upt: upts)
    {
        u_t n = 0;
        for (const au3_t& mpt: pts)
        {
            bool lt = true;
            for (size_t i = 0; lt && (i < 3); ++i)
            {
                lt = (mpt[i] < upt[i]);
            }
            if (lt)
            {
                ++n;
            }
        }
        res.push_back(n);
    }
}

#include <iostream>
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

static int test(const vau3_t& upts, const vau3_t& pts)
{
    int rc = 0;
    vu_t n_naive, n;
    count_below_naive(n_naive, upts, pts);
    count_below(n, upts, pts);
    if (n != n_naive)
    {
        rc = 1;
        cerr << "n= != n_naive=\n";
        vshow(cerr, "n:", n);
        vshow(cerr, "n_naive:", n_naive);
        cerr << upts.size() << '\n' << pts.size() << '\n';
        const char* sep = "";
        for (u_t mode: {0, 1})
        {
            for (const au3_t& mpt: (mode == 0 ? upts : pts)) 
            {
                sep = "";
                for (u_t x: mpt) { cerr << sep << x; sep = " "; }
                cerr << '\n';
            }
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
    size_t n_pt[2];
    cin >> n_pt[0] >> n_pt[1];
    vau3_t pts[2]; 
    for (u_t mode: {0, 1})
    {
        pts[mode].reserve(n_pt[mode]);
        for (size_t pi = 0; pi < n_pt[mode]; ++pi)
        {
            au3_t mpt;
            for (u_t& x: mpt)
            {
                cin >> x;
            }
            pts[mode].push_back(mpt);
        }
    }
    return test(pts[0], pts[1]);
}

static int test_random(int argc, char** argv)
{
    int rc = 0;
    int ai = 0;
    u_t nt = stoi(argv[ai++]);
    size_t npt[2];
    npt[0] = stoi(argv[ai++]);
    npt[1] = stoi(argv[ai++]);
    u_t vmax = stoi(argv[ai++]);
    for (u_t ti = 0; (rc == 0) && (ti < nt); ++ti)
    {
        vau3_t pts[2]; 
        for (u_t mode: {0, 1})
        {
            pts[mode].reserve(npt[mode]);
            while (pts[mode].size() < npt[mode])
            {
                au3_t mpt;
                for (u_t& x: mpt)
                {
                    x = rand() % (vmax + 1);
                }
                pts[mode].push_back(mpt);
            }
        }
        rc = test(pts[0], pts[1]);
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

