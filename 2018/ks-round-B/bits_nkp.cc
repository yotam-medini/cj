#include <iostream>
#include <bitset>
#include <algorithm>
#include <vector>
#include <utility>
#include <map>
#include <set>
#include <string>
#include <numeric>

using namespace std;

typedef unsigned u_t;
typedef unsigned long long ull_t;
typedef pair<u_t, u_t> uu_t;
typedef vector<u_t> vu_t;
typedef vector<bool> vb_t;
typedef std::multiset<unsigned> msuints_t;

string vb2str(const vb_t& vb)
{
    string ret;
    for (vb_t::const_reverse_iterator iter = vb.rbegin(); iter != vb.rend();
        ++iter)
    {
        bool zo = *iter;
        ret.push_back("01"[int(zo)]);
    }
    return ret;
}

string u2str(ull_t x)
{
    string s = "";
    while (x > 0)
    {
        s.push_back("01"[x % 2]);
        x /= 2;
    }
    if (s.empty()) { s = string("0"); }
    string rs(s.rbegin(), s.rend());
    return rs;
}

u_t gcd(u_t m, u_t n)
{
   while (n)
   {
      u_t t = n;
      n = m % n;
      m = t;
   }
   return m;
}

u_t choose(u_t n, u_t k)
{
    if (k > n/2)
    {
        k = n - k;
    }
    vu_t high, low;
    high = vu_t(vu_t::size_type(k), 0);
    iota(high.begin(), high.end(), n - k + 1);
    u_t d = 2;
    while (d <= k)
    {
        u_t dd = d;
        for (vu_t::iterator i = high.begin(), e = high.end();
            (dd > 1) && (i != e); ++i)
        {
            u_t g = gcd(dd, *i);
            if (g > 1)
            {
                *i /= g;
                dd /= g;
            }
        }
        ++d;
    }
    u_t r =
        accumulate(high.begin(), high.end(), 1, multiplies<u_t>());
    return r;
}

u_t choose_cache(u_t n, u_t k)
{
    typedef map<uu_t, u_t> uu2u_t;
    static uu2u_t nk2c;

    u_t ret = 0;
    uu2u_t::key_type nk(n, k);
    pair<uu2u_t::iterator, uu2u_t::iterator> w = nk2c.equal_range(nk);
    if (w.first == w.second)
    {
        ret = choose(n, k);
        uu2u_t::value_type v(nk, ret);
        nk2c.insert(w.first, v);
    }
    else
    {
        ret = (*w.first).second;
    }
    return ret;
}

ull_t bits_nkp(u_t n, u_t k, ull_t p) // assuming: n>=k &  0<p<= choose(n,k)
{
    ull_t ret = 0;
    if (k > 0)
    {
        if (k == n)
	{
	    // assuming p = 1
	    ret = (1ull << n) - 1;
	}
	else // n > k > 0
	{
            ull_t count = 0;
	    for (u_t nsub = k - 1; (nsub <= n) && (count < p); ++nsub)
	    {
	        ull_t sub_choose = choose_cache(nsub, k - 1);
		if (p <= count + sub_choose)
		{
                    ull_t subret = bits_nkp(nsub, k - 1, p - count);
                    ret = (1ull << nsub) | subret;
                    count = p;
		}
		else
		{
                    count += sub_choose;
		}
	    }
	}
    }
    return ret;
}

// assuming: n>=k &  0<p<= choose(n,k)
void bits_nkp(vb_t& res, u_t n, u_t k, ull_t p)
{
    if (k > 0)
    {
        if (k == n)
	{
	    // assuming p = 1
            res = vb_t(size_t(n), true);
	}
	else // n > k > 0
	{
            ull_t count = 0;
	    for (u_t nsub = k - 1; (nsub <= n) && (count < p); ++nsub)
	    {
	        ull_t sub_choose = choose_cache(nsub, k - 1);
		if (p <= count + sub_choose)
		{
                    res.clear();
                    bits_nkp(res, nsub, k - 1, p - count);
                    size_t nz = nsub - res.size();
                    res.insert(res.end(), nz, false);
                    res.push_back(true);
                    count = p;
		}
		else
		{
                    count += sub_choose;
		}
	    }
	}
    }
}


ull_t bits_nkp_naive(u_t n, u_t k, ull_t p)
{
    ull_t ret = 0;
    ull_t found = 0;
    while (found < p)
    {
        u_t nbits = 0;
        for (ull_t shifted = ret; shifted != 0; shifted >>= 1)
        {
            nbits += (shifted & 0x1);
        }
        found += (nbits == k ? 1 : 0);
	++ret;
    }
    --ret;
    return ret;
}

bool test(u_t n, u_t k, ull_t p, bool speical=false)
{
    bool ok = true;
    ull_t r = bits_nkp(n, k, p);
    ull_t r_naive = bits_nkp_naive(n, k, p);
    vb_t res;
    if (r != r_naive)
    {
        cerr << "r=" << u2str(r) << " != r_naive=" << u2str(r_naive) << '\n';
        cerr << "Failed: speical " << n << ' ' << k << ' ' << p << '\n';
        ok = false;
    }
    if (ok)
    {
        bits_nkp(res, n, k, p);
        const string sres = vb2str(res);
        const string sr = u2str(r);
        if (sres != sr)
        {
            cerr << "sres=" << sres << " != sr=" << sr << '\n';
            cerr << "Failed: speical " << n << ' ' << k << ' ' << p << '\n';
            ok = false;
        }
    }
    if (ok && speical)
    {
        cout << "n="<<n << ", k="<<k << ", p="<<p <<
             " ==> " << u2str(r) << '\n';
    }
    return ok;
}

bool test_rand(u_t ntests,
    u_t nmin, u_t nmax, u_t kmin, u_t kmax, ull_t pmin, ull_t pmax)
{
    bool ok = true;
    for (u_t ti = 0; ok && (ti < ntests); ++ti)
    {
        u_t n = nmin + (nmin < nmax ? rand() % (nmax - nmin) : 0);
        kmax = min(kmax, n);
        u_t k = kmin + (kmin < kmax ? rand() % (kmax - kmin) : 0);
        pmax = min(pmax, ull_t(choose_cache(n, k)));
        ull_t p = pmin + (pmin < pmax ? rand() % (pmax - pmin) : 0);
	ok = test(n, k, p);
    }
    return ok;
}

int main(int argc, char **argv)
{
    bool ok = true;
    string cmd(argv[1]);
    if ((cmd == string("simple")) || (cmd == string("ssimple")))
    {
        int ai = 1;
	u_t n = stoi(argv[++ai]);
	u_t k = stoi(argv[++ai]);
	ull_t p = stol(argv[++ai]);
        string s;
        if (cmd == string("simple"))
        {
            ull_t r = bits_nkp(n, k, p);
            s = u2str(r);
        }
        else
        {
            vb_t res;
            bits_nkp(res, n, k, p);
            s = vb2str(res);
        }
        cout << "n="<<n << ", k="<<k << ", p="<<p <<
             " ==> " << s << '\n';
    }
    else if (cmd == string("special"))
    {
        int ai = 1;
	u_t n = stoi(argv[++ai]);
	u_t k = stoi(argv[++ai]);
	ull_t p = stol(argv[++ai]);
	ok = test(n, k, p, true);
    }
    else
    {
        int ai = 0;
        u_t ntests = stoi(argv[++ai]);
        u_t nmin = stoi(argv[++ai]);
        u_t nmax = stoi(argv[++ai]);
        u_t kmin = stoi(argv[++ai]);
        u_t kmax = stoi(argv[++ai]);
        ull_t pmin = stoi(argv[++ai]);
        ull_t pmax = stoi(argv[++ai]);
	ok = test_rand(ntests, nmin, nmax, kmin, kmax, pmin, pmax);
    }
    return ok ? 0 : 1;
}
