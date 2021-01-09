// Inspired by
// https://robert1003.github.io/2020/01/31/cdq-divide-and-conquer.html
#include <algorithm>
#include <array>
#include <vector>
using namespace std;

typedef unsigned u_t;
typedef array<u_t, 2> au2_t;
typedef vector<int> vi_t;
typedef vector<u_t> vu_t;
typedef vector<au2_t> vau2_t;

class BIT
{
 public:
    BIT(u_t _max_idx) : 
        max_idx(_max_idx), 
        tree(_max_idx + 1, 0),
        total_delta(0)
    {}
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
        total_delta += delta;
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
    int cquery(int idx) const // complement query
    {
        int n = total_delta - query(idx);
        return n;
    }
 private:
    u_t max_idx;
    vi_t tree;
    u_t total_delta;
};

class IPoint
{
 public:
    IPoint(int _i=-1, const au2_t& _v={0, 0}) : i(_i), v(_v) {}
    int i;
    au2_t v;
};
typedef vector<IPoint> vipt_t;

// inspired by: 
// robert1003.github.io/2020/01/31/cdq-divide-and-conquer.html
class CDQ2
{
 public:
    CDQ2(const vau2_t& _upts, const vau2_t& _pts) :
        n_below(_upts.size(), 0),
        pts(_pts),
        bit(5*1000000)
    {
        ipts.reserve(_upts.size());
        for (size_t i = 0, sz = _upts.size(); i < sz; ++i)
        {
            const au2_t& upt = _upts[i];
            const au2_t& upt1{upt[0] - 1, upt[1] - 1};
            ipts.push_back(IPoint(i, upt1));
        }
        solve();
    }
    vu_t n_below;
 private:
    void solve();
    vipt_t ipts;
    vau2_t pts;
    BIT bit;
};

void CDQ2::solve()
{
    sort(ipts.begin(), ipts.end(),
        [](const IPoint& p0, const IPoint& p1) -> bool
        {
            size_t i;
            for (i = 0; (i < 2) && (p0.v[i] == p1.v[i]); ++i) {}
            bool lt = (i < 2 ? (p0.v[i] < p1.v[i]) : p0.i < p1.i);
            return lt;
        });
    sort(pts.begin(), pts.end(),
        [](const au2_t& p0, const au2_t& p1) -> bool
        {
            size_t i;
            for (i = 0; (i < 2) && (p0[i] == p1[i]); ++i) {}
            bool lt = (i < 2) && (p0[i] < p1[i]);
            return lt;
        });
    const size_t isz = ipts.size();
    const size_t bsz = pts.size();
    vu_t record;
    u_t sum = 0;
    size_t a = 0, b = 0;
    while ((a < isz) && (b < bsz))
    {
        const IPoint& a_ipt = ipts[a];
        const au2_t& b_pt = pts[b];
        if (b_pt[0] <= a_ipt.v[0])
        {
            u_t y = b_pt[1];
            bit.update(y, 1);
            record.push_back(y);
            ++sum;
            ++b;
        }
        else
        {
            n_below[a_ipt.i] += sum - bit.cquery(a_ipt.v[1]);
            ++a;
        }
    }
    while (a < isz)
    {
        const IPoint& a_ipt = ipts[a];
        n_below[a_ipt.i] += sum - bit.cquery(a_ipt.v[1]);
        ++a;
    }
    for (u_t y: record)
    {
        bit.update(y, -1);
    }
}

static void count_below(vu_t& res, const vau2_t& upts, const vau2_t& pts)
{
    CDQ2 cdq(upts, pts);
    res = cdq.n_below;
}

static void count_below_naive(vu_t& res, const vau2_t& upts, const vau2_t& pts)
{
    res.clear(); res.reserve(upts.size());
    for (const au2_t& upt: upts)
    {
        u_t n = 0;
        for (const au2_t& mpt: pts)
        {
            bool lt = true;
            for (size_t i = 0; lt && (i < 2); ++i)
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

static int test(const vau2_t& upts, const vau2_t& pts)
{
    int rc = 0;
    vu_t n_naive, n;
    count_below_naive(n_naive, upts, pts);
    count_below(n, upts, pts);
    if (n != n_naive)
    {
        rc = 1;
        cerr << "n= != n_naive\n";
        vshow(cerr, "n:", n);
        vshow(cerr, "n_naive:", n_naive);
        cerr << upts.size() << '\n' << pts.size() << '\n';
        const char* sep = "";
        for (u_t mode: {0, 1})
        {
            for (const au2_t& mpt: (mode == 0 ? upts : pts)) 
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
    size_t n_pt[2];
    cin >> n_pt[0] >> n_pt[1];
    vau2_t pts[2]; 
    for (u_t mode: {0, 1})
    {
        pts[mode].reserve(n_pt[mode]);
        for (size_t pi = 0; pi < n_pt[mode]; ++pi)
        {
            au2_t mpt;
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
        vau2_t pts[2]; 
        for (u_t mode: {0, 1})
        {
            pts[mode].reserve(npt[mode]);
            while (pts[mode].size() < npt[mode])
            {
                au2_t mpt;
                for (u_t& x: mpt)
                {
                    x = (rand() % vmax) + 1;
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

