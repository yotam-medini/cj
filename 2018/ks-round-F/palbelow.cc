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
    const char* space = "";
    for (u_t e: a)
    {
        os << space << e;
        space = " ";
    }
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

ull_t n_palindromes_below_naive(u_t L, u_t maxlen, const vu_t& suffix,
    const vu_t& below)
{
    const vvu_t& pals = get_palindromes(L, maxlen);
    vvu_t psfx(pals);
    for (vu_t& p: psfx)
    {
        p.insert(p.end(), suffix.begin(), suffix.end());
    }
    sort(psfx.begin(), psfx.end());
    vvu_t::const_iterator lb = lower_bound(psfx.begin(), psfx.end(), below);
    ull_t ret = lb - psfx.begin();
    return ret;
}

ull_t n_palindromes_below0(u_t L, u_t maxlen, const vu_t& suffix,
    const vu_t& below)
{   // below ==  [c]+suffix or [c,c]+suffix
    ull_t n = 0;
    const u_t below_sz = below.size();
    ull_t lp =1;
    for (u_t len = 1; len <= maxlen; ++len)
    {
        const bool even = (len % 2 == 0);
        lp *= (even ? 1 : L);
        const u_t half = (len + 1)/2;
        const u_t be = (below_sz < half ? below_sz : half);
        u_t bi = 0;
        ull_t m = lp/L;
        for ( ; bi < be; ++bi, m /= L)
        {
            n += below[bi] * m;
        }
        vu_t end_pal(below.begin(), below.begin() + be);
        for (int j = be - 1 - (even ? 0 : 1); j >= 0; --j)
        {
            end_pal.push_back(below[j]);
        }
        end_pal.insert(end_pal.end(), suffix.begin(), suffix.end());
        n += (end_pal < below ? 1 : 0);
    }
    return n;
}

static u_t get_cbelow(const vu_t& c_cc, const vu_t& prefix, u_t bi)
{
    const u_t c_cc_sz = c_cc.size();
    const u_t c_cc_pfx_sz = c_cc_sz + prefix.size();
    const vu_t::const_reverse_iterator suffix = prefix.rbegin();
    u_t ret = (bi < c_cc_sz ? c_cc[bi] :
        (bi < c_cc_pfx_sz ? *(suffix + (bi - c_cc_sz)) : 0));
    return ret;
}

ull_t n_palindromes_below(u_t L, ull_t maxlen, ull_t zprefix,
   const vu_t& prefix, const vu_t& c_cc)
{   // below ==  [c]+suffix or [c,c]+suffix
    ull_t n = 0;
    ull_t lp =1;
    const u_t below_sz = c_cc.size() + prefix.size() + zprefix;
    for (u_t len = 1; len <= maxlen; ++len)
    {
        const bool even = (len % 2 == 0);
        lp *= (even ? 1 : L);
        const u_t half = (len + 1)/2;
        const u_t be = (below_sz < half ? below_sz : half);
        ull_t m = lp/L;
        vu_t end_pal;
        for (u_t bi = 0; bi < be; ++bi, m /= L)
        {
            u_t cbelow = get_cbelow(c_cc, prefix, bi);
            n += cbelow * m;
            end_pal.push_back(cbelow);
        }
        if (half < below_sz)
        {
            for (u_t bi = be - (even ? 0 : 1); bi > 0;)
            {
                --bi;
                u_t cbelow = get_cbelow(c_cc, prefix, bi);
                end_pal.push_back(cbelow);
            } // now end_pal.size() == len
            vu_t below({c_cc});
            below.insert(below.end(), prefix.rbegin(), prefix.rend());
            end_pal.insert(end_pal.end(), prefix.rbegin(), prefix.rend());
            n += (end_pal < below ? 1 : 0);
        }
    }
    return n;
}

bool test_np_below(u_t L, u_t maxlen, const vu_t& suffix, const vu_t& below)
{
    ull_t n0 = n_palindromes_below0(L, maxlen, suffix, below);
    ull_t n = n0;
    ull_t zprefix = 0;
    vu_t prefix, c_cc;
    u_t sfxsz = suffix.size(), bsz = below.size();
    for (size_t nc = 1; nc <= 2; ++nc)
    {
        if ((bsz == sfxsz + nc) && 
            equal(suffix.begin(), suffix.end(), below.begin() + nc))
        {
            c_cc.insert(c_cc.end(), nc, below[0]);
            vu_t::const_reverse_iterator i = suffix.rbegin();
            for (; (i != suffix.rend()) && (*i == 0); ++zprefix, ++i) {}
            prefix.insert(prefix.end(), i, suffix.rend());
        }
    }
    if (!c_cc.empty())
    {
        n = n_palindromes_below(L, maxlen, zprefix, prefix, c_cc);
    }
    ull_t n_naive = n_palindromes_below_naive(L, maxlen, suffix, below);
    bool ok = (n == n_naive) && (n == n0);
    if (!ok)
    {
        cerr << "Failed with: " << L << ' ' << maxlen << ' ' <<
            vu2str(suffix) << " b " << vu2str(below) << '\n';
        cerr << "n=" <<n << ", n0=" << n0 << ", n_naive=" << n_naive << '\n';
    }
    else
    {
        cout << "#P(" << L << ", " << maxlen << ", [" << 
            vu2str(suffix) << "], [" << vu2str(below) << "] = " << n << '\n';
    }
    return ok;
}

int test_special(int argc, char **argv, int b)
{
    int ai = 0;
    u_t L = stoi(argv[++ai]);
    u_t maxlen = stoi(argv[++ai]);
    vu_t suffix, below;
    for (++ai; ai < b; ++ai)
    {
        suffix.push_back(stoi(argv[ai]));
    }
    for (++ai; ai < argc; ++ai)
    {
        below.push_back(stoi(argv[ai]));
    }
    bool ok = test_np_below(L, maxlen, suffix, below);
    return ok ? 0 : 1;
}

int test_ranges(int argc, char **argv)
{
    bool ok = true;
    int ai = 0;
    u_t Lmax = stoi(argv[++ai]);
    u_t maxlen_max = stoi(argv[++ai]);
    u_t suffix_max = stoi(argv[++ai]);
    for (u_t L = 2; (L <= Lmax) && ok; ++L)
    {
        for (u_t maxlen = 2; (maxlen <= maxlen_max) && ok; ++maxlen)
        {
            for (u_t sfx_sz = 0; (sfx_sz <= suffix_max) && ok; ++sfx_sz)
            {
                bool more_suffix = true;
                for (u_t sfx_num = 0; ok && more_suffix; ++sfx_num)
                {
                    vu_t suffix;
                    u_t d = sfx_num;
                    more_suffix = false;
                    while (suffix.size() < sfx_sz)
                    {
                        suffix.push_back(d % L);
                        d /= L;
                        more_suffix = more_suffix || (suffix.back() !=  L - 1);
                    }
                    // ok = test_np_below(L, maxlen, suffix, suffix);
                    for (u_t c = 0; ok && (c < L); ++c)
                    {
                        for (size_t pp = 1; ok && (pp <= 2); ++pp)
                        {
                            vu_t below(pp, c);
                            below.insert(below.end(), 
                                suffix.begin(), suffix.end());
                            ok = test_np_below(L, maxlen, suffix, below);
                        }
                    }
                }
            }    
        }
    }
    return ok ? 0 : 1;    
}

int main(int argc, char **argv)
{
    int rc = 0;
    int b = 0;
    const string sb("b");
    for (b = 1; (b < argc) && (string(argv[b]) != sb); ++b) {}
    if (b < argc)
    {
        rc = test_special(argc, argv, b);
    }
    else
    {
        rc = test_ranges(argc, argv);
    }
    return rc;
}
