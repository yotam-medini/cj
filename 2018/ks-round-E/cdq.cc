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
    IPoint(int _i=-1, const au3_t& _v={0, 0, 0}) : i(_i), v(_v) {}
    int i;
    au3_t v;
};
typedef vector<IPoint> vipt_t;

// inspired by: 
// robert1003.github.io/2020/01/31/cdq-divide-and-conquer.html
class CDQ
{
 public:
    CDQ(const vau3_t& upts, const vau3_t& pts) :
        n_below(upts.size(), 0),
        bit(5*1000000)
    {
        ipts.reserve(upts.size() + pts.size());
        for (size_t i = 0, sz = upts.size(); i < sz; ++i)
        {
            const au3_t& upt = upts[i];
            const au3_t& upt1{upt[0] - 1, upt[1] - 1, upt[2] - 1}; // Z as-is ?
            ipts.push_back(IPoint(i, upt1));
        }
        for (const au3_t& v: pts)
        {
            ipts.push_back(IPoint(-1, v));
        }
        solve();
    }
    vu_t n_below;
 private:
    void solve();
    void cdq(size_t l, size_t r);
    vipt_t ipts;
    BIT bit;
};

void CDQ::solve()
{
    sort(ipts.begin(), ipts.end(),
        [](const IPoint& p0, const IPoint& p1) -> bool
        {
            size_t i;
            for (i = 0; (i < 3) && (p0.v[i] == p1.v[i]); ++i) {}
            bool lt = (i < 3 ? (p0.v[i] < p1.v[i]) : p0.i < p1.i);
            return lt;
        });
    u_t sz = ipts.size();
    cdq(0, sz);
}

void CDQ::cdq(size_t l, size_t r)
{
    if (l + 1 < r)
    {
        size_t mid = (l + r)/2;
        cdq(l, mid);
        cdq(mid, r);
        // Now ipts[l,mid) [0(x)] <= ipts[mid,r) [0(x)]
        // and both have increasing [1(y)]
        vu_t record;
        vipt_t tmp; tmp.reserve(r - l);
        size_t a = l, b = mid;
        u_t sum = 0;
        while(a < mid && b < r)
        {
            const IPoint& ipta = ipts[a];
            const IPoint& iptb = ipts[b];
            if (ipta.v[1] <= iptb.v[1])
            {
                if (ipta.i == -1)
                {
                    bit.update(ipta.v[2], 1);
                    sum++;
                    record.push_back(ipta.v[2]);
                }
                tmp.push_back(ipta);
                ++a;
            } 
            else
            {
                if (iptb.i >= 0)
                {
                    n_below[iptb.i] += sum - bit.cquery(iptb.v[2]);
                }
                tmp.push_back(iptb);
                ++b;
            }
        }
        for ( ; a < mid; ++a)
        {
            tmp.push_back(ipts[a]);
        }
        for ( ; b < r; ++b)
        {
            const IPoint& iptb = ipts[b];
            if (iptb.i >= 0)
            {
                n_below[iptb.i] += sum - bit.cquery(iptb.v[2]);
            }
            tmp.push_back(iptb);
        }
        copy(tmp.begin(), tmp.end(), ipts.begin() + l);
        for (u_t z: record)
        {
            bit.update(z, -1);
        }
    }
}

static void count_below(vu_t& res, const vau3_t& upts, const vau3_t& pts)
{
    CDQ cdq(upts, pts);
    res = cdq.n_below;
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
        cerr << "n= != n_naive\n";
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

