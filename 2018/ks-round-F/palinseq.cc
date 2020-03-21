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
    void vs_to_char_count(vcu_t& vcu, const vs_t& vs, u_t ci) const;
    u_t L;
    ull_t N, K;
    u_t solution;
};
const char* PalindromeSequence::az = "abcdefghijklmnopqrstuvwxyz";

PalindromeSequence::PalindromeSequence(istream& fi) : solution(u_t(-1))
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
    vull_t b_sizes, b_sum_sizes;
    b_sizes.reserve(N + 1);
    b_sum_sizes.reserve(N + 1);
    b_sizes.push_back(0);
    b_sum_sizes.push_back(0);
    if (N >= 1)
    {
        b_sizes.push_back(L);
        b_sum_sizes.push_back(L);
    }
    if (N >= 2) {
        b_sizes.push_back(L);
        b_sum_sizes.push_back(L + L);
    }
    for (u_t n = 3; n <= N; ++n)
    {
        b_sizes.push_back(L * b_sizes[n - 2]);
        b_sum_sizes.push_back(b_sum_sizes.back() + b_sizes.back());
    }

    // We need to have an prefix string o the target and the index
    // of the first palindrom having that prefix.
    solution = 0;
    ull_t k = K - 1; // pending to advance
    ull_t n_all =  b_sum_sizes.back();
    if (k < n_all)
    {
        string s;
        ull_t cblock = (n_all / L);
        u_t nb0 = k / cblock;
        char c = az[nb0];
        s.push_back(c);
        k %= cblock;
        while (k > 0)
        {
            u_t sz = s.size();
            bool pali_already = false;
            vs_t short_palis;
            palicomp(short_palis, s, N);
            if ((!short_palis.empty()) && (short_palis[0].size() == sz))
            {
                pali_already = true;
                --k;
            }
            if (2*sz < N)
            {
                char cnext = 'a';
                u_t mid = N - 2*sz;
                ull_t pre_char_size = b_sum_sizes[mid] / L;
                vcu_t cnext_counts;
                vs_to_char_count(cnext_counts, short_palis, sz);
                for (ull_t cci = 0, cce = cnext_counts.size();
                    (cci != cce) && (s.size() == sz); ++cci)
                {
                    const cu_t& cc = cnext_counts[cci];
                    char cbound = cc.first;
                    u_t nb = k / pre_char_size;
                    u_t nb_low = u_t(cbound - cnext);
                    if (nb < nb_low)
                    {
                        k -= nb * pre_char_size;
                        cnext += nb;
                        s.push_back(cnext);
                    }
                    else
                    {
                        cnext = cbound;
                        ull_t kbound = (nb_low + 1) * pre_char_size + cc.second;
                        if (k < kbound)
                        {
                            s.push_back(cbound);
                            k -= nb_low * pre_char_size;
                        }
                        else
                        {
                            cnext = cbound + 1;
                            k -= kbound;
                        }
                    }
                }
                if (s.size() == sz)
                {
                    u_t nb = k / pre_char_size;
                    cnext += nb;
                    if (u_t(cnext - 'a') < L)
                    {
                        s.push_back(cnext);
                    }
                    else
                    {
                        cerr << "Error:"<<__LINE__<< " k="<<k <<
                            ", s="<<s << '\n';
                        exit(1);
                    }
                    k -= nb * pre_char_size;
                }
            } 
            else
            {
                k += (pali_already ? 1 : 0);
                if (k < short_palis.size())
                {
                    s = short_palis[k];
                    k = 0;
                }
                else
                {
                    cerr << "Error:"<<__LINE__<<", k="<<k <<", s="<<s<<'\n';
                    exit(1);
                }
            }
        }
        vs_t palis;
        palicomp(palis, s, N);
        if (2*s.size() < N)
        {
            string mirror;
            for (string::reverse_iterator si = s.rbegin(); si != s.rend(); ++si)
            {
                mirror.push_back(*si);
            }
            string a("a");
            for (string p = (s + a + mirror); p.size() <= N; 
                a.push_back('a'), p = s + a + mirror)
            {
                 palis.push_back(p);
            }
            sort(palis.begin(), palis.end());
        }
        s = palis[0];
        solution = s.size();
    }
}

void PalindromeSequence::vs_to_char_count(vcu_t& vcu, const vs_t& vs,
    u_t ci) const
{
    for (const string& s: vs)
    {
        if (ci < s.size())
        {
            const char c = s[ci];
            if (vcu.empty() || (vcu.back().first != c))
            {
                vcu.push_back(cu_t{c, 1});
            }
            else
            {
                ++vcu.back().second;
            }
        }
    }
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
