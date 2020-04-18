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
            bsizes.push_back(1); // empty is one single possibility
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

#if 0
u_t n_prefix_complete_between0(const vu_t& prefix, const vu_t& suffix,
    u_t maxlen, u_t b)
{
    u_t ret = 0;
    vu_t bsfx, bbsfx;
    bsfx.push_back(b);
    bbsfx.push_back(b);
    bbsfx.push_back(b);
    for (u_t c: suffix)
    {
        bsfx.push_back(c);
        bbsfx.push_back(c);
    }
    vu_t cand_even(prefix);
    vu_t cand_odd(prefix);
    cand_even.push_back(suffix.back());
    for (int i = prefix.size() - 2; i >= 0; --i)
    {
        cand_even.push_back(prefix[i]);
        cand_odd.push_back(prefix[i]);
    }
    if (bsfx > bbsfx)
    {
        swap(bsfx, bbsfx);
    }
    if ((cand_even.size() <= maxlen) && 
        (bsfx < cand_even) && (cand_even < bbsfx))
    {
        ++ret;
    }
    if ((bbsfx < cand_odd) && (cand_odd < bbsfx))
    {
        ++ret;
    }
    return ret;
}
#endif

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
            u_t bbc = (i == 0 ? b : (i > 1 ? suffix[i - 1] : bb_cdefault));
            const u_t clow = (bpbb ? bc : bbc);
            const u_t chigh = (bpbb ? bbc : bc);
            pfx_low.push_back(clow);
            pfx_high.push_back(chigh);
            u_t n_free = 0;
            if (low_eq_high)
            {
                if (clow > chigh)
                {
                    cerr << "Error\n;";
                }
                n_free = (clow < chigh ? chigh - clow - 1 : 0);
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
                n_free = (clow < chigh ? chigh - clow : clow - chigh);
                n_free = (n_free > 0 ? n_free - 1 : 0);
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
                    ret += n_free * bsizes[inner_maxlen - def];
                }
                ret += n_free; // single prefix
            }
        }
    }
    return ret;
}

#if 0
ull_t n_palindroms_between1(u_t L, u_t maxlen, u_t b, const vu_t& suffix)
{
    ull_t ret = 0;
    const u_t sfx_sz = suffix.size();
    if (sfx_sz > 0)
    {
        u_t ieq = 0;
        for (; (ieq < sfx_sz) && suffix[ieq] == b; ++ieq) {}
        vu_t pfxs[2];
        pfxs[0] = pfxs[1] = vu_t(ieq, b);
        pfxs[0].push_back(suffix[ieq]);
        pfxs[1].push_back(ieq == 0 ? b : suffix[ieq - 1]);
        if (ieq == sfx_sz)
        {
            pfxs[1].push_back(b);
        }
        u_t inner_maxlen = (inner_maxlen > 2 ? inner_maxlen - 2 : 0);
        bsizes_fill(bsizes, L, maxlen);
        u_t half = (inner_maxlen + 1)/2;
    }
    return ret;
}
#endif

#if 0
ull_t n_palindroms_between0(u_t L, u_t maxlen, u_t b, const vu_t& suffix)
{
    ull_t ret = 0;
    static vull_t bsizes;
    bsizes_fill(bsizes, L, maxlen);
    u_t inner_maxlen = (maxlen > 2 ? maxlen - 2 : 0);
    u_t half = (inner_maxlen + 1)/2;
    u_t suffix_size = suffix.size();
    vu_t prefix;
    bool done_up = (suffix_size > 0) && (suffix[0] > b);
    for (u_t i = 0; (i < half) && !done_up; ++i) // on inner
    {
        u_t sfx_char = (i < suffix_size ? suffix[i] : 0);
        u_t bbc = (i == 0 ? b : i - 1 < suffix_size ? suffix[i - 1] : 0);
        u_t n_free = (sfx_char < bbc ? bbc - sfx_char - 1 : 0);
        done_up = sfx_char > bbc;
        ull_t bsz = (2*i < inner_maxlen ? bsizes[inner_maxlen - 2*i] : 1);
        ret += n_free * bsz;
        prefix.push_back(sfx_char);
        if (i + 1 == half)
        {
            ret += n_prefix_complete_between(prefix, suffix, maxlen, b);
        }
    }
    bool done_down = (suffix_size > 0) && (suffix[0] <= b);
    for (u_t i = 0; (i < half) && !done_down; ++i) // on inner
    {
        u_t sfx_char = (i < suffix_size ? suffix[i] : 0);
        u_t bbc = (i == 0 ? b : i - 1 < suffix_size ? suffix[i - 1] : 0);
        u_t n_free = (sfx_char < bbc ? bbc - sfx_char - 1 : 0);
        done_down = sfx_char <= bbc;
        ull_t bsz = (2*i < inner_maxlen ? bsizes[inner_maxlen - 2*i] : 1);
        ret += n_free * bsz;
        prefix.push_back(sfx_char);
        if (i + 1 == half)
        {
            ret += n_prefix_complete_between(prefix, suffix, maxlen, b);
        }
    }
    return ret;
}
#endif 

#if 0
ull_t n_palindroms_between(u_t L, u_t maxlen, u_t b, const vu_t& suffix)
{
    ull_t ret = 0;
    static vull_t bsizes;
    bsizes_fill(bsizes, L, maxlen);
    ull_t freedom = (maxlen > 4 ? bsizes[maxlen - 2] : 
        (maxlen > 2 ? 1 : 0));
    u_t n_free = 0;
    u_t suffix_size = suffix.size();
    if (suffix_size == 0)
    {
        n_free = b;
    }
    else
    {
        u_t s0 = suffix[0];
        u_t delta = s0 < b ? b - s0 : s0 - b;
        n_free = delta > 0 ? delta - 1 : 0;
        u_t half = maxlen/2;
        ull_t add = 0;
        if (s0 < b)
        {
            u_t si = 0; // suffix index
            while ((si + 1 <= half) && (si + 1 < suffix_size) && 
                suffix[si + 1] == suffix[si])
            {
                ++si;
            }
            if (si + 1 <= half)
            {
                if ((si + 1 == suffix_size) || (suffix[si + 1] == suffix[si]))
                {
                    add += bsizes[half - (si + 1)];
                }
            }
        }
        ret += add;
    }
    ret += n_free * freedom;
    return ret;
}
#endif

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

bool test_n_palindroms_between(u_t L, u_t maxlen, u_t b, const vu_t& suffix)
{
    ull_t n_naive = n_palindroms_between_naive(L, maxlen, b, suffix);
    ull_t n = n_palindroms_between(L, maxlen, b, suffix);
    bool ok = (n == n_naive);
    if (!ok)
    {
        cerr << "Failed with: speical " << L << ' ' << maxlen << ' ' << b <<
            ' ' << vu2str(suffix) << '\n';
        cerr << "n="<<n << "  vs  n_naive="<<n_naive << '\n';
    }
    cout << "#P(" << L << ", " << maxlen << ", " << b << ", " <<
        vu2str(suffix) << ") = " << n << '\n';
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
