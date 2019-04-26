// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <iostream>
#include <fstream>
#include <string>
// #include <set>
#include <map>
#include <vector>
#include <utility>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
typedef map<u_t, u_t> u2u_t;
typedef vector<u2u_t> vu2u_t;

static unsigned dbg_flags;

class Diverse
{
 public:
    Diverse(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    u_t lr_count(u_t l, u_t r) const;
    u_t n, s;
    u_t solution;
    vu_t trinkets;
    vu2u_t count_till;
};

Diverse::Diverse(istream& fi) : solution(0)
{
    fi >> n >> s;
    trinkets.reserve(n);
    for (u_t i = 0; i < n; ++i)
    {
        u_t t;
        fi >> t;
        trinkets.push_back(t);
    }
}

void Diverse::solve_naive()
{
    u2u_t count;
    count_till.reserve(n + 1);
    count_till.push_back(count);
    for (u_t i = 0; i < n; ++i)
    {
        u_t t = trinkets[i];
        auto er = count.equal_range(t);
        if (er.first == er.second)
        {
            u2u_t::value_type v(t, 1);
            count.insert(er.first, v);
        }
        else
        {
            ++(*er.first).second;
        }
        count_till.push_back(count); 
    }

    for (u_t l = 0; l < n; ++l)
    {
        for (u_t r = l + 1; r <= n; ++r)
        {
            u_t candid = lr_count(l, r);
            if (solution < candid)
            {
                solution = candid;
            }
        }
    }
}

u_t Diverse::lr_count(u_t l, u_t r) const
{
    u_t ret = 0;
    const u2u_t& rcount = count_till[r];
    const u2u_t& lcount = count_till[l];
    for (u2u_t::const_iterator i = rcount.begin(), e = rcount.end();
        i != e; ++i)
    {
        const u2u_t::value_type &v = *i;
        u_t nr = v.second;
        u2u_t::const_iterator li = lcount.find(v.first);
        u_t nl = (li == lcount.end() ? 0 : (*li).second);
        u_t nn = nr - nl;
        if (nn <= s)
        {
            ret += nn;
        }
    }
    return ret;
}   

#if 0
u_t Diverse::lr_count(u_t l, u_t r) const
{
    u2u_t count;
    for (u_t i = l; i < r; ++i)
    {
        u_t t = trinkets[i];
        auto er = count.equal_range(t);
        if (er.first == er.second)
        {
            u2u_t::value_type v(t, 1);
            count.insert(er.first, v);
        }
        else
        {
            ++(*er.first).second;
        }
    }
    u_t ret = 0;
    for (u2u_t::const_iterator i = count.begin(), e = count.end(); i != e; ++i)
    {
        const u2u_t::value_type &v = *i;
        if (v.second <= s)
        {
            ret += v.second;
        }
    }
    return ret;
}
#endif

void Diverse::solve()
{
    solve_naive();
}

void Diverse::print_solution(ostream &fo) const
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

    void (Diverse::*psolve)() =
        (naive ? &Diverse::solve_naive : &Diverse::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Diverse diverse(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (diverse.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        diverse.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
