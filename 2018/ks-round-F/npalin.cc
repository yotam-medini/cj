#include <vector>
#include <algorithm>
#include <string>
#include <sstream>
#include <iostream>
using namespace std;

typedef unsigned u_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
typedef vector<vu_t> vvu_t;
typedef vector<vvu_t> vvvu_t;
typedef vector<ull_t> vull_t;

string vu2str(const vu_t& a)
{
    ostringstream os;
    os << '[';
    const char* space = "";
    for (u_t e: a)
    {
        os << space << e;
        space = " ";
    }
    os << "]";
    return os.str();
}

static const vvu_t& get_palindromes(u_t L, u_t maxlen)
{
    static u_t L_curr;
    static u_t maxlen_curr;
    static vvu_t pals;
    if ((L_curr != L) || (maxlen_curr != maxlen))
    {
        pals.clear();
        L_curr = L;
        maxlen_curr = maxlen;

        vvvu_t lens_pals;
        lens_pals.reserve(maxlen + 1);
        lens_pals.push_back(vvu_t()); // empty
        for (u_t len = 1; len <= maxlen; ++len)
        {
            vvu_t lenp;
            if (len <= 2)
            {
                for (u_t c = 0; c < L; ++c)
                {
                    vu_t p(size_t(len), c);
                    lenp.push_back(p);
                }
            }
            else
            {
                for (u_t c = 0; c < L; ++c)
                {
                    for (const vu_t& subp: lens_pals[len - 2])
                    {
                        vu_t p;
                        p.reserve(len);
                        p.push_back(c);
                        p.insert(p.end(), subp.begin(), subp.end());
                        p.push_back(c);
                        lenp.push_back(p);
                    }
                }
            }
            lens_pals.push_back(lenp);
        }
        for (const vvu_t& lenp: lens_pals)
        {
            pals.insert(pals.end(), lenp.begin(), lenp.end());
        }
        sort(pals.begin(), pals.end());
    }
    return pals;
}

ull_t n_palindroms_between_naive(u_t L, u_t maxlen, u_t b, const vu_t& suffix)
{
    // u_t max_whole_len = 2*suffix.size() + maxlen;
    const vvu_t& pals = get_palindromes(L, maxlen);
    vvu_t psfx(pals);
    for (vu_t& p: psfx)
    {
        p.insert(p.end(), suffix.begin(), suffix.end());
    }
    sort(psfx.begin(), psfx.end());
    vu_t pb, pbb;
    pb.push_back(b);
    pbb.push_back(b);
    pbb.push_back(b);
    pb.insert(pb.end(), suffix.begin(), suffix.end());
    pbb.insert(pbb.end(), suffix.begin(), suffix.end());

    vvu_t::const_iterator lb_pb = lower_bound(psfx.begin(), psfx.end(), pb);
    vvu_t::const_iterator lb_pbb = lower_bound(psfx.begin(), psfx.end(), pbb);
    ull_t ret = (lb_pb < lb_pbb ? lb_pbb - lb_pb : lb_pb - lb_pbb) - 1;
    return ret;
}

static void bsizes_fill(vull_t& bsizes, u_t L, u_t maxlen)
{
    static u_t bsizes_L = 0;
    if (bsizes_L != L)
    {
        bsizes.clear();
        bsizes_L = L;
    }
    if (bsizes.size() < maxlen + 1)
    {
        ull_t lp = L; u_t p = 1;
        if (bsizes.empty())
        {
            bsizes.push_back(0); // or 1? - empty is one single possibility
            bsizes.push_back(L);
            bsizes.push_back(L + L);
        }
        while (bsizes.size() < maxlen + 1)
        {
            u_t len = bsizes.size();
            u_t pnew = (len + 1)/2;
            if (p != pnew) // must be pnew==p+1
            {
                ++p;
                lp *= L;
            }
            ull_t sz = bsizes.back() + lp;
            bsizes.push_back(sz);
        }
    }
}

u_t n_prefix_complete_between(const vu_t& prefix, const vu_t& suffix,
     const vu_t& bdy_low, const vu_t& bdy_high, u_t maxlen)
{
    u_t ret = 0;
    for (u_t pi = 0; pi != 2; ++pi)
    {
        vu_t p(prefix);
        p.insert(p.end(), prefix.rbegin() + pi, prefix.rend());
        if (p.size() <= maxlen)
        {
            p.insert(p.end(), suffix.begin(), suffix.end());
            if ((bdy_low < p) && (p < bdy_high))
            {
                ++ret;
            }
        }
    }
    return ret;
}

u_t lh_get_nfree(u_t clow, u_t chigh)
{
    u_t n_free = (clow < chigh ? chigh - clow - 1 : 0);
    return n_free;
}

ull_t n_palindroms_between(u_t L, u_t maxlen, u_t b, const vu_t& suffix)
{
    ull_t ret = 0;
    if (maxlen > 2)
    {
        static vull_t bsizes;
        const u_t sfx_sz = suffix.size();
        const u_t inner_maxlen = maxlen - 2;
        const u_t half = (inner_maxlen + 1)/2;
        bsizes_fill(bsizes, L, maxlen - 2);
        u_t n_eqb = 0;
        for ( ; (n_eqb < sfx_sz) && (suffix[n_eqb] == b); ++n_eqb) {}
        vu_t bdy_low, bdy_high;
        bdy_low.push_back(b);
        bdy_high.push_back(b);
        bdy_high.push_back(b);
        bdy_low.insert(bdy_low.end(), suffix.begin(), suffix.end());
        bdy_high.insert(bdy_high.end(), suffix.begin(), suffix.end());
        bool bpbb = bdy_low < bdy_high ; // if "b..." precedes "bb..."
        if (!bpbb)
        {
            swap(bdy_low, bdy_high);
        }
        const u_t b_cdefault = (bpbb ? 0 : L - 1);
        const u_t bb_cdefault = (L - b_cdefault) - 1;
        vu_t pfx_low, pfx_high;
        pfx_low.push_back(b);
        pfx_high.push_back(b);
        bool low_eq_high = true;
        for (u_t i = 0; i < half; ++i)
        {
            u_t bc = i < sfx_sz ? suffix[i] : b_cdefault;
            u_t bbc = (i == 0 ? b : (i - 1 < sfx_sz ? suffix[i - 1]
                : bb_cdefault));
            const u_t clow = (bpbb ? bc : bbc);
            u_t chigh = (bpbb ? bbc : bc);
            pfx_low.push_back(clow);
            pfx_high.push_back(chigh);
            while ((clow < chigh) && (bdy_high < pfx_high))
            {
                pfx_high.back() = --chigh;
            }
            u_t n_free = 0;
            if (low_eq_high)
            {
                if (clow > chigh)
                {
                    cerr << "Error\n;";
                }
                n_free = lh_get_nfree(clow, chigh);
                ret += n_prefix_complete_between(
                    pfx_low, suffix, bdy_low, bdy_high, maxlen);
                if (clow != chigh)
                {
                    low_eq_high = false;
                    ret += n_prefix_complete_between(
                        pfx_high, suffix, bdy_low, bdy_high, maxlen);
                }
            }
            else
            {
                while ((chigh > 0) && (bdy_high <= pfx_high))
                {
                    pfx_high.back() = --chigh;
                }
                n_free = clow < L - 1 ? L - 1 - clow : 0;
                n_free += chigh;
                ret += n_prefix_complete_between(
                    pfx_low, suffix, bdy_low, bdy_high, maxlen);
                ret += n_prefix_complete_between(
                    pfx_high, suffix, bdy_low, bdy_high, maxlen);
            }
            if (n_free > 0)
            {
                const u_t def = 2*(i + 1);
                if (def <= inner_maxlen)
                {   //  prefix block xiferp
                    ret += n_free * (bsizes[inner_maxlen - def] + 1);
                }
                ret += n_free; // single prefix
            }
        }
    }
    return ret;
}

bool test_n_palindroms_between(u_t L, u_t maxlen, u_t b, const vu_t& suffix)
{
    ull_t n_naive = n_palindroms_between_naive(L, maxlen, b, suffix);
    ull_t n = n_palindroms_between(L, maxlen, b, suffix);
    bool ok = (n == n_naive);
    if (!ok)
    {
        cerr << "Failed with: special " << L << ' ' << maxlen << ' ' << b <<
            ' ' << vu2str(suffix) << '\n';
        cerr << "n="<<n << "  vs  n_naive="<<n_naive << '\n';
    }
    else
    {
        cout << "#P(" << L << ", " << maxlen << ", " << b << ", " <<
            vu2str(suffix) << ") = " << n << '\n';
    }
    return ok;
}

int main(int argc, char**argv)
{
    bool ok = true;
    int ai = 0;
    if (string(argv[1]) == string("special"))
    {
        ai = 1;
        u_t L = stoi(argv[++ai]);
        u_t maxlen = stoi(argv[++ai]);
        u_t b = stoi(argv[++ai]);
        vu_t suffix;
        while (++ai < argc)
        {
            suffix.push_back(stoi(argv[ai]));
        }
        ok = test_n_palindroms_between(L, maxlen, b, suffix);
    }
    else
    {
        u_t L = stoi(argv[++ai]);
        u_t max_maxlen = stoi(argv[++ai]);
        u_t max_suffix = stoi(argv[++ai]);
        for (u_t maxlen = 2; ok && (maxlen <= max_maxlen); ++maxlen)
        {
            for (u_t sfx_len = 0; ok && (sfx_len <= max_suffix); ++sfx_len)
            {
                bool more_suffix = true;
                for (u_t sfx_num = 0;  ok && more_suffix; ++sfx_num)
                {
                    vu_t sfx;
                    u_t d = sfx_num;
                    more_suffix = false;
                    while (sfx.size() < sfx_len)
                    {
                        sfx.push_back(d % L);
                        d /= L;
                        more_suffix = more_suffix || (sfx.back() !=  L - 1);
                    }
                    for (u_t b = 0; ok && (b < L); ++b)
                    {
                        ok = test_n_palindroms_between(L, maxlen, b, sfx);                           }
                }
            }
        }
    }
    return (ok ? 0 : 1);
}
