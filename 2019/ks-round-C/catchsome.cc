// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <numeric>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
typedef vector<vu_t> vvu_t;
typedef set<u_t> setu_t;
typedef map<u_t, u_t> u2u_t;
typedef pair<u_t, u_t> uu_t;

static unsigned dbg_flags;

class Dog
{
 public:
    Dog(u_t _p=0, u_t _a=0) : p(_p), a(_a) {}
    u_t p;
    u_t a;
};
bool operator<(const Dog& d0, const Dog& d1)
{
    bool lt = uu_t(d0.p, d0.a) < uu_t(d1.p, d1.a);
    return lt;
}
typedef vector<Dog> vdog_t;

class CatchSome
{
 public:
    CatchSome(istream& fi);
    void solve_naive();
    void solve_naive1();
    void solve();
    void print_solution(ostream&) const;
 private:
    void sort_dogs();
    void traverse_comb(vu_t& comb, u_t pending);
    u_t comb_time(vu_t& comb);
    u_t sub_cost(const vu_t& pre_cost, const vu_t& pending) const;
    u_t n, k;
    vu_t p;
    vu_t a; // colors;
    u_t solution;
    vdog_t dogs; // sorted
    vvu_t color_dogs; // [color] -> sorted positions    
};

CatchSome::CatchSome(istream& fi) : solution(0)
{
    fi >> n >> k;
    p.reserve(n);
    a.reserve(n);
    u_t x;
    for (u_t i = 0; i < n; ++i)
    {
        fi >> x;
        p.push_back(x);
    }
    for (u_t i = 0; i < n; ++i)
    {
        fi >> x;
        a.push_back(x);
    }
}

void CatchSome::solve_naive()
{
    sort_dogs();
    vu_t comb;
    comb.reserve(color_dogs.size());
    solution = u_t(-1); // infinity
    traverse_comb(comb, k);    
}

void CatchSome::traverse_comb(vu_t& comb, u_t pending)
{
    if (pending == 0) // sum(comb) == k
    {
        u_t t = comb_time(comb);
        if (solution > t)
        {
            solution = t;
        }
    }
    else
    {
        u_t ai = comb.size();
        if (ai < color_dogs.size())
        {
            const vu_t& dogs_ai = color_dogs[ai];
            comb.push_back(0);
            u_t nde = min(u_t(dogs_ai.size()), pending);
            for (u_t nd = 0; nd <= nde; ++nd)
            {
                comb.back() = nd;
                traverse_comb(comb, pending - nd);
            }
            comb.pop_back();
        }
    }
}

u_t CatchSome::comb_time(vu_t& comb)
{
    u_t ret = 0;
    u_t farthest = 0;
    for (u_t i = 0, e = comb.size(); i != e; ++i)
    {
        u_t sz = comb[i];
        if (sz > 0)
        {
            const vu_t& cdogs = color_dogs[i];
            u_t last_dog = cdogs[sz - 1];
            ret += last_dog;
            if (farthest < last_dog)
            {
                farthest = last_dog;
            }
        }
    }
    ret = 2*ret; // back home
    ret -= farthest; // except last
    return ret;
}

void CatchSome::solve_naive1()
{
    sort_dogs();
    solution = u_t(-1);
    const u_t n_colors = color_dogs.size();
    const vu_t& cdog0_pos = color_dogs[0];
    const u_t cdog0_size = cdog0_pos.size();
    for (u_t last_dog = 0; last_dog != n_colors; ++last_dog)
    {
        vvu_t cost{vvu_t::size_type(n_colors), vu_t()};
        cost[0].push_back(0); // j=0
        {
            const u_t je = min(k, cdog0_size);
            const u_t w = (0 == last_dog ? 1 : 2);
            for (u_t j = 0; j < je; ++j)
            {
                cost[0].push_back(w * cdog0_pos[j]);
            }
        }
        
        for (u_t i = 1; i != n_colors; ++i)
        {
            const vu_t& pcosts = cost[i - 1];
            const vu_t& cdog_pos = color_dogs[i];
            cost[i].push_back(0); // j=0
            vu_t& icosts = cost[i];
            const u_t w = (i == last_dog ? 1 : 2);
            const u_t psize = pcosts.size();
            const u_t isize = cdog_pos.size();
            const u_t je = min(k, psize - 1 + isize);
            for (u_t j = 1; j <= je; ++j)
            {
                u_t ij_cost = u_t(-1);
                const u_t jjb = (j <= isize ? 0 : j - isize);
                const u_t jje = min(j + 1, psize);
                for (u_t jj = jjb; jj < jje; ++jj)
                {
                    u_t pcost = (jj < psize ? pcosts[jj] : 0);
                    u_t ic = j - jj;
                    u_t icost = ((ic > 0) && (ic - 1 < isize)
                        ? w * cdog_pos[ic - 1] : 0);
                    u_t pi_cost = pcost + icost;
                    if (ij_cost > pi_cost)
                    {
                        ij_cost = pi_cost;
                    }
                }
                icosts.push_back(ij_cost);
            }
        }
        u_t ld_cost = cost[n_colors - 1][k];
        if (solution > ld_cost)
        {
            solution = ld_cost;
        }
    }
}

void CatchSome::solve()
{
    sort_dogs();
    u_t n_colors = color_dogs.size();
    if (n_colors == 1)
    {
        solution = color_dogs[0][k - 1];
    }
    else
    {
        vu_t all(vu_t::size_type(n_colors), 0);
        iota(all.begin(), all.end(), 0);
        const vu_t pre_cost(vu_t::size_type(1), 0);
        solution = sub_cost(pre_cost, all);
    }
}

void CatchSome::sort_dogs()
{
    dogs.reserve(n);
    for (u_t i = 0; i < n; ++i)
    {
        dogs.push_back(Dog(p[i], a[i]));
    }
    sort(dogs.begin(), dogs.end());

    setu_t colors(a.begin(), a.end());
    const u_t n_colors = colors.size();
    u2u_t color_map;
    u_t nci = 0;
    for (u_t c: colors)
    {
        u2u_t::value_type v(c, nci++);
        color_map.insert(color_map.end(), v);
    }
    color_dogs = vvu_t(vvu_t::size_type(n_colors), vu_t());
    for (const Dog& dog: dogs)
    {
        u_t ci = color_map[dog.a];
        color_dogs[ci].push_back(dog.p);
    }
}

u_t CatchSome::sub_cost(const vu_t& pre_cost, const vu_t& pending) const
{
    u_t ret(u_t(-1));
    u_t sz = pending.size();
    if (sz == 1) // last dog color
    {
        const vu_t& cdog_pos = color_dogs[pending[0]];
        const u_t psize = pre_cost.size();
        const u_t isize = cdog_pos.size();

        const u_t jjb = (isize < k ? k - isize : 0);
        const u_t jje = min(k, psize - 1);
        for (u_t jj = jjb; jj <= jje; ++jj)
        {
            u_t pcost = pre_cost[jj];
            u_t ic = k - jj;
            u_t icost = (ic > 0 ? cdog_pos[ic - 1] : 0);
            u_t pi_cost = pcost + icost;
            if (ret > pi_cost)
            {
                ret = pi_cost;
            }
        }
    }
    else
    {
        vu_t::const_iterator pb = pending.begin();
        const vu_t::const_iterator iib[2] = {pb, pb + sz/2};
        const vu_t::const_iterator iie[2] = {pb + sz/2, pending.end()};
        for (u_t bin = 0; bin != 2; ++bin)
        {
            const vu_t sub_pending(iib[1 - bin], iie[1 - bin]);
            vu_t curr_cost(pre_cost);
            for (vu_t::const_iterator ii = iib[bin]; ii != iie[bin]; ++ii)
            {
                u_t i = *ii;
                const vu_t& cdog_pos = color_dogs[i];
                vu_t next_cost(vu_t::size_type(1), 0); // j=0
                const u_t psize = curr_cost.size();
                const u_t isize = cdog_pos.size();
                const u_t je = min(k, psize - 1 + isize);
                for (u_t j = 1; j <= je; ++j)
                {
                    u_t ij_cost = u_t(-1);
                    const u_t jjb = (isize < j ? j - isize : 0);
                    const u_t jje = min(j, psize - 1);
                    for (u_t jj = jjb; jj <= jje; ++jj)
                    {
                        u_t pcost = curr_cost[jj];
                        u_t ic = j - jj;
                        u_t icost = (ic > 0 ? 2 * cdog_pos[ic - 1] : 0);
                        u_t pi_cost = pcost + icost;
                        if (ij_cost > pi_cost)
                        {
                            ij_cost = pi_cost;
                        }
                    }
                    next_cost.push_back(ij_cost); // [j]
                }
                swap(curr_cost, next_cost);
            }
            u_t bret = sub_cost(curr_cost, sub_pending);
            if (ret > bret)
            {
                ret = bret;
            }
        }         
    }
    return ret;
}

void CatchSome::print_solution(ostream &fo) const
{
    fo << ' ' << solution;
}

int main(int argc, char ** argv)
{
    const string dash("-");

    bool naive = false;
    bool naive1 = false;
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
        else if (opt == string("-naive1"))
        {
            naive1 = true;
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

    void (CatchSome::*psolve)() =
        (naive ? &CatchSome::solve_naive 
             : (naive1 ? &CatchSome::solve_naive1 : &CatchSome::solve));
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        CatchSome catchsome(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (catchsome.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        catchsome.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
