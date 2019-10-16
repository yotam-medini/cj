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

typedef unsigned char uc_t;
typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
// typedef vector<ul_t> vul_t;
typedef set<uc_t> setuc_t;
typedef array<uc_t, 5> auc5_t;
typedef vector<setuc_t> vsetuc_t;
typedef vector<uc_t> vuc_t;
typedef vector<ul_t> vul_t;
typedef map<ull_t, ull_t> ull_t2ull_t;

static unsigned dbg_flags;
static const auc5_t z5 = {0, 0, 0, 0, 0};

class Teachme
{
 public:
    Teachme(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    ull_t pack(const setuc_t&) const;
    ull_t pack(const auc5_t&) const;
    u_t unpack(auc5_t& v, ull_t x) const;
    ull_t n_subsets(ull_t x) const;
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
    ull_t n_neq = 0, n_sub = 0;
    for (const ull_t2ull_t::value_type& e: skills_count)
    {
        ull_t count = e.second;
        n_neq += count * (n - count);
        ull_t n_sube = n_subsets(e.first);
        n_sub += count * n_sube;
    }
    solution = n_neq - n_sub;
}

ull_t Teachme::pack(const setuc_t& skills) const
{
    auc5_t a5 = z5;
    u_t ai = 0;
    for (uc_t skill: skills)
    {
        a5[ai++] = skill;    
    }
    return pack(a5);
}

ull_t Teachme::pack(const auc5_t& skills) const
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

u_t Teachme::unpack(auc5_t& v, ull_t x) const
{
    u_t nv, nz, xi;
    ull_t xs;
    for (nz = 0, xs = x; (xs % 1000) == 0; xs /= 1000, ++nz) {}
    nv = 5 - nz;
    for (xs = x, xi = 5; xi > 5 - nz; xs /= 1000)
    {
        v[--xi] = 0;
    }
    for ( ; xi > 0; xs /= 1000)
    {
        v[--xi] = xs % 1000;
    }
    return nv;
}

ull_t Teachme::n_subsets(ull_t x) const
{
    ull_t n_sub = 0;
    auc5_t a;
    u_t nv = unpack(a, x);
    for (u_t bits = 1, bits_e = (1u << nv) - 1; bits < bits_e; ++bits)
    {
        auc5_t as = z5;
        u_t ai = 0;
        for (u_t bit = 0; bit < nv; ++bit)
        {
            if (bits & bit)
            {
                as[ai++] = a[bit];
            }
        }
        ull_t xs = pack(as);
        ull_t2ull_t::const_iterator i = skills_count.find(xs);
        if (i != skills_count.end())
        {
            ull_t sub_count = i->second;
            n_sub += sub_count;
        }
    }
    return n_sub;
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
