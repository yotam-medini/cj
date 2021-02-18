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
typedef vector<ull_t> vull_t;
typedef vector<vull_t> vvull_t;
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
    static const ull_t MAX_PRICE;
    void build();
    void init_js_prices();
    void js_reduce_price(u_t junction, u_t stone);
    void cook_reduce();
    ull_t jrecipe_price(u_t j, u_t ri) const;
    u_t N, M, S, R;
    vau2_t streets;
    vsetu_t jstones; // 0-based -> 1-based
    vsetu_t jstones0; // 0-based -> 0-based
    vrecipe_t recipes; // 1-based
    vrecipe_t recipes0; // 0-based

    vvu_t adjs; // [node] -> adjacent  0-based
    vvu_t stone2junc; // 0-based stone-number to 0-based junctions
    vvull_t junc_stone_prices; // [junction][stone] 0-0-based
    ull_t solution;
};
//                                  123456789 12
const ull_t GoldStone::MAX_PRICE = 1000000000000; // 10^12

GoldStone::GoldStone(istream& fi) : solution(MAX_PRICE)
{
    // copy_n(istream_iterator<u_t>(fi), N, back_inserter(a));
    fi >> N >> M >> S >> R;

    streets.reserve(M);
    for (u_t i = 0; i < M; ++i)
    {
        u_t v0, v1;
        fi >> v0 >> v1;
        streets.push_back(au2_t{v0, v1});
    }

    jstones.reserve(N);
    for (u_t i = 0; i < N; ++i)
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

    recipes.reserve(R);
    for (u_t i = 0; i < R; ++i)
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
    init_js_prices();
    cook_reduce();
}

void GoldStone::solve()
{
    build();
    junc_stone_prices = vvull_t(size_t(N), vull_t(S, ull_t(MAX_PRICE)));
    vvu_t req_stone_to_recipes = vvu_t(size_t(S), vu_t());
    for (u_t ri = 0; ri < R; ++ri)
    {
        for (u_t s: recipes0[ri].need)
        {
            req_stone_to_recipes[s].push_back(ri);
        }
    }
    typedef unsigned short us_t;
    typedef array<us_t, 2> js_t; // (junction, stone)
    typedef pair<ull_t, js_t> pjs_t; // (price, (junction, stone));
    typedef set<pjs_t> qpjs_t; // {(price, recipe-index)}
    qpjs_t q;
    for (us_t j = 0; j < us_t(N); ++j)
    {
        for (u_t s: jstones0[j])
        {
            if (s == 0)
            {
                solution = 0;
            }
            junc_stone_prices[j][s] = 0;
            pjs_t pjs(0, js_t{j, us_t(s)});
            q.insert(q.end(), pjs);
        }
    }
    while (!q.empty())
    {
        const pjs_t& pjs = *(q.begin());
        const ull_t price = pjs.first;
        const us_t j = pjs.second[0]; 
        const us_t s = pjs.second[1]; 
        q.erase(q.begin());
        for (u_t ri: req_stone_to_recipes[s])
        {
            const us_t give = recipes0[ri].give;
            const ull_t curr_price = junc_stone_prices[j][give];
            const ull_t rprice = jrecipe_price(j, ri);
            if (rprice < curr_price)
            {
                if ((give == 0) && (rprice < solution))
                {
                    solution = rprice;
                }
                const js_t jgive{j, give};
                if (curr_price < MAX_PRICE)
                {
                    const pjs_t del(curr_price, jgive);
                    q.erase(del);
                }
                junc_stone_prices[j][give] = rprice;
                const pjs_t pj(rprice, jgive);
                q.insert(q.end(), pjs_t(rprice, jgive));
            }
        }
        for (u_t a: adjs[j])
        {
            const ull_t aprice = price + 1;
            const ull_t curr_price = junc_stone_prices[a][s];
            if (aprice < curr_price)
            {
                const js_t as{us_t(a), s};
                if (curr_price < MAX_PRICE)
                {
                    const pjs_t del(curr_price, as);
                    q.erase(del);
                }
                junc_stone_prices[a][s] = aprice;
                q.insert(q.end(), pjs_t(aprice, as));
            }
        }
    }
}

void GoldStone::build()
{
    adjs = vvu_t(N, vu_t());
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

    jstones0.reserve(jstones.size());
    for (const setu_t& s1: jstones)
    {
        setu_t s0;
        for (u_t stone1: s1)
        {
            s0.insert(s0.end(), stone1 - 1);
        }
        jstones0.push_back(s0);
    }

    stone2junc = vvu_t(S, vu_t());
    for (u_t j = 0; j < N; ++j)
    {
        for (u_t stone: jstones0[j])
        {
            stone2junc[stone].push_back(j);
        }
    }

    recipes0 = recipes;
    for (Recipe& recipe: recipes0)
    {
        for (u_t &ing: recipe.need)
        {
            --ing;
        }
        --recipe.give;
    }
}

void GoldStone::init_js_prices()
{
    junc_stone_prices = vvull_t(size_t(N), vull_t(S, ull_t(MAX_PRICE)));
    for (u_t s = 0; s < S; ++s)
    {
        for (u_t j: stone2junc[s])
        {
            junc_stone_prices[j][s] = 0;
            if (s == 0) // golden
            {
                solution = 0;
            }
        }
    }
    for (u_t j = 0; j < N; ++j)
    {
        for (u_t s: jstones0[j])
        {
            js_reduce_price(j, s);
        }        
    }
}

void GoldStone::cook_reduce()
{
    bool reducing = true;
    while (reducing)
    {
        reducing = false;
        for (u_t j = 0; j < N; ++j)
        {
            vull_t& sprices = junc_stone_prices[j];
            for (const Recipe& recipe: recipes0)
            {
                ull_t price = 0;
                for (u_t ing: recipe.need)
                {
                    price += sprices[ing];
                }
                if (price < sprices[recipe.give])
                {
                    reducing = true;
                    sprices[recipe.give] = price;
                    if ((recipe.give == 0) && (price < solution))
                    {
                        solution = price;
                    }
                    js_reduce_price(j, recipe.give);
                }
            }
        }
    }
}

void GoldStone::js_reduce_price(u_t junction, u_t stone)
{
    typedef pair<ull_t, u_t> pj_t;
    typedef set<pj_t> qpj_t; qpj_t qpj; // queue (price, junction);
    ull_t p = junc_stone_prices[junction][stone];
    qpj.insert(qpj.end(), pj_t{p, junction});
    while (!qpj.empty())
    {
        const pj_t& pj = *qpj.begin();
        p = pj.first;
        if (p + 1 <= MAX_PRICE)
        {
            u_t j = pj.second;
            qpj.erase(qpj.begin());
            for (u_t a: adjs[j])
            {
                ull_t aprice = junc_stone_prices[a][stone];
                if ((p + 1 < aprice) && (p + 1 < MAX_PRICE))
                {
                    pj_t old{aprice, a};
                    qpj.erase(old);
                    junc_stone_prices[a][stone] = p + 1;
                    qpj.insert(qpj.end(), pj_t{p + 1, a});
                }
            }
        }
    }
}

ull_t GoldStone::jrecipe_price(u_t j, u_t ri) const
{
    const vull_t& sprices = junc_stone_prices[j];
    const Recipe& recipe = recipes0[ri];
    ull_t price = 0;
    for (u_t ing: recipe.need)
    {
        price += sprices[ing];
    }
    return price;
}

void GoldStone::print_solution(ostream &fo) const
{
    if (solution < MAX_PRICE)
    {
        fo << ' ' << solution;
    }
    else
    {
        fo << " -1";
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
