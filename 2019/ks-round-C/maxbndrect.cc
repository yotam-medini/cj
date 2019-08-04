#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

typedef unsigned u_t;
typedef vector<u_t> vu_t;
typedef pair<u_t, u_t> uu_t;
typedef vector<uu_t> vuu_t;

u_t max_bounded_rect_naive(const vu_t& v, vuu_t& be)
{
    u_t max_rect = 0;
    u_t sz = v.size();
    vu_t::const_iterator vb = v.begin();
    for (u_t bi = 0; bi < sz; ++bi)
    {
        for (u_t ei = bi + 1; ei <= sz; ++ei)
        {
            vu_t::const_iterator imin = min_element(vb + bi, vb + ei);
            u_t vi = *imin;
            u_t rect = vi * (ei - bi);
            if (max_rect <= rect)
            {
                if (max_rect < rect)
                {
                    max_rect = rect;
                    be.clear();
                }
                be.push_back(uu_t(bi, ei));
            }
        }
    }
    return max_rect;
}

class MaxBoundRect
{
 public:
    MaxBoundRect(const vu_t& _v) : v(_v), result(0), bi(0), ei(0)
    {
        solve();
    }
    u_t get(u_t* pbi=0, u_t* pei=0) const
    {
        u_t dum;
        *(pbi ? : &dum) = bi;
        *(pei ? : &dum) = ei;
        return result;
    }
 private:
    class XY
    {
      public:
         XY(u_t _x=0, u_t _y=0) : x(_x), y(_y) {}
      u_t x, y;
    };
    typedef vector<XY> vxy_t;
    void solve();
    u_t top() const { return inc_stack.empty() ? 0 : inc_stack.back().y; }
    u_t pop(u_t ie);
    const vu_t &v;
    u_t result;
    u_t bi;
    u_t ei;
    vxy_t inc_stack;
};

void MaxBoundRect::solve()
{
    u_t sz = v.size();
    bi = 0;
    ei = 1;
    
    for (u_t i = 0; i < sz; ++i)
    {
        u_t x = i;
        u_t y = v[i];
        while (y < top())
        {
            x = pop(i);
        }
        if (top() < y)
        {
            inc_stack.push_back(XY(x, y));
        }
    }
    while (!inc_stack.empty())
    {
        pop(sz);
    }
}

u_t MaxBoundRect::pop(u_t ie)
{
    const XY& back = inc_stack.back();
    u_t dx = ie - back.x;
    u_t xback = back.x;
    u_t yback = back.y;
    u_t rect = dx*yback;
    if (result < rect)
    {
        result = rect;
        bi = xback;
        ei = ie;
    }
    inc_stack.pop_back();
    return xback;
}

u_t max_bounded_rect(const vu_t& v, u_t* pbi=0, u_t* pei=0)
{
    return MaxBoundRect(v).get(pbi, pei);
}

#include <cstdlib>

static int test_fast_naive(const vu_t &v)
{
    int rc = 0;
    u_t mr, bi, ei;
    vuu_t naive_be;
    mr = max_bounded_rect(v, &bi, &ei);
    u_t mr_naive;
    mr_naive = max_bounded_rect_naive(v, naive_be);
    if (mr != mr_naive ||
        (find(naive_be.begin(), naive_be.end(), uu_t(bi, ei)) ==
         naive_be.end()))
    {
        cerr <<
            "Naive: R="<<mr_naive << ", be: {";
        for (const uu_t& be: naive_be)
        {
            cerr << "[" << be.first << ", " << be.second << "), ";
        }
        cerr <<"}\n"
            "Fast:  R="<<mr << ", ["<<bi << ", "<<ei << ")\n";
        rc = 1;
    }
    return rc;
}

static int test_explicit(int argc, char** argv)
{
    vu_t v;
    for (int ai = 0; ai < argc; ++ai)
    {
        v.push_back(strtoul(argv[ai], 0, 0));
    }
    int rc = test_fast_naive(v);
    return rc;
}

static int test_rand(int argc, char** argv)
{
    u_t ai = 0;
    u_t n = strtoul(argv[ai++], 0, 0);
    u_t sz = strtoul(argv[ai++], 0, 0);
    u_t M = strtoul(argv[ai++], 0, 0);
    vu_t v(vu_t::size_type(sz), 0);
    int rc = 0;
    for (u_t ti = 0; (ti < n) && (rc == 0); ++ti)
    {
        for (u_t i = 0; i < sz; ++i)
        {
            v[i] = rand() % (M + 1);
        }
        rc = test_fast_naive(v);
        if (rc != 0)
        {
            cerr << "... explicit";
            for (u_t x: v)
            {
                cerr << ' ' << x;
            }
            cerr << '\n';
        }
    }
    return rc;
}

int main(int argc, char** argv)
{
    int rc = 0;
    string cmd = (argc > 1 ? argv[1] : "");
    if (cmd == string("explicit"))
    {
        rc = test_explicit(argc - 2, argv + 2);
    }
    else if (cmd == string("rand"))
    {
        rc = test_rand(argc - 2, argv + 2);
    }
    else
    {
        cerr << "Bad command: '" << cmd << "'\n";
        rc = 1;
    }
    return rc;
}
