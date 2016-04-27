// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <map>
#include <vector>
#include <utility>

#include <cstdlib>

using namespace std;

// typedef unsigned long ul_t;
// typedef vector<ul_t> vul_t;
typedef vector<int> vi_t;
typedef vector<unsigned> vu_t;
typedef set<unsigned> setu_t;
typedef map<unsigned, unsigned> u2u_t;

static unsigned dbg_flags;

class Cycle
{
 public:
    // setu_t e;
    Cycle() : max_extern_path(0) {}
    u2u_t e2max_extern_path;
    unsigned emin() const { return (*(e2max_extern_path.begin())).first; }
    unsigned max_extern_path;
};

typedef map<unsigned, Cycle> u2cyc_t;

class BFF
{
 public:
    BFF(istream& fi);
    void solve();
    void print_solution(ostream&);
 private:
    void dump(ostream& f=cerr) const;
    void get_paths();
    void get_cycles();
    unsigned N;
    vu_t bff;
    u2cyc_t cycles; // keys belong to cycles
    vi_t cycle_begin; // 1st enterance to parh
    unsigned solution;
};

BFF::BFF(istream& fi) : solution(0)
{
    fi >> N;
    bff.reserve(N);
    for (unsigned i = 0; i < N; ++i)
    {
        unsigned b;
        fi >> b;
        bff.push_back(b - 1);
    }
}

void BFF::solve()
{
    get_cycles();
    get_paths();
    if (dbg_flags & 0x1) { dump(); }

    unsigned solution_2cycles = 0, solution_large_cycle = 0;
    for (auto ci = cycles.begin(); ci != cycles.end(); ++ci)
    {
        const Cycle& cycle = (*ci).second;
        const u2u_t &e2m = cycle.e2max_extern_path;
        if (e2m.size() == 2)
        {
            unsigned candid = 2;
            for (auto ei = e2m.begin(); ei != e2m.end(); ++ei)
            {
                candid += (*ei).second;
            }
            solution_2cycles += candid;
        }
        else
        {
            unsigned candid = e2m.size();
            if (solution_large_cycle < candid)
            {
                solution_large_cycle = candid;
            }
        }
    }
    solution = max(solution_2cycles, solution_large_cycle);
}

void BFF::get_cycles()
{
    cycle_begin = vi_t(vi_t::size_type(N), -1);
    setu_t visited;
    for (unsigned e = 0; e < N; ++e)
    {
        setu_t curr_path;
        setu_t::iterator w;
        unsigned f;
        for (f = e;
            ((w = curr_path.find(f)) == curr_path.end()) &&
                (cycles.find(f) == cycles.end());
             f = bff[f])
        {
            visited.insert(visited.end(), f);
            curr_path.insert(curr_path.end(), f);
        }
        if (w != curr_path.end())
        {   // cycle coudn;
            unsigned emin = f;
            Cycle cycle;
            u2u_t &e2m = cycle.e2max_extern_path;
            e2m.insert(e2m.end(), u2u_t::value_type(f, 0));
            for (unsigned n = bff[f]; n != f; n = bff[n])
            {
                // cycle.e.insert(cycle.e.end(), n);
                e2m.insert(e2m.end(), u2u_t::value_type(n, 0));
                if (emin > f)
                {
                    emin = f;
                }
            }
            cycles.insert(cycles.end(), u2cyc_t::value_type(emin, cycle));
            for (auto i = e2m.begin(); i != e2m.end(); ++i)
            {
                cycle_begin[(*i).first] = emin;
            }
        }
    }
}

void BFF::get_paths()
{
    setu_t done;
    for (unsigned e = 0; e < N; ++e)
    {
        if (cycle_begin[e] == -1)
        {
            unsigned sz = 1, f;
            for (f = bff[e]; cycle_begin[f] == -1; f = bff[f], ++sz);
            Cycle& cycle = cycles[cycle_begin[f]];
            if (cycle.e2max_extern_path[f] < sz)
            {
                cycle.e2max_extern_path[f] = sz;
                if (cycle.max_extern_path < sz)
                {
                    cycle.max_extern_path = sz;
                }
            }
        }
    }
}


void BFF::print_solution(ostream &fo)
{
    fo << " " << solution;
}

void BFF::dump(ostream &oe) const
{
    const char *delim = "";
    oe << "{ BFF:dump\n";
    oe << "  bff: {";
    for (unsigned i = 0; i < N; ++i)
    {
        oe << delim << i << ": " << bff[i];
        delim = ", ";
    }
    oe << "  }\n";
    oe << "  cycles: {\n";
    for (auto ci = cycles.begin(); ci != cycles.end(); ++ci)
    {
        unsigned emin = (*ci).first;
        const Cycle &cycle = (*ci).second;
        const u2u_t &e2m = cycle.e2max_extern_path;
        oe << "    [" << emin << "]: {";
        delim = "";
        for (auto ei = e2m.begin(); ei != e2m.end(); ++ei)
        {
            oe << delim << (*ei).first << ": " << (*ei).second;
            delim = ", ";
        }
        oe << "}\n";
    }
    oe << "  }\n";
    delim = "";
    oe << "  cycle_begin: {";
    for (unsigned i = 0; i < N; ++i)
    {
        if (cycle_begin[i] != -1)
        {
            oe << delim << i << ": " << cycle_begin[i];
            delim = ", ";
        }
    }
    oe << "  }\n";
    oe << "} BFF:dump\n";
}

int main(int argc, char ** argv)
{
    const string dash("-");

    istream *pfi = (argc < 2 || (string(argv[1]) == dash))
         ? &cin
         : new ifstream(argv[1]);
    ostream *pfo = (argc < 3 || (string(argv[2]) == dash))
         ? &cout
         : new ofstream(argv[2]);

    if ((!pfi) || (!pfo) || pfi->fail() || pfo->fail())
    {
        cerr << "Open file error\n";
        exit(1);
    }

    if (argc > 3) { dbg_flags = strtoul(argv[3], 0, 0); }
    // bool large = (argc > 4) && !strcmp(argv[4], "-large");

    unsigned n_cases;
    *pfi >> n_cases;

    ostream &fout = *pfo;
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        BFF bff(*pfi);
        // cerr << "Case ci="<<ci << " (ci+1)="<<ci+1 << "\n";
        bff.solve();
        fout << "Case #"<< ci+1 << ":";
        bff.print_solution(fout);
        fout << "\n";
        fout.flush();
        
    }
    
    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}

