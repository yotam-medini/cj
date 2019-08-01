#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

typedef unsigned u_t;
typedef vector<u_t> vu_t;

u_t max_bounded_rect_naive(const vu_t& v, u_t* pbi=0, u_t* pei=0)
{
    u_t max_rect = 0;
    u_t sz = v.size();
    u_t dum = 0;
    pbi = pbi ? : &dum;
    pei = pei ? : &dum;
    vu_t::const_iterator vb = v.begin();
    for (u_t bi = 0; bi < sz; ++bi)
    {
	for (u_t ei = bi + 1; ei <= sz; ++ei)
	{
	    vu_t::const_iterator imax = max_element(vb + bi, vb + ei);
	    u_t vi = *imax;
	    u_t rect = vi * (ei - bi);
	    if (max_rect < rect)
	    {
	        max_rect = rect;
	        *pbi = bi;    
	        *pei = ei;    
	    }
	}
    }
    return max_rect;
}

u_t max_bounded_rect(const vu_t& v, u_t* pbi=0, u_t* pei=0)
{
    typedef vector<vu_t> vvu_t;
    u_t ret = 0;
    u_t dum = 0;
    pbi = pbi ? : &dum;
    pei = pei ? : &dum;
    u_t sz = v.size();
    vu_t incidx;
    vvu_t wake(vvu_t::size_type(sz ? : 1), vu_t());
    wake[0].push_back(0);
    for (u_t i = 1; i < sz; ++i)
    {
        u_t y = v[i];
        if (incidx.empty() || (y > v[incidx.back()]))
        {
            incidx.push_back(i);
        }
        while (v[incidx.back()] > y)
        {
            incidx.pop_back();
        }
    }
    return ret;
}

#if 0
u_t max_bounded_rect(const vu_t& v, u_t* pbi=0, u_t* pei=0)
{
    u_t dum = 0;
    pbi = pbi ? : &dum;
    pei = pei ? : &dum;
    u_t sz = v.size();
    u_t preval = (sz > 0 ? v[0] : 0);
    u_t max_rect = preval, curr_height = preval;
    vu_t incidx;
    incidx.push_back(0);
    u_t li = 0;
    for (u_t i = 1; i < sz; ++i)
    {
        if (v[i] >= v[i - 1])
        {
            incidx.push_back(i);
            u_t rect = (i - li + 1)*curr_height;
            if (max_rect < rect)
            {
                max_rect = rect;
                *pbi = li;
                *pei = i + 1;
            }
        }
        else
        {
            ; // pop
        }
        if (v[i] > curr_height)
        {
           ;
        }
    } 
    return max_rect;
}
#endif

#include <cstdlib>

static int test_fast_naive(const vu_t &v)
{
    int rc = 0;
    u_t mr, bi, ei;
    mr = max_bounded_rect(v, &bi, &ei);
    u_t mr_naive, bi_naive, ei_naive;
    mr_naive = max_bounded_rect_naive(v, &bi_naive, &ei_naive);
    if (mr != mr_naive || bi != bi_naive || ei != ei_naive)
    {
        cerr << 
            "Naive: R="<<mr_naive << ", ["<<bi_naive << ", "<<ei_naive << ")\n"
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
