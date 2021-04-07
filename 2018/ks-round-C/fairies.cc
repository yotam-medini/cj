// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
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
typedef vector<u_t> vu_t;
typedef vector<vu_t> vvu_t;
typedef array<u_t, 2> au2_t;
typedef vector<au2_t> vau2_t;
typedef set<u_t> setu_t;
typedef set<vu_t> setvu_t; // sorted vu_t
typedef set<vau2_t> setvau2_t; // sorted by au2_t[0]

static unsigned dbg_flags;

class Stick
{
 public:
   Stick(const au2_t& _ij={0, 0}, u_t _len=0) : ij(_ij), len(_len) {}
   au2_t ij;
   u_t len;
};
typedef vector<Stick> vstick_t;

class Fairies
{
 public:
    Fairies(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    void get_sticks();
    bool form_polygon(const vu_t& stick_idxs) const;
    void backtrack(const vu_t& picked, const vu_t& available);
    void try_subsets(const vu_t& picked);
    void backtrack_nodes(vau2_t& picked, u_t nodes_used);
    void add_polygons(const vau2_t& picked);
    u_t N;
    vvu_t L;
    u_t solution;
    vstick_t sticks;
    setvu_t polygons;
    setvau2_t ij_polygons;
};

Fairies::Fairies(istream& fi) : solution(0)
{
    fi >> N;
    L.reserve(N);
    for (u_t i = 0; i < N; ++i)
    {
        vu_t Li;
        copy_n(istream_iterator<u_t>(fi), N, back_inserter(Li));
	L.push_back(Li);
    }
}

void Fairies::solve_naive()
{
    get_sticks();
    vu_t picked, available;
    for (u_t si = 0; si < sticks.size(); ++si)
    {
        available.push_back(si);
    }
    backtrack(picked, available);
}

void Fairies::get_sticks()
{
    for (u_t i = 0; i < N; ++i)
    {
	for (u_t j = i + 1; j < N; ++j)
	{
	    u_t len = L[i][j];
	    if (len > 0)
	    {
	        sticks.push_back(Stick(au2_t{i, j}, len));
	    }
	}
    }
    if (dbg_flags & 0x1) { cerr << "#(sticks)=" << sticks.size() << '\n'; }
}

void Fairies::backtrack(const vu_t& picked, const vu_t& available)
{
    if (available.empty())
    {
        if (dbg_flags & 0x2) { cerr << "#(picked)=" << picked.size() << '\n'; }
	try_subsets(picked);
    }
    else
    {
        vu_t sub_picked(picked);
        for (u_t si: available)
	{
	    sub_picked.push_back(si);
	    const Stick& stick = sticks[si];
	    vu_t sub_available;
	    for (u_t a: available)
	    {
	        const Stick& a_stick = sticks[a];
	        if ((a_stick.ij[0] != stick.ij[0]) &&
	            (a_stick.ij[0] != stick.ij[1]) &&
	            (a_stick.ij[1] != stick.ij[0]) &&
	            (a_stick.ij[1] != stick.ij[1]))
		{
		    sub_available.push_back(a);
		}
		backtrack(sub_picked, sub_available);
	    }
	    sub_picked.pop_back();
	}
    }
    
}

void Fairies::try_subsets(const vu_t& picked)
{
    vu_t spicked(picked);
    sort(spicked.begin(), spicked.end());
    const u_t n = spicked.size(), max_mask = 1u << n;
    for (u_t mask = 0; mask < max_mask; ++mask)
    {
        vu_t stick_idxs;
	for (u_t si = 0; si < n; ++si)
	{
	    if ((1u << si) & mask)
	    {
	        stick_idxs.push_back(spicked[si]);
	    }
	}
	if (polygons.find(stick_idxs) == polygons.end())
	{
	    if (form_polygon(stick_idxs))
	    {
	        ++solution;
	        polygons.insert(polygons.end(), stick_idxs);
	    }
	}
    }
}

bool Fairies::form_polygon(const vu_t& stick_idxs) const
{
    u_t sum = 0, lmax = 0;
    for (const u_t si: stick_idxs)
    {
        u_t len = sticks[si].len;
	sum += len;
	if (lmax < len)
	{
	    lmax = len;
	}
    }
    bool can = lmax < sum - lmax;
    return can;
}

void Fairies::solve()
{
    vau2_t picked;
    backtrack_nodes(picked, 0);
    solution = ij_polygons.size();
}

void Fairies::backtrack_nodes(vau2_t& picked, u_t nodes_used)
{
    bool full = true;
    for (u_t i = (picked.empty() ? 0 : picked.back()[0] + 1); i < N; ++i)
    {
	const u_t ibit = 1u << i;
	if ((nodes_used & ibit) == 0)
	{
	    nodes_used |= ibit;
	    for (u_t j = i + 1; j < N; ++j)
	    {
		const u_t jbit = 1u << j;
		if (((nodes_used & jbit) == 0) && (L[i][j] > 0))
		{
		    full = false;
		    nodes_used |= jbit;
		    picked.push_back(au2_t{i, j});
		    backtrack_nodes(picked, nodes_used);
		    picked.pop_back();
		    nodes_used ^= jbit;
		}
	    }
	    nodes_used ^= ibit;
	}
    }
    if (full)
    {
        if (dbg_flags & 0x2) { cerr << "#(picked)=" << picked.size() << '\n'; }
	if (picked.size() >= 3)
	{
	    add_polygons(picked);
	}
    }
}

void Fairies::add_polygons(const vau2_t& picked)
{
    const u_t sz = picked.size();
    const u_t mask_max = 1u << sz;
    for (u_t mask = (1+2+4); mask < mask_max; ++mask)
    {
        vau2_t poly;
	for (u_t pi = 0; pi < sz; ++pi)
	{
	    if (((1u << pi) & mask) != 0)
	    {
	        poly.push_back(picked[pi]);
	    }
	}
	if (poly.size() >= 3)
	{
	    u_t len_max = 0, len_sum = 0;
	    for (const au2_t& ij: poly)
	    {
	        const u_t i = ij[0], j = ij[1];
	        const u_t len = L[i][j];
		if (len_max < len)
		{
		    len_max = len;
		}
		len_sum += len;
	    }
	    if (len_max < len_sum - len_max)
	    {
	        ij_polygons.insert(ij_polygons.end(), poly);
	    }
	}
    }
}

void Fairies::print_solution(ostream &fo) const
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

    void (Fairies::*psolve)() =
        (naive ? &Fairies::solve_naive : &Fairies::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Fairies fairies(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (fairies.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        fairies.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
