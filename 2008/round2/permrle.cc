// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <fstream>
#include <string>
// #include <set>
// #include <map>
#include <algorithm>
#include <vector>
#include <utility>

#include <cstdlib>
#include <cstring>

using namespace std;

typedef unsigned long ul_t;
typedef vector<int> vi_t;
typedef vector<vi_t> vvi_t;
typedef vector<unsigned> vu_t;
typedef vector<vu_t> vvu_t;

static unsigned dbg_flags;

#if 0
static void ov(ostream &fo, const vu_t &v)
{
    fo << "[";
    const char *del = "";
    for (auto i = v.begin(), e = v.end(); i != e; ++i)
    {
        fo << del << *i;
        del = ", ";
    }
    fo << "]\n";
}
#endif

static void iota(vu_t& v, unsigned n, unsigned val=0)
{
    v.clear();
    v.reserve(n);
    for (unsigned k = 0; k < n; ++k, val++)
    {
        v.push_back(val);
    }
}

void permutation_first(vu_t &p, unsigned n)
{
    iota(p, n, 0);
}

bool permutation_next(vu_t &p)
{
    unsigned n = p.size();

    // find the last 'in order'
    int j = n - 1 - 1;
    while ((j >= 0) && (p[j] > p[j + 1]))
    {
        --j;
    }
    bool ret = j >= 0;
    if (ret)
    {
        // Find last > p[j]. Must find since p[j] < p[j+1]
        unsigned l;
        for (l = n - 1; p[j] >= p[l]; --l);
        swap(p[j], p[l]);
        reverse(p.begin() + j + 1, p.end());
    }
    return ret;
}

static void mini(unsigned &v, unsigned x) { if (v > x) { v = x; } }

class PermRLE
{
 public:
    PermRLE(istream& fi);
    ~PermRLE()
    {
        delete [] cs;
        delete [] cs_alt;
    }
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    unsigned compress_size(const char *ccs, unsigned limit) const;
    void compute_graph();
    unsigned solve_last();
    unsigned solve_last_dp(ul_t A, unsigned x);
    unsigned xy_length(unsigned x, unsigned y) const
    {
        unsigned ret = (x == last ? next_edge_price[x][y] : edge_price[x][y]);
        return ret;
    }
    unsigned k;
    string s;
    unsigned slen;;
    unsigned m;
    char *cs;
    char *cs_alt;
    vvu_t edge_price;
    vvu_t next_edge_price;
    vvi_t memo;
    unsigned last;
    unsigned solution;
};

PermRLE::PermRLE(istream& fi) : k(0), slen(0), cs(0), cs_alt(0), solution(0)
{
    fi >> k;
    fi >> s;
    slen = s.size();
    m = slen / k;
    cs = new char[slen + 1];
    cs_alt = new char[slen + 1];
    strcpy(cs, s.c_str());
    strcpy(cs_alt, s.c_str());
}

void PermRLE::solve_naive()
{
    const unsigned slen = s.size();
    vu_t perm;
    permutation_first(perm, k);
    unsigned best = compress_size(cs_alt, slen);
    while (permutation_next(perm))
    {
        for (unsigned ci = 0; ci < slen; )
        {
            unsigned ci0 = ci;
            for (unsigned pi = 0; pi < k; ++pi, ++ci)
            {
                cs_alt[ci0 + perm[pi]] = cs[ci];
            }
        }
        unsigned csize = compress_size(cs_alt, best);
        mini(best, csize);
        if ((dbg_flags & 0x1) && (best == csize)) {
            cerr << "best="<<best << ", cs_alt="<<cs_alt << "\n";
        }
    }
    solution = best;
}

void PermRLE::solve()
{
    solution = compress_size(cs, slen);
    if (k > 1)
    {
        compute_graph();
        for (last = 0; last < k; ++last)
        {
            unsigned ksol = solve_last();
            mini(solution, ksol);
        }
        ++solution; // #(changes) + 1
    }
}

void PermRLE::print_solution(ostream &fo) const
{
    fo << " " << solution;
}

unsigned PermRLE::compress_size(const char *ccs, unsigned limit) const
{
    char clast = ' ';
    unsigned ret = 0;
    char c;
    for (const char *pc = ccs; (c = *pc) && (ret < limit); ++pc)
    {
        if (clast != c)
        {
            ++ret;
            clast = c;
        }
    }
    return ret;
}

void PermRLE::compute_graph()
{
    edge_price.reserve(k);
    next_edge_price.reserve(k);
    for (unsigned x = 0; x < k; ++x)
    {
        vu_t x_edge(vu_t::size_type(k), 0);
        vu_t x_next_edge(vu_t::size_type(k), 0);
        for (unsigned y = 0; y < k; ++y)
        {
            unsigned price = 0, next_price = 0;
            for (unsigned si0 = (x != y ? 0 : slen); si0 < slen; si0 += k)
            {
                if (s[si0 + x] != s[si0 + y])
                {
                    ++price;
                }
                unsigned ysi = si0 + y + k;
                if ((ysi < slen) && (s[si0 + x] != s[ysi]))
                {
                    ++next_price;
                }
            }
            x_edge[y] = price;
            x_next_edge[y] = next_price;
        }
        edge_price.push_back(x_edge);
        next_edge_price.push_back(x_next_edge);
    }
}

unsigned PermRLE::solve_last()
{
    vi_t::size_type two_pwr_k = 1 << k;
    memo = vvi_t(vvi_t::size_type(k), vi_t(two_pwr_k, -1));
    unsigned ret = solve_last_dp(two_pwr_k - 1, 0);
    return ret;
}

static unsigned bitlog(ul_t n) // assignment n > 0
{
    ul_t bit = 0;
    while (n)
    {
        ++bit;
        n >>= 1;
    }
    return bit - 1;
}

unsigned PermRLE::solve_last_dp(ul_t A, unsigned x)
{
    unsigned ret = 0;
    int cached = memo[x][A];
    if (cached >= 0)
    {
        ret = cached;
    }
    else
    {
        if ((A & (A - 1)) == 0) // single bir
        {
            if (A == (1u << x))
            {
                ret = xy_length(x, 0);
            }
            else
            {
                unsigned y = bitlog(A);
                ret = xy_length(y, 0) + xy_length(x, y);
            }
        }
        else
        {
            ret = slen;
            for (unsigned y = 0; y < k; ++y)
            {
                unsigned ybit = (1u << y);
                if (A & (ybit))
                {
                    unsigned ydp = solve_last_dp(A ^ ybit, y) + xy_length(x, y);
                    mini(ret, ydp);
                }
            }
        }
        memo[x][A] = ret;
    }
    return ret;
}

int main(int argc, char ** argv)
{
    const string dash("-");

    bool naive = false;
    bool tellg = false;
    int rc = 0, ai;

    for (ai = 1; (rc == 0) && (argv[ai][0] == '-') && argv[ai][1] != '\0'; ++ai)
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

    unsigned n_cases;
    *pfi >> n_cases;

    void (PermRLE::*psolve)() =
        (naive ? &PermRLE::solve_naive : &PermRLE::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        PermRLE permrle(*pfi);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << ", " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (permrle.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        permrle.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
