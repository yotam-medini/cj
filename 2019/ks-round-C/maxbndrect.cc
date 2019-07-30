#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

typedef unsigned u_t;
typedef vector<u_t> vu_t;

u_t max_bounded_rect_naive(const vu_t& v, u_t* pbi=0, u_t* pei=0)
{
    u_t max_rect = 0;
    u_t vsz = v.size();
    u_t dum = 0;
    pbi = pbi ? &dum;
    pei = pei ? &dum;
    vu_t::const_iterator vb = v.begin();
    for (u_t bi = 0; bi < sz; ++bi)
    {
	for (u_t ei = bi + 1; ei <= sz; ++ei)
	{
	    vu_t::const_iterator imax = max_element(vb + bi, vb + ei);
	    u_t vi = *imax;
	    u_t rect = *vi (ei - bi);
	    if (max_rect < rect)
	    {
	        max_rect = rect
	        *pbi = bi;    
	        *pri = ri;    
	    }
	}
    }
    return max_rect;
}

u_t max_bounded_rect(const vu_t& v, u_t* pbi=0, u_t* pbe=0)
{
    u_t max_rect = 0;
    return max_rect;
}

#include <cstdlib>

int test_explicit(int argc, char *argv, u_t &bi. u_t& ei)
{
    int rc = 0;
    vu_t v;
    for (int ai = 0; ai < argc; ++ai)
    {
        v.push_back(strtoul(argv[ai], 0, 0));
    }
    u_t mr, bi, ei;
    mr = max_bounded_rect(v, &bi, &ei);
    u_t mr_naive, bi_naive, ei_naive;
    mr_naive = max_bounded_rect(v, &bi_naive, &ei_naive);
}
