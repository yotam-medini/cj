// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

// #include <algorithm>
#include <fstream>
#include <iostream>
#include <array>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<bool> vb_t;
typedef array<u_t, 2> au2_t;
typedef map<au2_t, vb_t> u2vb_t;
typedef set<vb_t> setvb_t;

static unsigned dbg_flags;

class DoubleOrNot
{
 public:
    DoubleOrNot(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    void s2vb(vb_t& vb, const string& s) const;
    void vb_double(vb_t& r, const vb_t& a) const;
    void vb_not(vb_t& r, const vb_t& a) const;
    vb_t S, E;
    int solution;
};

DoubleOrNot::DoubleOrNot(istream& fi) : solution(-1)
{
    string s;
    fi >> s;
    s2vb(S, s);
    fi >> s;
    s2vb(E, s);
}

void DoubleOrNot::solve_naive()
{
    u_t bump = 0;
    setvb_t done;
    u2vb_t q;
    q.insert(q.end(), u2vb_t::value_type(au2_t{u_t(0), bump++}, S));
    done.insert(done.end(), S);
    while ((!q.empty()) && (solution == -1))
    {
        const u2vb_t::value_type& vmin = *(q.begin());
        const u_t d = vmin.first[0];
        if (vmin.second == E)
        {
            solution = d;
        }
        else
        {
            vb_t adjs[2];
            vb_double(adjs[0], vmin.second);
            vb_not(adjs[1], vmin.second);
            q.erase(q.begin()); // pop
            for (u_t i: {0, 1})
            {
                const vb_t& adj = adjs[i];
                if (adj.size() < 16)
                {
                    setvb_t::iterator iter = done.find(adj);
                    if (iter == done.end())
                    {
                        au2_t key{d + 1, bump++};
                        q.insert(q.end(), u2vb_t::value_type(key, adj));
                        done.insert(iter, adj);
                    }
                }
            }
        }
    }
}

void DoubleOrNot::solve()
{
     solve_naive();
}

void DoubleOrNot::print_solution(ostream &fo) const
{
    if (solution >= 0)
    {
        fo << ' ' << solution;
    }
    else
    {
        fo << " IMPOSSIBLE";
    }
}

void DoubleOrNot::s2vb(vb_t& vb, const string& s) const
{
    vb.reserve(s.size());
    for (string::const_reverse_iterator i = s.rbegin(), e = s.rend();
        i != e; ++i)
    {
        const char c = *i;
        vb.push_back(c != '0');
    }
}

void DoubleOrNot::vb_double(vb_t& r, const vb_t& a) const
{
    r.clear();
    r.reserve(a.size() + 1);
    r.push_back(false);
    r.insert(r.end(), a.begin(), a.end());
}

void DoubleOrNot::vb_not(vb_t& r, const vb_t& a) const
{
    r.clear();
    r.reserve(a.size());
    for (bool b: a)
    {
        r.push_back(!b);
    }
    while ((r.size() > 1) && !(r.back()))
    {
        r.pop_back();
    }
}

int main(int argc, char ** argv)
{
    const string dash("-");

    bool naive = false;
    bool tellg = false;
    u_t solve_ver = 0;
    int rc = 0, ai;

    for (ai = 1; (rc == 0) && (ai < argc) && (argv[ai][0] == '-') &&
        argv[ai][1] != '\0'; ++ai)
    {
        const string opt(argv[ai]);
        if (opt == string("-naive"))
        {
            naive = true;
        }
        else if (opt == string("-solve1"))
        {
            solve_ver = 1;
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

    void (DoubleOrNot::*psolve)() =
        (naive ? &DoubleOrNot::solve_naive : &DoubleOrNot::solve);
    if (solve_ver == 1) { psolve = &DoubleOrNot::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        DoubleOrNot doubleOrNot(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (doubleOrNot.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        doubleOrNot.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
