#include <algorithm>
#include <numeric>
#include <vector>
using namespace std;
typedef unsigned u_t;
typedef vector<u_t> vu_t;
typedef vector<vu_t> vvu_t;

typedef int stval_t;
typedef vector<stval_t> vval_t;
typedef vector<vval_t> vvval_t;

class SegTree1D
{
 public:
    SegTree1D(size_t _n=1) : n(_n), st(2*_n, 0) {}
    void add(size_t i, stval_t v)
    {
        i += n;
        st[i] += v;
        for (size_t iup = 0; i > 1; i = iup)
        {
            iup = i / 2;
            st[iup] = st[i] + st[i ^ 1];
        }
    }
    stval_t query(size_t b, size_t e) const
    {
        size_t ret = 0;
        for (b += n, e += n; b < e; b /= 2, e /= 2)
        {
            if (b % 2 == 1)
            {
                ret += st[b++];
            } 
            if (e % 2 == 1)
            {
                ret += st[--e];
            } 
        }
        return ret;
    }
 private:
    size_t n;
    vval_t st;
};

class SegTree2D
{
 public:
    SegTree2D(size_t _m=1, size_t _n=1) :
        m(_m), n(_n), st(2*_m, vval_t(2*_n, 0)) {}
    void add(size_t x, size_t y, stval_t v)
    {
        x += m;
        size_t yj = y + n;
        st[x][yj] += v;
        for (size_t yup = 0; yj > 1; yj = yup)
        {
            yup = yj / 2;
            st[x][yup] = st[x][yj] + st[x][yj ^ 1];
        }

        for (size_t xup = 0; x > 1; x = xup)
        {
            xup = x / 2;
            yj = y + n;
            st[xup][yj] = st[x][yj] + st[x ^ 1][yj];
            for (size_t yup = 0; yj > 1; yj = yup)
            {
                yup = yj / 2;
                st[xup][yup] = st[x][yup] + st[x ^ 1][yup];
            }
        }
    }
    stval_t query(size_t xb, size_t xe, size_t yb, size_t ye) const
    {
        size_t ret = 0;
        for (xb += m, xe += m; xb < xe; xb /= 2, xe /= 2)
        {
            if (xb % 2 == 1)
            {
                ret += query1d(st[xb++], yb, ye);
            } 
            if (xe % 2 == 1)
            {
                ret += query1d(st[--xe], yb, ye);
            } 
        }
        return ret;
    }
 private:
    stval_t query1d(const vval_t& st1d, size_t yb, size_t ye) const
    {
        size_t ret = 0;
        for (yb += n, ye += n; yb < ye; yb /= 2, ye /= 2)
        {
            if (yb % 2 == 1)
            {
                ret += st1d[yb++];
            } 
            if (ye % 2 == 1)
            {
                ret += st1d[--ye];
            } 
        }
        return ret;
    }
    size_t m, n;
    vvval_t st;
};

#include <iostream>
#include <string>

int test_1d(const vval_t& a)
{
    int rc = 0;
    size_t n = a.size();
    SegTree1D segtree(n);
    for (size_t i = 0; i < n; ++i)
    {
        segtree.add(i, a[i]);
    }
    for (size_t b = 0; (rc == 0) && (b < n); ++b)
    {
        for (size_t e = b; (rc == 0) && (e <= n); ++e)
        {
            stval_t qnaive = accumulate(a.begin() + b, a.begin() + e, 0);
            stval_t q = segtree.query(b, e);
            if (q != qnaive)
            {
                rc = 1;
                cerr << "SegTree1D error: qnaive=" << qnaive << 
                    ", q=" << q << "\nspec1d";
                for (stval_t v: a) { cerr << ' ' << v; }
                cerr << "\n b=" << b << ", e=" << e << '\n';
            }
        }
    }
    return rc;
}

int test_1d_specific(int argc, char** argv)
{
    vval_t a;
    for (int i = 0; i < argc; ++i)
    {
        a.push_back(stoi(argv[i]));
    }
    return test_1d(a);
}

int test_1d_rand(int argc, char** argv)
{
    int rc = 0;
    int ai = 0;
    size_t nt = stoi(argv[ai++]);
    size_t n = stoi(argv[ai++]);
    size_t M = stoi(argv[ai++]);
    for (size_t ti = 0; (rc == 0) && (ti < nt); ++ti)
    {
        vval_t a;
        while (a.size() < n)
        {
            a.push_back(rand() % (M + 1));
        }
        rc = test_1d(a);
    }
    return rc;
}

int test_2d(const vvval_t& a)
{
    int rc = 0;
    size_t m = a.size();
    size_t n = a[0].size();
    SegTree2D segtree(m, n);
    for (size_t i = 0; i < m; ++i)
    {
        for (size_t j = 0; j < n; ++j)
        {
            segtree.add(i, j, a[i][j]);
        }
    }
    for (size_t xb = 0; (rc == 0) && (xb < m); ++xb)
    {
        for (size_t xe = xb; (rc == 0) && (xe <= m); ++xe)
        {
            for (size_t yb = 0; (rc == 0) && (yb < n); ++yb)
            {
                for (size_t ye = yb; (rc == 0) && (ye <= n); ++ye)
                {
                    stval_t qnaive = 0;
                    for (size_t r = xb; r < xe; ++r)
                    {
                        qnaive += accumulate(
                            a[r].begin() + yb, a[r].begin() + ye, 0);
                    }
                    stval_t q = segtree.query(xb, xe, yb, ye);
                    if (q != qnaive)
                    {
                        rc = 1;
                        cerr << "SegTree2D error: qnaive=" << qnaive << 
                            ", q=" << q << "\nspec2d " << m << ' ' << n;
                        for (const vval_t& row: a) {
                            for (stval_t v: row) {
                                cerr << ' ' << v; } }
                        cerr << "\n xb=" << xb << ", xe=" << xe <<
                        ", yb=" << yb << ", ye=" << ye << '\n';
                    }
                }
            }
        }
    }
    return rc;
}

int test_2d_specific(int argc, char** argv)
{
    int ai = 0;
    size_t m = stoi(argv[ai++]);
    size_t n = stoi(argv[ai++]);
    vvval_t a(m, vval_t(n, 0));
    for (size_t i = 0; i < m; ++i)
    {
        for (size_t j = 0; j < n; ++j, ++ai)
        {
            a[i][j] = stoi(argv[ai]);
        }
    }
    return test_2d(a);
}

int test_2d_rand(int argc, char** argv)
{
    int rc = 0;
    int ai = 0;
    size_t nt = stoi(argv[ai++]);
    size_t m = stoi(argv[ai++]);
    size_t n = stoi(argv[ai++]);
    size_t M = stoi(argv[ai++]);
    for (size_t ti = 0; (rc == 0) && (ti < nt); ++ti)
    {
        vvval_t a(m, vval_t(n, 0));
        for (size_t i = 0; i < m; ++i)
        {
            for (size_t j = 0; j < n; ++j)
            {
                a[i][j] = rand() % (M + 1);
            }
        }
        rc = test_2d(a);
    }
    return rc;
}

int main(int argc, char **argv)
{
    int rc = 0;
    const string cmd(argv[1]);
    if (cmd == string("spec1d"))
    {
        rc = test_1d_specific(argc - 2, argv + 2);
    }
    else if (cmd == string("rand1d"))
    {
        rc = test_1d_rand(argc - 2, argv + 2);
    }
    else if (cmd == string("spec2d"))
    {
        rc = test_2d_specific(argc - 2, argv + 2);
    }
    else if (cmd == string("rand2d"))
    {
        rc = test_2d_rand(argc - 2, argv + 2);
    }
    else
    {
        cerr << "Unsupported cmd: " << cmd << '\n';
        rc = 1;
    }
    return rc;
}
