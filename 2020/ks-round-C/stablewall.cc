// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

// #include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<string> vs_t;
typedef set<char> setc_t;
typedef map<char, u_t> c2u_t;
typedef vector<u_t> vu_t;
typedef vector<vu_t> vvu_t;
typedef set<u_t> setu_t;
typedef queue<u_t> qu_t;


static unsigned dbg_flags;

class StableWall
{
 public:
    StableWall(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    typedef vector<setu_t> graph_t;
    void comp_c2u();
    void comp_urows();
    void build_graph();
    u_t r, c;
    vs_t rows;
    vvu_t urows;
    string solution;
    c2u_t c2u;
    string i2c;
    graph_t graph;
    graph_t igraph;
    u_t n_poly;
};

StableWall::StableWall(istream& fi)
{
    fi >> r >> c;
    rows = vs_t{size_t(r), string()};
    for (u_t ri = r; ri > 0; )
    {
        --ri;
        fi >> rows[ri];
    }
}

void StableWall::solve_naive()
{
    comp_c2u();
    comp_urows();
    build_graph();
    qu_t q;
    for (u_t node = 0; node < n_poly; ++node)
    {
        const setu_t& incoming = igraph[node];
        if (incoming.empty())
        {
            q.push(node);
        }
    }
    // bool possible = true;
    while ((!q.empty()) && (solution.size() < n_poly))
    {
        u_t node = q.front();
        q.pop();
        char cc = i2c[node];
        solution.push_back(cc);
        const setu_t& outgoing = graph[node];
        for (u_t og: outgoing)
        {
            setu_t& incoming = igraph[og];
            incoming.erase(node);
            if (incoming.empty())
            {
                q.push(og);
            }
        }
    }
    if (solution.size() < n_poly)
    {
        solution = string("-1");
    }
}

void StableWall::comp_c2u()
{
    setc_t setc;
    for (u_t ri = 0; ri < r; ++ri)
    {
        const string& row = rows[ri];
        for (char cc: row)
        {
            setc.insert(setc.end(), cc);
        }
    }
    u_t i = 0;
    for (char cc: setc)
    {
        c2u.insert(c2u.end(), c2u_t::value_type(cc, i));
        i2c.push_back(cc);
        ++i;
    }
    n_poly = setc.size();
}

void StableWall::comp_urows()
{
    urows.reserve(r);
    for (u_t ri = 0; ri < r; ++ri)
    {
        const string& row = rows[ri];
        vu_t urow;
        urow.reserve(c);
        for (char cc: row)
        {
            c2u_t::const_iterator iter = c2u.find(cc);
            if (iter == c2u.end())
            {
                cerr << "ERROR:" << __LINE__ << '\n';
                exit(1);
            }
            u_t i = iter->second;
            urow.push_back(i);
        }
        urows.push_back(urow);
    }
}

void StableWall::build_graph()
{
    graph.insert(graph.end(), size_t(n_poly), setu_t());
    igraph.insert(igraph.end(), size_t(n_poly), setu_t());
    for (u_t ri = 0; ri < r - 1; ++ri)
    {
        const vu_t& b_row = urows[ri];
        const vu_t& up_row = urows[ri + 1];
        for (u_t ci = 0; ci < c; ++ci)
        {
            char bc = b_row[ci];
            char uc = up_row[ci];
            if (bc != uc)
            {
                setu_t& gbc = graph[bc];
                gbc.insert(gbc.end(), uc);
                setu_t& guc = igraph[uc];
                guc.insert(guc.end(), bc);
            }
        }
    }
}

void StableWall::solve()
{
    solve_naive();
}

void StableWall::print_solution(ostream &fo) const
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

    void (StableWall::*psolve)() =
        (naive ? &StableWall::solve_naive : &StableWall::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        StableWall stablewall(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (stablewall.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        stablewall.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
