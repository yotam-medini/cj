#include <algorithm>
#include <vector>
using namespace std;

template <class T>
class RMQ_Naive
{
 public:
    typedef vector<T> vt_t;
    RMQ_Naive(const vt_t& _a) : a(_a) {}
    const vt_t& get_a() const { return a; }
    T range_min(size_t b, size_t e) const
    {
        return *min_element(a.begin() + b, a.begin() + e);
    }
 private:
    vt_t a;
};

template <class T>
class RMQ
{
 public:
    typedef vector<T> vt_t;
    RMQ(const vt_t& _a);
    const vt_t& get_a() const { return a; }
    T range_min(size_t b, size_t e) const
    {
        return get_min(0, 0, a.size() - 1, b, e);
    }
 private:
    void build(size_t i, size_t b, size_t e);
    T get_min(size_t i, size_t l, size_t r, size_t b, size_t e) const;
    vt_t a;
    vt_t tree;
};

template <class T>
RMQ<T>::RMQ(const vt_t& _a) : a(_a)
{
    const size_t sz = _a.size();
    tree.assign(4*sz, 0);
    build(0, 0, sz - 1);
}

template <class T>
void RMQ<T>::build(size_t i, size_t b, size_t e)
{
    if (b == e)
    {
        tree[i] = a[b];
    }
    else
    {
        const size_t lmid = b + (e - b)/2;
        const size_t rmid = lmid + 1;
        build(2*i + 1, b, lmid);
        build(2*i + 2, rmid, e);
        tree[i] = min(tree[2*i + 1], tree[2*i + 2]);
    }
}

template <class T>
T RMQ<T>::get_min(size_t i, size_t l, size_t r, size_t b, size_t e) const
{
    T ret = 0;
    if ((b <= l) && (r <= e))
    {
        ret = tree[i];
    }
    else
    {
        const size_t lmid = l + (r - l)/2;
        const size_t rmid = lmid + 1;
        if (e <= lmid)
        {
            ret = get_min(2*i + 1, l, lmid, b, e);
        }
        else if (rmid <= b)
        {
            ret = get_min(2*i + 2, rmid, r, b, e);
        }
        else
        {
            const T lret = get_min(2*i + 1, l, lmid, b, e);
            const T rret = get_min(2*i + 2, rmid, r, b, e);
            ret = min(lret, rret);
        }
    }
    return ret;
}

typedef vector<int> vi_t;
typedef RMQ_Naive<int> rmqni_t;
typedef RMQ<int> rmqi_t;

#include <iostream>

static 
int test_rmq2(const rmqni_t& rmqni, const rmqi_t& rmqi, size_t b, size_t e)
{
    int rc = 0;
    int mini_naive = rmqni.range_min(b, e);    
    int mini = rmqi.range_min(b, e);    
    if (mini != mini_naive)
    {
        rc = 1;
        cerr << "Failed: specific" << b << ' ' << e;
        for (int x: rmqni.get_a()) { cerr << ' ' << x; }
        cerr << '\n';
    }
    return rc;
}

static int test_rmq(const vi_t& a)
{
    const rmqni_t rnq_naive(a);    
    const rmqi_t rnq(a);
    int rc = 0;
    const size_t sz = a.size();
    for (size_t b = 0, b1 = 1; (rc == 0) && (b1 < sz); b = b1++)
    {
        for (size_t e = b1; (rc == 0) && (e < sz); ++e)
        {
            rc = test_rmq2(rnq_naive, rnq, b, e);
        }
    }
    return rc;
}

static int test_iter(vi_t a, size_t sz, int mini, int maxi)
{
    int rc = 0;
    if (a.size() < sz)
    {
        a.push_back(0);
        for (int x = mini; (rc == 0) && (x <= maxi); ++x)
        {
            a.back() = x;
            rc = test_rmq(a);
            if (rc == 0)
            {
                rc = test_rmq(a);
            }
        }
        a.pop_back();
    }
    return rc;
}

int main(int argc, char **argv)
{
    int rc = 0;
    int ai = 0;
    if (string(argv[1]) == string("specific"))
    {
        ai = 1;
        size_t b = strtoul(argv[++ai], nullptr, 0);
        size_t e = strtoul(argv[++ai], nullptr, 0);
        vi_t a;
        for (++ai; ai < argc; ++ai)
        {
            a.push_back(strtol(argv[ai], nullptr, 0));
        }
        rc = test_rmq2(rmqni_t(a), rmqi_t(a), b, e);
    }
    else
    {
        size_t sz = strtoul(argv[++ai], nullptr, 0);
        int mini = strtoul(argv[++ai], nullptr, 0);
        int maxi = strtoul(argv[++ai], nullptr, 0);
        cerr << "Testing combinatoions with sz<=" << sz << ", " <<
            mini << " <= x <= " << maxi << '\n';
        vi_t a;
        rc = test_iter(a, sz, mini, maxi);
    }
    return rc;
}
