// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <vector>
#include <utility>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef long long ll_t;
typedef unsigned long long ull_t;
typedef vector<string> vs_t;
typedef vector<u_t> vu_t;
typedef vector<ull_t> vull_t;

typedef pair<char, u_t> cu_t;
typedef vector<cu_t> vcu_t;

static unsigned dbg_flags;

// short completions to palindromes
void palicomp(vs_t& palis, const string& s, u_t max_sz)
{
    u_t sz = s.size();
    palis.clear();
    if (max_sz > 2*sz)
    {
        max_sz = 2*sz;
    }
    for (u_t psz = sz; psz <= max_sz; ++psz)
    {
        u_t half = psz/2;
        u_t down = half - 1;
        u_t up = psz - half;
        bool can = true;
        for (; can && (up < sz); --down, ++up)
        {
            can = (s[down] == s[up]);
        }
        if (can)
        {
            string pali{s};
            while (pali.size() < psz)
            {
                pali.push_back(s[psz - pali.size() - 1]);
            }
            palis.push_back(pali);
        }
    }
    sort(palis.begin(), palis.end());
}

class PalindromeSequence
{
 public:
    PalindromeSequence(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    static const char* az;
    void reduce_block(ull_t& block, ull_t& lp, u_t& pblock) const;
    ull_t n_palindromes_below(u_t maxlen, const vu_t& suffix,
        const vu_t& below) const;
    u_t L;
    ull_t N, K;
    ull_t solution;
};
const char* PalindromeSequence::az = "abcdefghijklmnopqrstuvwxyz";

PalindromeSequence::PalindromeSequence(istream& fi) : solution(ull_t(-1))
{
    fi >> L >> N >> K;
}

void PalindromeSequence::solve_naive()
{
    vs_t lang;
    for (u_t n = 1; n <= 2; ++n)
    {
        for (u_t i = (N >= n ? 0 : L) ; i != L; ++i)
        {
            string s(string(size_t(n), 'a' + i));
            lang.push_back(s);
        }
    }
    for (u_t n = 3; n <= N; ++n)
    {
        for (u_t si = 0, se = lang.size(); si != se; ++si)
        {
            const string ss = lang[si]; // not ref, since lang grows
            if (ss.size() == n - 2) // not efficient, but naive
            {
                for (u_t azi = 0; azi != L; ++azi)
                {
                    string sc(string(size_t(1), az[azi]));
                    string s = sc + ss + sc;
                    lang.push_back(s);
                }
            }
        }
    }
    solution = 0;
    if (K - 1 < lang.size())
    {
        sort(lang.begin(), lang.end());
        if (dbg_flags & 0x1) {
          for (const string& s: lang) { cerr << s << '\n'; }
        }
        const string& target = lang[K - 1];
        solution = target.size();
    }
}

void PalindromeSequence::solve()
{
    static ull_t undef = ull_t(-1);
    if (K <= N)
    {
        solution = K;
    }
    else if (L == 1)
    {
        solution = 0;
    }
    else if (N == 1)
    {
        solution = (K <= L ? 1 : 0);
    }
    else // K, L >= 2
    {
        u_t p = 1, pblock = 1;
        ull_t lp = L;
        ull_t block = L;
        ull_t k = K - 1;
        ull_t Lk = L*k;
        while ((pblock < N) && (block <= Lk))
        {
            ++pblock;
            if (pblock % 2 == 1)
            {
                ++p;
                lp *=L;
            }
            block += lp;
        }
        if ((pblock == N) && (block < K))
        {
            solution = 0; // may return
        }
        vu_t prefix;
        ull_t s = 0;
        ull_t maxlen = N;
        while ((pblock < maxlen) && (solution == undef))
        {
            if (k < maxlen)
            {
                solution = s + k;
            }
            s += 2;
            k -= 2;
            maxlen -= 2;
            prefix.push_back(0);
        }
        while (pblock > maxlen)
        {
            reduce_block(block, lp, pblock);
        }
        u_t last_char = 0, last_back_char = L;
        while (solution == undef)
        {
            const ull_t cblock = block / L;
            const u_t curr_char = k / cblock;
            if ((last_back_char == L) || (last_char != curr_char))
            {
                last_back_char = last_char;
            }
            if (maxlen == 1)
            {
                solution = s + 1;
            }
            else if (false && (maxlen == 2))
            {
                const u_t lbc = last_back_char % L; // so L will be like 0
                k %= 2;
                u_t add = (lbc <= curr_char ? k + 1 : 2 - k);
                solution = s + add;
            }
            else
            {
                k %= cblock;
                // const ull_t ccblock = (cblock - 2) / L;
                vu_t suffix(prefix.rbegin(), prefix.rend());
                vu_t below;
                below.push_back(curr_char);
                below.insert(below.end(), suffix.begin(), suffix.end());
                ull_t pidx1 = n_palindromes_below(maxlen, suffix, below);
                below.insert(below.begin(), curr_char);
                ull_t pidx2 = n_palindromes_below(maxlen, suffix, below);
                const ull_t idx1 = pidx1 % cblock;
                const ull_t idx2 = pidx2 % cblock;
                if (k == idx1)
                {
                    solution = s + 1;
                }
                else if (k == idx2)
                {
                    solution = s + 2;
                }
                else
                {
                    s += 2;
                    maxlen -= 2;
                    u_t sub = u_t(idx1 < k) + u_t(idx2 < k);
                    k -= sub;
                    reduce_block(block, lp, pblock);
                    reduce_block(block, lp, pblock);
                    // last_char = curr_char;
                    prefix.push_back(curr_char);
                }
            }
        }
    }
}

void PalindromeSequence::reduce_block(ull_t& block, ull_t& lp, u_t& pblock)
    const
{
    block -= lp;
    if (pblock % 2 == 1)
    {
        lp /= L;
    }
    --pblock;
}

ull_t PalindromeSequence::n_palindromes_below(u_t maxlen, const vu_t& suffix,
    const vu_t& below) const
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

void PalindromeSequence::print_solution(ostream &fo) const
{
    fo << ' ' << solution;
}

int main(int argc, char ** argv)
{
    const string dash("-");

    bool naive = false;
    bool tellg = false;
    int rc = 0, ai;

    for (ai = 1; (rc == 0) && (ai < argc) && (argv[ai][0] == '-') &&
        argv[ai][1] != '\0'; ++ai)
    {
        const string opt(argv[ai]);
        if (opt == string("-naive"))
        {
            naive = true;
        }
        else if (opt == string("-debug"))
        {
            dbg_flags = strtoul(argv[++ai], 0, 0);
        }
        else if (opt == string("-tellg"))
        {
            tellg = true;
        }
        else
        {
            cerr << "Bad option: " << opt << "\n";
            return 1;
        }
    }

    int ai_in = ai;
    int ai_out = ai + 1;
    istream *pfi = (argc <= ai_in || (string(argv[ai_in]) == dash))
         ? &cin
         : new ifstream(argv[ai_in]);
    ostream *pfo = (argc <= ai_out || (string(argv[ai_out]) == dash))
         ? &cout
         : new ofstream(argv[ai_out]);

    if ((!pfi) || (!pfo) || pfi->fail() || pfo->fail())
    {
        cerr << "Open file error\n";
        exit(1);
    }

    string ignore;
    unsigned n_cases;
    *pfi >> n_cases;
    getline(*pfi, ignore);

    void (PalindromeSequence::*psolve)() =
        (naive ? &PalindromeSequence::solve_naive : &PalindromeSequence::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        PalindromeSequence palinseq(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (palinseq.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        palinseq.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
