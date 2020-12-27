// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
// typedef vector<ul_t> vul_t;

static unsigned dbg_flags;

class Similar
{
 public:
    Similar(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    typedef unordered_set<u_t> hsetu_t;
    size_t n;
    vector<hsetu_t> sets;
    // unordered_set<hsetu_t> e2sets;
    bool found;
    u_t s0, s1;
};

Similar::Similar(istream& fi) : found(false), s0(0), s1(0)
{
    fi >> n;
    sets.insert(sets.end(), n, hsetu_t());
    vector<hsetu_t> common(n, hsetu_t());
    for (size_t si = 0; si < n; ++si)
    {
        hsetu_t& seti = sets[si];
        u_t sz, x;
        fi >> sz;
        for (u_t ei = 0; ei < sz; ++ei)
        {
            fi >> x;
            seti.insert(x);
        }
    }
}

void Similar::solve_naive()
{
    typedef unordered_map<u_t, hsetu_t> e2sets_t;
    vector<hsetu_t> common(n, hsetu_t());
    e2sets_t e2sets;
    for (size_t si = 0; (!found) && (si < n); ++si)
    {
        const hsetu_t& seti = sets[si];
        for (hsetu_t::const_iterator iter = seti.begin();
            (!found) && (iter != seti.end()); ++iter)
        {
            const u_t e = *iter;
            e2sets_t::iterator eiter = e2sets.find(e);
            if (eiter == e2sets.end())
            {
                eiter = e2sets.insert(
                    e2sets.end(), e2sets_t::value_type(e, hsetu_t()));
            }
            hsetu_t& e_sets = eiter->second;
            for (hsetu_t::const_iterator esiter = e_sets.begin();
                (!found) && (esiter != e_sets.end()); ++ esiter)
            {
                const u_t si_other = *esiter;
                hsetu_t& common_other = common[si_other];
                found = common_other.find(si) != common_other.end();
                if (found)
                {
                    s0 = si_other;
                    s1 = si;
                }
                common_other.insert(si);
            }
            eiter->second.insert(si);
        }
    }
}

void Similar::solve()
{
    solve_naive();
}

void Similar::print_solution(ostream &fo) const
{
    if ((s0 == 0) && (s1 == 0))
    {
        fo << "-1\n";
    }
    else
    {
        fo << (s0 + 1) << ' ' << (s1 + 1) << '\n';
    }
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

    void (Similar::*psolve)() =
        (naive ? &Similar::solve_naive : &Similar::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Similar similar(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (similar.*psolve)();
        similar.print_solution(fout);
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
