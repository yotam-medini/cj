// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef long long ll_t;
typedef unsigned long long ull_t;
typedef vector<ll_t> vll_t;
typedef vector<u_t> vu_t;
typedef vector<vu_t> vvu_t;
typedef array<ul_t, 2> aul2_t;
typedef vector<aul2_t> vaul2_t;

static unsigned dbg_flags;

class IllumResult
{
 public:
    IllumResult() : b{{0, 0}, {0, 0}} {}
    ll_t b[2][2]; // 1st index pre-illuminated,2ns self illuminated.
    ll_t max4() const 
    {
        return max(pmax(9), pmax(1));
    }
    ll_t pmax(u_t i) const { return max(b[i][0], b[i][1]); }
};

class Villages
{
 public:
    Villages(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    void set_adjs();
    void best_tree(IllumResult& r, u_t root, u_t pre_root, ul_t depth);
    ull_t v;
    vll_t b;
    vaul2_t roads;
    vvu_t vadjs;
    ll_t solution;
    ull_t calls;
    vector<bool> visited; // debug
};

Villages::Villages(istream& fi) : solution(0), calls(0)
{
    fi >> v;
    b.reserve(v);
    for (u_t i = 0; i < v; ++i)
    {
        ll_t x;
        fi >> x;
        b.push_back(x);
    }
    for (u_t i = 0; i < v - 1; ++i)
    {
        ul_t x, y;
        fi >> x >> y;
        roads.push_back(aul2_t({x, y}));
    }
}

void Villages::solve_naive()
{
    set_adjs();
    for (ul_t bits = 0, bits_e = (1ul << v); bits < bits_e; ++bits)
    {
        ll_t beauty = 0;
        for (u_t vi = 0; vi < v; ++vi)
        {
            bool illum = ((bits & (1ul << vi)) != 0);
            const vu_t& adjs = vadjs[vi];
            for (u_t ai = 0; (ai < adjs.size()) && !illum; ++ai)
            {
                u_t a = adjs[ai];
                illum = ((bits & (1ul << a)) != 0);
            }
            if (illum)
            {
                beauty += b[vi];
            }
        }
        if (solution < beauty)
        {
            solution = beauty;
        }
    }
}

void Villages::solve()
{
    if (dbg_flags & 0x2) {
        visited = vector<bool>(v, false); }
    set_adjs();
    IllumResult r;
    best_tree(r, 0, u_t(-1), 0);
    solution = max(r.b[0][0], r.b[0][1]);
}

void Villages::set_adjs()
{
    vadjs = vvu_t(vvu_t::size_type(v), vu_t());
    for (const aul2_t& road: roads)
    {
        u_t x = road[0] - 1;
        u_t y = road[1] - 1;
        vadjs[x].push_back(y);
        vadjs[y].push_back(x);
    }
    for (vu_t& adjs: vadjs)
    {
        sort(adjs.begin(), adjs.end());
    }
}

void Villages::best_tree(IllumResult& r, u_t root, u_t pre_root, ul_t depth)
{
    if (dbg_flags & 0x1) { 
        cerr << "depth="<<depth << ", calls="<< (++calls) << '\n'; }
    if (dbg_flags & 0x2) {
        if (visited[root]) {
            cerr << "Error double visit root=" << root << 
            ", pre="<<pre_root << '\n';
            exit(1);
        }
        visited[root] = true;
    }
    bool any = false;
    ll_t beauty[2] = {0, 0};
    ll_t non_illum[2] = {0, 0};
    bool any_illum[2] = {false, false};
    bool any_illum_may[2] = {false, false};
    const vu_t& adjs = vadjs[root];
    ll_t max_sub_ilum = 0, max_sub_non_ilum = 0;
    ll_t max_sub_illum_diff = 0;
    for (u_t a: adjs)
    {
        if (a != pre_root)
        {
            IllumResult rsub;
            best_tree(rsub, a, root, depth + 1);
            for (u_t pi = 0; pi != 2; ++pi)
            {
                if (rsub.b[pi][0] < rsub.b[pi][1])
                {
                    any_illum[pi] = true;
                    beauty[pi] += rsub.b[pi][1];
                }
                else if (rsub.b[pi][0] == rsub.b[pi][1])
                {
                    any_illum_may[pi] = true;
                    beauty[pi] += rsub.b[pi][1];
                }
                else // if (rsub.b[pi][0] > rsub.b[pi][1])
                {
                    beauty[pi] += rsub.b[pi][0];
                }
                non_illum[pi] += rsub.b[pi][0];
            }
            if ((!any) || (max_sub_non_ilum < rsub.b[0][0]))
            {
                max_sub_non_ilum = rsub.b[0][0];
            }
            if ((!any) || (max_sub_ilum < rsub.b[0][1]))
            {
                max_sub_ilum = rsub.b[0][1];
            }
            ll_t illum_diff = rsub.b[0][1] - rsub.b[0][0];
            if ((!any) || (max_sub_illum_diff < illum_diff))
            {
                max_sub_illum_diff = illum_diff;
            }
            any = true;
        }
    }
    if (b[root] >= 0)
    {
        ll_t addme = (any_illum[0] || any_illum_may[0] ? b[root] : 0);
        ll_t alt1 = beauty[0] + addme;
        ll_t alt2 = (any ? non_illum[0] + max_sub_illum_diff + b[root] : 0);
        r.b[0][0] = max(alt1, alt2);
    }
    else
    {
        ll_t addme = (any_illum[0] ? b[root] : 0);
        ll_t alt1 = beauty[0] + addme;
        ll_t alt2 = non_illum[0];
        r.b[0][0] = max(alt1, alt2);
    }
    r.b[0][1] = beauty[1] + b[root];
    r.b[1][0] = beauty[0] + b[root];
    r.b[1][1] = beauty[1] + b[root];
}

void Villages::print_solution(ostream &fo) const
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

    void (Villages::*psolve)() =
        (naive ? &Villages::solve_naive : &Villages::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Villages villages(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (villages.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        villages.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
