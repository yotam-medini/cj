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
typedef long long ll_t;
typedef unsigned long long ull_t;
// typedef vector<ul_t> vul_t;

typedef vector<string> vs_t;
typedef map<ll_t, vs_t> u2vs_t;
typedef set<char> setc_t;
typedef map<char, u_t> c2u_t;
typedef array<u_t, 2> au2_t;

static unsigned dbg_flags;

class Overrand
{
 public:
    Overrand(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    void init();
    u_t u;
    u2vs_t q2s;
    string solution;
    setc_t c9, c10;
    string sc9, sc10;
};

Overrand::Overrand(istream& fi)
{
    fi >> u;
    for (u_t i = 0; i < 10000; ++i)
    {
        ll_t q;
        string s;
        fi >> q >> s;
        auto er = q2s.equal_range(q);
        u2vs_t::iterator iter = er.first;
        if (er.first == er.second)
        {
            vs_t vs;
            iter = q2s.insert(iter, u2vs_t::value_type(q, vs));
        }
        vs_t& ss = iter->second;
        ss.push_back(s);
    }
}

void Overrand::solve_naive()
{
    init();
    for (u_t i = 0; solution.empty(); ++i)
    {
        const char c = sc10[i];
        if (c9.find(c) == c9.end())
        {
            solution.push_back(c);
        }
    }
    ll_t tenp = 1;
    for (u_t p = 1; p < u; ++p)
    {
        tenp *= 10;
    }
    for (u_t i = 1; i < 10; ++i)
    {
        c2u_t c_count;
        const ll_t low = i*tenp, high = low + tenp;
        for (u2vs_t::const_iterator ui = q2s.begin(), ue = q2s.end();
            ui != ue; ++ui)
        {
            const ll_t q = ui->first;
            if ((low <= q) && (q < high))
            {
                const vs_t& ss = ui->second;
                for (vs_t::const_iterator si = ss.begin(), se = ss.end();
                    si != se; ++si)
                {
                    const string& s = *si;
                    char c = s[0];
                    if (solution.find(c) == string::npos)
                    {
                        auto er = c_count.equal_range(c);
                        c2u_t::iterator iter = er.first;
                        if (er.first == er.second)
                        {
                            iter = c_count.insert(iter, c2u_t::value_type(c, 0));
                        }
                        ++(iter->second);
                    }
                }
            }
        }
        char cmax = '?';
        u_t cmax_val = 0;
        for (const c2u_t::value_type& cn: c_count)
        {
            if (cmax_val < cn.second)
            {
                cmax_val = cn.second;
                cmax = cn.first;
            }
        }
        solution.push_back(cmax);
    }
}

void Overrand::init()
{
    bool done = false;
    for (u2vs_t::const_iterator i = q2s.begin(), e = q2s.end();
        (i != e) && !done; ++i)
    {
        const vs_t& ss = i->second;
        for (vs_t::const_iterator si = ss.begin(), se = ss.end();
            (si != se) && !done; ++si)
        {
            const string& s = *si;
            c9.insert(c9.end(), s[0]);
            for (char c: s)
            {
                c10.insert(c10.end(), c);
            }
            done = (c9.size() == 9) && (c10.size() == 10);
        }
    }
    sc9 = string(c9.begin(), c9.end());
    sc10 = string(c10.begin(), c10.end());
    // cerr << "sc9: " << sc9 << ", sc10: " << sc10 << '\n';
}

void Overrand::solve()
{
    init();
    if (u == 2)
    {
        solve_naive();
    }
    else
    {
        au2_t count_ci[10];
        for (u_t ci = 0; ci != 10; ++ci)
        {
            count_ci[ci] = au2_t{0, ci};
        }
        for (u2vs_t::const_iterator ui = q2s.begin(), ue = q2s.end();
            ui != ue; ++ui)
        {
            const vs_t& ss = ui->second; // ignoring first (Q)
            for (vs_t::const_iterator si = ss.begin(), se = ss.end();
                si != se; ++si)
            {
                const string& s = *si;
                if (s.size() == 16)
                {
                    char c = s[0];
                    u_t ci = sc10.find(c);
                    ++(count_ci[ci][0]);
                }
            }
        }
        sort(count_ci, count_ci + 10);
        solution.push_back(sc10[count_ci[0][1]]);
        for (u_t ci = 9; ci > 0; --ci)
        {
            solution.push_back(sc10[count_ci[ci][1]]);
        }
    }
}

void Overrand::print_solution(ostream &fo) const
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

    void (Overrand::*psolve)() =
        (naive ? &Overrand::solve_naive : &Overrand::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Overrand overrand(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (overrand.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        overrand.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
