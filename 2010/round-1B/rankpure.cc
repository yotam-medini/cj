// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <algorithm>
#include <iterator>
#include <vector>
#include <utility>
#include <numeric>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
typedef pair<u_t, u_t> uu_t;
typedef map<uu_t, u_t> uu2u_t;

static unsigned dbg_flags;

static u_t gcd(u_t m, u_t n)
{
   while (n)
   {
      u_t t = n;
      n = m % n;
      m = t;
   }
   return m;
}

static u_t choose(u_t n, u_t k)
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
    u_t r = 1;
    for (vu_t::const_iterator i = high.begin(), e = high.end(); (i != e); ++i)
    {
        u_t h = *i;
        r = (r*h) % 100003;
    }
    return r;
}

static u_t choose_cache(u_t n, u_t k)
{
    static uu2u_t memo;

    u_t c = 0;
    uu_t key(n, k);
    auto er = memo.equal_range(key);
    if (er.first == er.second)
    {
        c = choose(n, k);
        uu2u_t::value_type v(key, c);
        memo.insert(er.first, v);
    }
    else
    {
        c = (*(er.first)).second;
    }
    return c;
}

static uu2u_t posnum_memo;

class RankPure
{
 public:
    RankPure(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    void bits2seq(vu_t &s, u_t bits);
    bool is_pure(const vu_t &s);
    u_t solve_posnum(u_t pos, u_t num);
    u_t n;
    u_t solution;
};

RankPure::RankPure(istream& fi) : solution(0)
{
    fi >> n;
}

void RankPure::bits2seq(vu_t &s, u_t bits)
{
    s.clear();
    s.push_back(0); // dummy - so we can count from 1
    for (u_t bit = 1; bit <= n; ++bit)
    {
        if ((bits & (1u << bit)) != 0)
        {
            s.push_back(bit);
        }
    }
}

bool RankPure::is_pure(const vu_t &s)
{
    bool pure = true;
    u_t v = n;
    u_t si = s.size();
    auto sb = s.begin();
    auto se = s.end();
    while (pure && (si > 1))
    {
        auto w = find(sb, se, v);
        if (w == se)
        {
            pure = false;
        }
        else
        {
            u_t si_next = w - sb;
            pure = (si_next < si);
            si = si_next;
            v = si;
        }
    }
    return pure;
}

void RankPure::solve_naive()
{
    u_t n_pure = 0;
    u_t bit_n = (1u << n);
    vu_t s;
    for (u_t bits = 0; bits < bit_n; bits += 2)
    {
        bits2seq(s, bits | bit_n);
        if (is_pure(s))
        {
            // auto eiter = ostream_iterator<u_t>(std::cerr, " ");
            // copy(s.begin() + 1, s.end(), eiter); cerr << '\n';
            ++n_pure;
        }
    }
    solution = n_pure % 100003;
}

u_t RankPure::solve_posnum(u_t pos, u_t num)
{
   unsigned ns = 0;
   uu_t key(pos, num);
   auto er = posnum_memo.equal_range(key);
   if (er.first != er.second)
   {
       ns = (*(er.first)).second;
   }
   else
   {
       if ((pos == 1) || (pos + 1 == n))
       {
           ns = 1;
       }
       else
       {
           u_t jump = min(num - pos, pos - 1);
           for (u_t subpos = pos - jump; subpos < pos; ++subpos)
           {
               ull_t sub_ns = solve_posnum(subpos, pos);
               ull_t c = choose_cache(num - pos - 1, pos - subpos - 1);
               ull_t csub_ns = (c*sub_ns) % 100003;
               ns = (ns + csub_ns) % 100003;
           }
       }
       uu2u_t::value_type v(key, ns);
       posnum_memo.insert(er.first, v);
   }
   return ns;
}

void RankPure::solve()
{
    u_t ns = 0;
    for (u_t pos = 1; pos < n; ++pos)
    {
        u_t nsp = solve_posnum(pos, n);
        ns = (ns + nsp) % 100003;
    }
    solution = ns;
}

void RankPure::print_solution(ostream &fo) const
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
    u_t n_cases;
    *pfi >> n_cases;
    getline(*pfi, ignore);

    void (RankPure::*psolve)() =
        (naive ? &RankPure::solve_naive : &RankPure::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (u_t ci = 0; ci < n_cases; ci++)
    {
        RankPure rankPure(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (rankPure.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        rankPure.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
