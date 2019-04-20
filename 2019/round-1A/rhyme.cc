// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- 2019/April/20

#include <iostream>
#include <fstream>
#include <string>
#include <set>
// #include <map>
#include <vector>
#include <utility>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<string> vs_t;
typedef set<string> sets_t;

static unsigned dbg_flags;

class Rhyme
{
 public:
    Rhyme(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    void mirror(string &m, const string &s) const;
    u_t eqlen(string &s0, const string &s1) const;
    u_t n;
    vs_t words;
    sets_t mirror_pending;
    u_t solution;
};

Rhyme::Rhyme(istream& fi) : solution(0)
{
    fi >> n;
    words.reserve(n);
    string s;
    for (u_t i = 0; i < n; ++i)
    {
        fi >> s;
        words.push_back(s);
    }
}

void Rhyme::solve_naive()
{
    string m;
    for (const string &w: words)
    {
        mirror(m, w);
        mirror_pending.insert(mirror_pending.end(), m);    
    }
    sets_t used;
    used.insert(used.end(), string("")); // So we will ignre empty
    bool found = true;
    while (found)
    {
        found = false;
        string wlast("");
        sets_t::iterator  mi_end = mirror_pending.end();
        sets_t::iterator  mi_last = mi_end, mi0 = mi_end, mi1 = mi_end;
        u_t maxlen = 0;
        for (sets_t::iterator mi = mirror_pending.begin(); mi != mi_end; ++mi)
        {
            const string &w = *mi;
            u_t eql = eqlen(wlast, w);
            while ((eql > maxlen) && 
                (used.find(wlast.substr(0, eql)) != used.end()))
            {
                --eql;
            }
            if (eql > maxlen)
            {
                mi0 = mi_last;
                mi1 = mi;
                maxlen = eql;
            }
            mi_last = mi;
            wlast = w;        
        }
        if (maxlen > 0)
        {
            found = true;
            const string &w = *mi0;
            string prefix = w.substr(0, maxlen);
            used.insert(used.end(), prefix);
            mirror_pending.erase(mi0);
            mirror_pending.erase(mi1);
            solution += 2;
        }
    }
}

void Rhyme::solve()
{
    solve_naive();
}

void Rhyme::mirror(string &m, const string &s) const
{
    m.clear();
    for (int si = s.size(); si > 0; )
    {
        char c = s[--si];
        m.insert(m.end(), c);
    }
}

u_t Rhyme::eqlen(string &s0, const string &s1) const
{
    u_t sz = min(s0.size(), s1.size());
    u_t ret = 0;
    for (; (ret < sz) && (s0[ret] == s1[ret]); ++ret) {}
    return ret;
}

void Rhyme::print_solution(ostream &fo) const
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

    void (Rhyme::*psolve)() =
        (naive ? &Rhyme::solve_naive : &Rhyme::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Rhyme rhyme(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (rhyme.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        rhyme.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
