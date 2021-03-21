// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
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

static unsigned dbg_flags;

class Planet
{
 public:
    Planet(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    void build_adjs();
    void dfs();
    void dfs_visit(u_t root);
    void set_cycle(u_t n0, u_t n1);
    u_t N;
    vau2_t pipes; // 1-based;
    vu_t solution; 
    vvu_t nadjs; // 0-based
    vector<bool> color; // dfs
    vu_t parent; // dfs
    vu_t cycle;
    vector<bool> in_cycle;
};

Planet::Planet(istream& fi)
{
    fi >> N;
    pipes.reserve(N);
    for (u_t i = 0; i < N; ++i)
    {
        au2_t pipe;
	fi >> pipe[0] >> pipe[1];
	pipes.push_back(pipe);
    }
}

void Planet::solve_naive()
{
    build_adjs();
    dfs();
    solution = vu_t(size_t(N), N); // undef
    for (u_t c: cycle)
    {
        solution[c] = 0;
    }
    for (u_t c: cycle)
    {
	// if (nadjs[c].size() == 3)
	u_t ray = N;
	for (u_t a: nadjs[c])
	{
	    if (solution[a] != 0)
	    {
		ray = a;
	    }
	}
	for (u_t dist = 1; ray != N; ++dist)
	{
	    solution[ray] = dist;
	    const vu_t& adjs = nadjs[ray];
	    ray = N;
	    for (u_t a: adjs)
	    {
		if (solution[a] != 0)
		{
		    ray = a;
		}
	    }
	}
    }
}

void Planet::solve()
{
    solve_naive();
}

void Planet::build_adjs()
{
    nadjs = vvu_t(size_t(N), vu_t());
    for (const au2_t& pipe: pipes)
    {
        u_t x = pipe[0] - 1, y = pipe[1] - 1;
        nadjs[x].push_back(y);
        nadjs[y].push_back(x);
    }
    for (vu_t& adjs: nadjs)
    {
        sort(adjs.begin(), adjs.end());
    }
}

void Planet::dfs()
{
    in_cycle = color = vector<bool>(size_t(N), false);
    parent = vu_t(size_t(N), N); // undefines
    color[0] = true;
    dfs_visit(0);
}

void Planet::dfs_visit(u_t root)
{
    const vu_t& adjs = nadjs[root];
    const u_t rparent = parent[root];
    for (size_t i = 0; (i != adjs.size()) && cycle.empty(); ++i)
    {
        u_t a = adjs[i];
	if (a != rparent)
	{
	    if (color[a]) // cycle found
	    {
	        set_cycle(a, root);
	    }
	    else
	    {
	        parent[a] = root;
		color[a] = true;
		dfs_visit(a);
	    }
	}
    }
}

void Planet::set_cycle(u_t n0, u_t n1)
{
    // find common ancestor
    vector<bool> is_n0_ancestor(size_t(N), false);
    for (u_t p = n0; p != N; p = parent[p])
    {
        is_n0_ancestor[p] = true;
    }
    u_t common_ancestor = N; // undef
    for (u_t p = n1; common_ancestor == N; p = parent[p])
    {
        if (is_n0_ancestor[p])
	{
	    common_ancestor = p;
	}
    }
    for (u_t p = n0; p != common_ancestor; p = parent[p])
    {
        cycle.push_back(p);
    }
    cycle.push_back(common_ancestor);
    vu_t ancestors1;
    for (u_t p = n1; p != common_ancestor; p = parent[p])
    {
        ancestors1.push_back(p);
    }
    cycle.insert(cycle.end(), ancestors1.rbegin(), ancestors1.rend());
}

void Planet::print_solution(ostream &fo) const
{
    for (u_t x: solution)
    {
        fo << ' ' << x;
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

    void (Planet::*psolve)() =
        (naive ? &Planet::solve_naive : &Planet::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Planet planet(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (planet.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        planet.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
