#include <algorithm>
#include <array>
#include <vector>
#include <string>
#include <set>
using namespace std;
typedef unsigned u_t;
typedef unsigned long long  ull_t;
typedef array<u_t, 2> au2_t;
typedef vector<u_t> vu_t;
typedef vector<vu_t> vvu_t;
typedef vector<vvu_t> vvvu_t;
typedef vector<vvvu_t> vvvvu_t;
typedef array<vu_t, 2> avu2_t;
typedef vector<au2_t> vau2_t;

class SegTree2D
{
 public:
    vvu_t mends[2];
    void init(const avu2_t& ends);
    // void init();
    void add(const au2_t& pt);
    ull_t n_gt(const au2_t& pt) const
    {
        return _a.empty() ? 0 : n_gt(pt, _a.size() - 1, 0);
    }
 private:
    ull_t n_gt(const au2_t& pt, size_t yi, size_t yii) const;
    ull_t y_n_gt(u_t x, const vvu_t& y, size_t xi, size_t xii) const;
    vvvvu_t _a;
};

void SegTree2D::init(const avu2_t& ends)
{
    for (int d: {0, 1})
    {
        vvu_t& e = mends[d];
        e.clear();
        e.push_back(ends[d]);
        while (e.back().size() > 1)
        {
            const vu_t& eback = e.back(); // strictly increasing
            const size_t bsz = eback.size();
            vu_t next; next.reserve((bsz + 1)/2);
            for (size_t i = 1, ie = bsz; i < ie; i += 2)
            {
                next.push_back(eback[i]);
            }
            if (next.back() != eback.back())
            {
                next.push_back(eback.back());
            }
            e.push_back(next);
        }
    }
    vvu_t x; x.reserve(mends[0].size());
    for (const vu_t& e: mends[0])
    {
        x.push_back(vu_t(e.size(), 0));
    }
    _a.clear(); _a.reserve(mends[1].size());
    for (const vu_t& e: mends[1])
    {
        _a.push_back(vvvu_t());
        vvvu_t& aback = _a.back();
        aback.insert(aback.end(), e.size(), x);
    }
}

void SegTree2D::add(const au2_t& pt)
{
    au2_t ii;
    for (u_t d: {0, 1})
    {
        const vu_t& e = mends[d][0];
        ii[d] = lower_bound(e.begin(), e.end(), pt[d]) - e.begin();
    }
    u_t yi = ii[1];
    for (vvvu_t& y: _a)
    {
        vvu_t& xx =  y[yi];
        u_t xi = ii[0]; 
        for (vu_t& x: xx)
        {
            ++x[xi];
            xi /= 2;
        }
        yi /= 2;
    }
}

ull_t SegTree2D::n_gt(const au2_t& pt, size_t yi, size_t yii) const
{
    ull_t n = 0;
    const vvvu_t& ayi = _a[yi];
    if (yii < ayi.size())
    {
        const u_t y = pt[1];
        if (y > mends[1][yi][yii])
        {
             n = y_n_gt(pt[0], _a[yi][yii], mends[0].size() - 1, 0);
        }
        else if (yi > 0)
        {
             n = n_gt(pt, yi - 1, 2*yii);
             n += n_gt(pt, yi - 1, 2*yii + 1);
        }
    }
    return n;    
}

ull_t SegTree2D::y_n_gt(u_t x, const vvu_t& y, size_t xi, size_t xii) const
{
    ull_t n = 0;
    const vu_t& yxi = y[xi];
    if (xii < yxi.size())
    {
        if (x > mends[0][xi][xii])
        {
            n = yxi[xii];
        }
        else if (xi > 0)
        {
            n = y_n_gt(x, y, xi - 1, 2*xii);
            n += y_n_gt(x, y, xi - 1, 2*xii + 1);
        }
    }   
    return n;
}

ull_t n_gt_naive(const au2_t& pt, const vau2_t& pts)
{
    ull_t n_naive = 0;
    for (const au2_t& p: pts)
    {
        if ((pt[0] > p[0]) && (pt[1] > p[1]))
        {
            ++n_naive;
        }
    }
    return n_naive;
}

#include <iostream>

void special_msg(const au2_t& pt, const vau2_t& pts)
{
    cerr << " special " << pt[0] << ' ' << pt[1];
    for (const au2_t& p: pts)
    {
        cerr << ' ' << p[0] << ' ' << p[1];
    }
    cerr << '\n';
}

#if defined(SEG2DTEST)
int test(const au2_t& pt, const vau2_t& pts)
{
    int rc = 0;
    ull_t n_naive = n_gt_naive(pt, pts);
    SegTree2D st;
    avu2_t ends;
    for (u_t d: {0, 1})
    {
        set<u_t> v;
        for (const au2_t& p: pts)
        {
            v.insert(p[d]);
        }
        ends[d] = vu_t(v.begin(), v.end());
    }
    st.init(ends);
    for (const au2_t& p: pts)
    {
        st.add(p);
    }
    ull_t n = st.n_gt(pt);
    if (n_naive != n)
    {
        rc = 1;
        cerr << "n_naive="<<n_naive << ", n="<<n << '\n';
        special_msg(pt, pts);
    }
    return rc;
}

u_t stou(const string& s)
{
   return u_t(stoi(s));
}

int test_specific(int argc, char** argv)
{
    au2_t pt{stou(argv[0]), stou(argv[1])};
    vau2_t pts;
    for (int ai = 2; ai < argc; ai += 2)
    {
        au2_t p{stou(argv[ai + 0]), stou(argv[ai + 1])};
        pts.push_back(p);
    }
    return test(pt, pts);
}

int test_rand(int argc, char** argv)
{
    int rc = 0;
    int ai = 0;
    u_t n_loops = stou(argv[ai++]);
    size_t n_pt = stou(argv[ai++]);
    u_t M = stou(argv[ai++]);
    for (u_t loopi = 0; (rc == 0) && (loopi < n_loops); ++loopi)
    {
        vau2_t pts;
        while (pts.size() < n_pt)
        {
            au2_t p{(rand() % M) + 1, (rand() % M) + 1};
            pts.push_back(p);
        }
        au2_t pt;
        for (pt[0] = 0; (rc == 0) && (pt[0] <= M + 1); ++pt[0])
        {
            for (pt[1] = 0; (rc == 0) && (pt[1] <= M + 1); ++pt[1])
            {
                rc = test(pt, pts);
            }
        }
    }
    return rc;
}

int main(int argc, char** argv)
{
    int rc = 0;
    if (string(argv[1]) == string("special"))
    {
        rc = test_specific(argc - 2, argv + 2);
    }
    else
    {
        rc = test_rand(argc - 1, argv + 1);
    }
    return rc;
}
#endif
