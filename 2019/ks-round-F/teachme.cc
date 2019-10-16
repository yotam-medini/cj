// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <map>
#include <vector>
#include <utility>

#include <cstdlib>

using namespace std;

typedef unsigned char uc_t;
typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
// typedef vector<ul_t> vul_t;
typedef set<uc_t> setuc_t;
typedef vector<setuc_t> vsetuc_t;
typedef vector<uc_t> vuc_t;
typedef vector<ul_t> vul_t;
typedef map<ull_t, ull_t> ull_t2ull_t;

static unsigned dbg_flags;

class Teachme
{
 public:
    Teachme(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    ull_t pack(const setuc_t&) const;
    u_t n, s;
    vsetuc_t eskills;
    ull_t solution;
    ull_t2ull_t skills_count;
};

Teachme::Teachme(istream& fi) : solution(0)
{
    fi >> n >> s;
    eskills.reserve(n);
    for (u_t i = 0; i < n; ++i)
    {
        u_t ns;
        fi >> ns;
        setuc_t skills;
        for (u_t si = 0; si < ns; ++si)
        {
            u_t skill;
            fi >> skill;
            skills.insert(skills.end(), uc_t(skill));
        }
        eskills.push_back(skills);
    }
}

void Teachme::solve_naive()
{
    for (u_t i = 0; i < n; ++i)
    {
        const setuc_t& si = eskills[i]; 
        for (u_t j = 0; j < n; ++j)
        {
            const setuc_t& sj = eskills[j]; 
            vuc_t diff;
            set_difference(si.begin(), si.end(), sj.begin(), sj.end(),
                inserter(diff, diff.end()));
            if (!diff.empty())
            {
                ++solution;
            }
        }
    }
}

void Teachme::solve()
{
    solve_naive();
    for (const setuc_t& skills: eskills)
    {
        ull_t uskills = pack(skills);
        auto er = skills_count.equal_range(uskills);
        if (er.first == er.second)
        {
            ull_t2ull_t::value_type v(uskills, 1);
            skills_count.insert(er.first, v);
        }
        else
        {
            ++(*er.first).second;
        }
    }
    
}

ull_t Teachme::pack(const setuc_t& skills) const
{
    ull_t ret = 0;
    u_t si = 0;
    for (uc_t skill: skills)
    {
        ret = (1000*ret) + skill;
        ++si;
    }
    for (; si < 5; ++si)
    {
        ret *= 1000;
    }
    return ret;
    
}

void Teachme::print_solution(ostream &fo) const
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

    void (Teachme::*psolve)() =
        (naive ? &Teachme::solve_naive : &Teachme::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Teachme teachme(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (teachme.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        teachme.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
