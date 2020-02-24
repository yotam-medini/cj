// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <array>
#include <set>
#include <map>
#include <vector>
#include <utility>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<char> vc_t;
typedef vector<string> vs_t;
typedef set<u_t> setu_t;
typedef array<ull_t, 26> aull26_t;

static unsigned dbg_flags;

class Tail
{
 public:
    Tail(u_t _wi=0, const aull26_t& a=aull26_t()) : wi(_wi), az_count{a} {}
    u_t wi;
    aull26_t az_count;
};
bool operator<(const Tail& t0, const Tail& t1)
{
    return t0.az_count < t1.az_count;
}
typedef vector<Tail> vtail_t;

typedef map<char, vtail_t> c2tail_t;
typedef map<u_t, c2tail_t> u2_c2tail_t;

class Scrambled
{
 public:
    Scrambled(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    void generate_text();
    bool word_is_legal(const string& w) const;
    void build_index();
    void subsort(vc_t& internal, const string& s, u_t b, u_t e) const;
    u_t L, N;
    vs_t lang;
    char c1, c2;
    ull_t A, B, C, D;
    string text;
    u2_c2tail_t c_sz_lang[26];
    vector<aull26_t> az_count;
    u_t solution;
};

Scrambled::Scrambled(istream& fi) : solution(0)
{
    string s1, s2;
    fi >> L;
    lang.reserve(L);
    for (u_t li = 0; li < L; ++li)
    {
        string w;
        fi >> w;
        lang.push_back(w);
    }
    fi >> s1 >> s2;
    c1 = s1[0];
    c2 = s2[0];
    fi >> N >> A >> B >> C >> D;
}

void Scrambled::solve_naive()
{
    generate_text();
    for (const string& w: lang)
    {
        if (word_is_legal(w))
        {
            if (dbg_flags & 0x1) { cerr << w << '\n'; }
            ++solution;
        }
    }
}

void Scrambled::solve()
{
    generate_text();
    build_index();
    setu_t wis;
    for (u_t ti = 0; ti < N; ++ti)
    {
        if ((dbg_flags & 0x2) && ((ti & (ti - 1)) == 0)) {
          cerr << "ti="<<ti << ", N="<<N << '\n';
        }
        
        char c = text[ti];
        const u2_c2tail_t& sz_lang = c_sz_lang[c - 'a'];
        for (const u2_c2tail_t::value_type& sz_ctail: sz_lang)
        {
            u_t sz = sz_ctail.first;
            const c2tail_t& c2tail = sz_ctail.second;
            c2tail_t::const_iterator iter = (ti + sz <= N 
                ? c2tail.find(text[ti + sz - 1]) : c2tail.end());
            if (iter != c2tail.end())
            {
                aull26_t az_delta{};
                if (sz > 2)
                {
                    for (u_t azi = 0; azi < 26; ++azi)
                    {
                        az_delta[azi] =
                            az_count[ti + sz - 1][azi] - az_count[ti + 1][azi];
                    }
                }
                const vtail_t& vtail = iter->second;
                auto er = equal_range(vtail.begin(), vtail.end(),
                    Tail{0, az_delta});
                for (vtail_t::const_iterator i = er.first; i != er.second; ++i)
                {
                    const Tail& tail = *i;
                    wis.insert(wis.end(), tail.wi);
                }
            }
        }
    }
    if (dbg_flags & 0x1) {
      for (u_t wi: wis) { cerr << lang[wi] << '\n'; }
    }
    solution = wis.size();
}

void Scrambled::print_solution(ostream &fo) const
{
    fo << ' ' << solution;
}

void Scrambled::generate_text()
{
    // xi = ( A * xi-1 + B * xi-2 + C ) modulo D.
    // Si = char(97 + ( xi modulo 26 )) 
    text.reserve(N);
    text.push_back(c1);
    text.push_back(c2);
    ull_t x0 = c1, x1 = c2;
    for (u_t ci = 2; ci < N; ++ci)
    {
        ull_t xi = (A*x1 + B*x0 + C) % D;
        char c = 'a' + (xi % 26);
        text.push_back(c);
        x0 = x1;
        x1 = xi;
    }
}

bool Scrambled::word_is_legal(const string& w) const
{
    bool legal = false;
    const size_t sz = w.size();
    if (sz == 1)
    {
        legal = (text.find(w[0]) != string::npos);
    }
    else
    {
        for (u_t si = 0; (si + sz <= N) && !legal; ++si)
        {
            legal = (text[si] == w[0]) && (text[si + sz - 1] == w[sz - 1]);
            if (legal)
            {
                vc_t text_internal, w_internal;
                subsort(text_internal, text, si + 1, si + sz - 1);
                subsort(w_internal, w, 1, sz - 1);
                legal = (text_internal == w_internal);
            }
        }
    }
    return legal;
}

void Scrambled::build_index()
{
    for (u_t wi = 0; wi < L; ++wi)
    {
        const string& w = lang[wi];
        u_t sz = w.size();
        char c = w[0];
        char clast = w[sz - 1];
        u2_c2tail_t::iterator iter;
        {
            u2_c2tail_t& sz_c2tail = c_sz_lang[c - 'a'];
            auto er = sz_c2tail.equal_range(sz);
            iter = er.first;
            if (er.first == er.second)
            {
                u2_c2tail_t::value_type v{sz, c2tail_t()};
                iter = sz_c2tail.insert(iter, v);
            }
        }
        c2tail_t& c2tail = iter->second;
        c2tail_t::iterator iter2;
        {
            auto er = c2tail.equal_range(clast);
            iter2 = er.first;
            if (er.first == er.second)
            {
                c2tail_t::value_type v{clast, vtail_t()};
                iter2 = c2tail.insert(iter2, v);
            }
        }
        vtail_t& vtail = iter2->second;
        aull26_t w_az_count{};
        for (u_t ci = 1; ci < sz - 1; ++ci)
        {
            ++w_az_count[w[ci] - 'a'];
        }
        vtail.push_back(Tail(wi, w_az_count));
    }
    for (u_t az = 0; az < 26; ++az)
    {
        u2_c2tail_t& sz_c2tail = c_sz_lang[az];
        for (u2_c2tail_t::value_type& szct: sz_c2tail)
        {
            for (c2tail_t::value_type& c2tail: szct.second)
            {
                vtail_t& vtail = c2tail.second;
                sort(vtail.begin(), vtail.end());
            }
        }
    }
    az_count.reserve(N + 1);
    az_count.push_back(aull26_t{});
    for (u_t ti = 0; ti < N; ++ti)
    {
        aull26_t az{az_count.back()};
        ++az[text[ti] - 'a'];
        az_count.push_back(az);
    }
}

void Scrambled::subsort(vc_t& internal, const string& s, u_t b, u_t e) const
{
    internal.clear();
    internal.reserve(e - b);
    for (u_t si = b; si < e; ++si)
    {
        internal.push_back(s[si]);
    }
    sort(internal.begin(), internal.end());
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

    void (Scrambled::*psolve)() =
        (naive ? &Scrambled::solve_naive : &Scrambled::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Scrambled scrambled(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (scrambled.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        scrambled.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
