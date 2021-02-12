// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
// #include <iterator>
// #include <map>
#include <set>
#include <string>
#include <utility>
#include <array>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef array<u_t, 2> au2_t;
typedef vector<u_t> vu_t;
typedef vector<vu_t> vvu_t;
typedef vector<au2_t> vau2_t;
typedef set<u_t> setu_t;
typedef vector<setu_t> vsetu_t;


static unsigned dbg_flags;

class Recipe
{
 public:
    Recipe(const vu_t& _need=vu_t(), u_t g=0) : need(_need), give(g) {}
    vu_t need;
    u_t give;
};
typedef vector<Recipe> vrecipe_t;

class GoldStone
{
 public:
    GoldStone(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    void build();
    u_t n, m, s, r;
    vau2_t streets;
    vsetu_t jstones; // 0-based
    vrecipe_t recipes;

    vvu_t adjs; // [node] -> adjacent  0-based
    vvu_t stone2junc; // stone-number to 0-based junctions
    ull_t solution;
};

GoldStone::GoldStone(istream& fi)
{
    // copy_n(istream_iterator<u_t>(fi), N, back_inserter(a));
    fi >> n >> m >> s >> r;

    streets.reserve(m);
    for (u_t i = 0; i < m; ++i)
    {
        u_t v0, v1;
        fi >> v0 >> v1;
        streets.push_back(au2_t{v0, v1});
    }

    jstones.reserve(n);
    for (u_t i = 0; i < n; ++i)
    {
        u_t ns, stone;
        setu_t ss;
        fi >> ns;
        for (u_t si = 0; si < ns; ++si)
        {
            fi >> stone;
            ss.insert(stone);
        }
        jstones.push_back(ss);
    }

    recipes.reserve(r);
    for (u_t i = 0; i < r; ++i)
    {
        u_t nn, g;
        vu_t need;
        fi >> nn;
        need = vu_t(nn, 0);
        for (u_t ni = 0; ni < nn; ++ni)
        {
            fi >> need[ni];
        }
        sort(need.begin(), need.end());
        fi >> g;
        recipes.push_back(Recipe(need, g));
    }
    
}

void GoldStone::solve_naive()
{
    build();
}

void GoldStone::solve()
{
    solve_naive();
}

void GoldStone::build()
{
    adjs = vvu_t(n, vu_t());
    for (const au2_t& street: streets)
    {
        u_t j0 = street[0] - 1;
        u_t j1 = street[1] - 1;
        adjs[j0].push_back(j1);
        adjs[j1].push_back(j0);
    }
    for (vu_t& a: adjs)
    {
        sort(a.begin(), a.end());
    }

    stone2junc = vvu_t(s + 1, vu_t());
    for (u_t j = 0; j < n; ++j)
    {
        for (u_t stone: jstones[j])
        {
            stone2junc[stone].push_back(j);
        }
    }
}

void GoldStone::print_solution(ostream &fo) const
{
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

    void (GoldStone::*psolve)() =
        (naive ? &GoldStone::solve_naive : &GoldStone::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        GoldStone goldstone(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (goldstone.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        goldstone.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
