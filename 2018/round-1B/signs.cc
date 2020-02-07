// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <vector>
#include <utility>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;

typedef multiset<int> mseti_t;
typedef set<int> seti_t;

static unsigned dbg_flags;

class Sign
{
 public:
    Sign(int vD=0, int vA=0, int vB=0) : D(vD), A(vA), B(vB) {}
    int m() const { return D + A; }
    int n() const { return D - B; }
    int D, A, B;
};

typedef vector<Sign> vsign_t;

class Road
{
 public:
    Road(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    bool mn_good(int m, int n, u_t b, u_t e) const;
    ul_t s;
    vsign_t signs;
    u_t sol_len;
    u_t sol_n;
};

Road::Road(istream& fi) : sol_len(0), sol_n(0)
{
    fi >> s;
    signs.reserve(s);
    for (u_t i = 0; i < s; ++i)
    {
        int d, a, b;
        fi >> d >> a >> b;
        signs.push_back(Sign(d, a, b));
    }
}

static u_t major_get(int* major, const seti_t &s, const mseti_t &ms)
{
    u_t ret = 0;
    for (auto x: s)
    {
        auto sz = ms.count(x);
        if (2*sz >= ms.size())
        {
            major[ret++] = x;
        }
    }
    return ret;
}

bool Road::mn_good(int m, int n, u_t b, u_t e) const
{
    bool agree = true;
    for (u_t i = b; (i != e) && agree; ++i)
    {
        const Sign &sign = signs[i];
        int sm = sign.m();
        int sn = sign.n();
        agree = (sm == m) || (sn == n);
    }
    return agree;
}

void Road::solve_naive()
{
    for (u_t sz = s; (sz > 0) && (sol_n == 0); --sz)
    {
        if (dbg_flags & 0x1) { cerr << "sz="<<sz << "\n"; }
        for (u_t b = 0, e = b + sz; e <= s; ++b, ++e)
        {
            seti_t mset, nset;
            mseti_t m_mset, m_nset;
            for (u_t i = b; i != e; ++i)
            {
                const Sign &sign = signs[i];
                mset.insert(mset.end(), sign.m());
                m_mset.insert(m_mset.end(), sign.m());
                nset.insert(nset.end(), sign.n());
                m_nset.insert(m_nset.end(), sign.n());
            }
            int m_major[2], n_major[2];
            u_t m_major_sz = major_get(m_major, mset, m_mset);
            u_t n_major_sz = major_get(n_major, nset, m_nset);

            bool found = false;
            for (u_t mi = 0; (mi < m_major_sz) && !found; ++mi)
            {
                int m = m_major[mi];
                for (seti_t::const_iterator ni = nset.begin(), ne = nset.end();
                    (ni != ne) && !found; ++ni)
                {
                    int n = *ni;
                    found = mn_good(m, n, b, e);
                    if (found)
                    {
                        ++sol_n;
                        sol_len = sz;
                    }
                }
            }
            for (u_t ni = 0; (ni < n_major_sz) && !found; ++ni)
            {
                int n = n_major[ni];
                for (seti_t::const_iterator mi = mset.begin(), me = mset.end();
                    (mi != me) && !found; ++mi)
                {
                    int m = *mi;
                    found = mn_good(m, n, b, e);
                    if (found)
                    {
                        ++sol_n;
                        sol_len = sz;
                    }
                }
            }
            if ((dbg_flags & 0x2) && found) { cerr<<"b="<<b<<", e="<<e<<'\n'; }
        }
    }
}

class Keep
{
 public:
   Keep(int v=0, int a=0, u_t r=1, u_t l=1) : val(v), alt(a), run(r), len(l) {}
   int val;
   int alt;
   u_t run;
   u_t len;
};

void Road::solve()
{
    // solve_naive();
    sol_len = 1;
    sol_n = s;
    const Sign& sign0 = signs[0];
    if (dbg_flags & 0x1) {cerr << "[0] m="<<sign0.m()<<", n="<<sign0.n()<<'\n';}
    Keep mkeep(sign0.m(), sign0.n());
    Keep nkeep(sign0.n(), sign0.m());
    for (ul_t si = 1; si < s; ++si)
    {
        const Sign& sign = signs[si];
        int m = sign.m();
        int n = sign.n();
        if (dbg_flags & 0x1) { cerr << "["<<si<<"] m="<<m<<", n="<<n<<'\n'; }
        if ((mkeep.val == m) && (nkeep.val == n))
        {
            ++mkeep.run;
            ++mkeep.len;
            ++nkeep.run;
            ++nkeep.len;
        }
        else if ((mkeep.val == m) && (nkeep.val != n))
        {
             nkeep.val = n;
             nkeep.run = 1;
             nkeep.len = (mkeep.alt == n ? mkeep.len : mkeep.run) + 1;
             nkeep.alt = mkeep.val;

             ++mkeep.run;
             ++mkeep.len;
        }
        else if ((mkeep.val != m) && (nkeep.val == n))
        {
             mkeep.val = m;
             mkeep.run = 1;
             mkeep.len = (nkeep.alt == m ? nkeep.len : nkeep.run) + 1;
             mkeep.alt = nkeep.val;

             ++nkeep.run;
             ++nkeep.len;
        }
        else // (mkeep.val != m) && (nkeep.val != n))
        {
             ul_t mlen = (nkeep.alt == m ? nkeep.len : nkeep.run) + 1;
             ul_t nlen = (mkeep.alt == n ? mkeep.len : mkeep.run) + 1;
             mkeep.alt = nkeep.val;
             nkeep.alt = mkeep.val;
             mkeep.val = m;
             nkeep.val = n;
             mkeep.len = mlen;
             nkeep.len = nlen;
             mkeep.run = 1;
             nkeep.run = 1;
        }
        u_t mn_len = (mkeep.len < nkeep.len ? nkeep.len : mkeep.len);
        if (sol_len <= mn_len)
        {
            if (sol_len < mn_len)
            {
                sol_len = mn_len;
                sol_n = 1;
            }
            else
            {
                ++sol_n;
            }
        }
    }
}

void Road::print_solution(ostream &fo) const
{
    fo << ' ' << sol_len << ' ' << sol_n;
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

    void (Road::*psolve)() =
        (naive ? &Road::solve_naive : &Road::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Road road(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (road.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        road.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
