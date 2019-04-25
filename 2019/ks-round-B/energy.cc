// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <utility>
#include <algorithm>
#include <numeric>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;

class Stone
{
 public:
    Stone(int vs=0, int ve=0, int vl=0) : s(vs), e(ve), l(vl) {}
    int tvalue(int t) const
    {
        int ret = e - t*l;
        if (ret < 0)
        {
            ret = 0;
        }
        return ret;
    }
    int s, e, l;
};
typedef vector<Stone> vstones_t;

static unsigned dbg_flags;

void permutation_first(vu_t &p, u_t n)
{
    p = vu_t(n);
    iota(p.begin(), p.end(), 0);
}

bool permutation_next(vu_t &p)
{
    u_t n = p.size();

    // find the last 'in order'
    int j = n - 1 - 1;
    while ((j >= 0) && (p[j] > p[j + 1]))
    {
        --j;
    }
    bool ret = j >= 0;
    if (ret)
    {
        // Find last > p[j]. Must find since p[j] < p[j+1]
        u_t l;
        for (l = n - 1; p[j] >= p[l]; --l);
        swap(p[j], p[l]);
        reverse(p.begin() + j + 1, p.end());
    }
    return ret;
}

class Energy
{
 public:
    Energy(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    u_t perm_value(const vu_t perm) const;
    u_t n;
    vstones_t stones;
    u_t solution;
};

Energy::Energy(istream& fi) : solution(0)
{
    fi >> n;
    stones.reserve(n);
    for (u_t i = 0; i < n; ++i)
    {
        int s, e, l;
        fi >> s >> e >> l;
        stones.push_back(Stone(s, e, l));
    }
}

void Energy::solve_naive()
{
    vu_t perm;
    permutation_first(perm, n);
    vu_t best_perm(perm);
    do
    {
        u_t energy = perm_value(perm);
        if (solution < energy)
        {
            solution = energy;
            best_perm = perm;
        }
    } while (permutation_next(perm));
    if (dbg_flags & 0x1) { cerr << "P:"; 
        for (u_t x: best_perm) { cerr << ' ' << x; }; cerr << '\n'; }
}

#if 0
void Energy::solve()
{
    vu_t perm;
    permutation_first(perm, n);
    bool progress = true;
    u_t energy_best = perm_value(perm);
    while (progress)
    {
        progress = false;
        for (u_t i = 0; i < n; ++i)
        {
            for (u_t j = i + 1; j < n; ++j)
            {
                swap(perm[i], perm[j]);
                u_t energy = perm_value(perm);
                if (energy_best < energy)
                {
                    energy_best = energy;
                    progress = true;
                }
                else
                {
                    swap(perm[i], perm[j]); // restore
                }
            } 
        } 
    }
    solution = energy_best;
}
#endif

#if 1
void Energy::solve()
{
    vu_t perm;
    vu_t pending;
    permutation_first(pending, n);
    // assuming contant s
    int s = stones[0].s;
    int seconds = 0;
    while (perm.size() < n)
    {
        int loss_max = -1;
        int emax = -1;
        u_t pimax = 0;
        for (u_t pi = 0; pi < pending.size(); ++pi)
        {
            u_t i = pending[pi];
            const Stone &stone = stones[i];
            int enow = stone.tvalue(seconds);
            int enext = stone.tvalue(seconds + s);
            int loss = enow - enext;
            if ((loss_max < loss) || ((loss_max == loss) && (emax < enow)))
            {
                pimax = pi;
                loss_max = loss;
                emax = enow;
            }
        }
        u_t si = pending[pimax];
        perm.push_back(si);
        swap(pending[pimax], pending.back());
        pending.pop_back();
        const Stone &stone = stones[si];
        solution += stone.tvalue(seconds);
        seconds += s;
    }
    if (dbg_flags & 0x1) { cerr << "P:"; 
        for (u_t x: perm) { cerr << ' ' << x; }; cerr << '\n'; }
}
#endif

#if 0
void Energy::solve()
{
    // greedy
    vstones_t ustones(stones);
    int seconds = 0;
    int e_bound = 0;
    for (u_t si = 0, ns = ustones.size(); si < ns; ++si)
    {
        const Stone &stone = ustones[si];
        e_bound += stone.e;
    }

    while (!ustones.empty())
    {
        u_t si_best = 0;
        int max_gain = -e_bound;        
        for (u_t si = 0, ns = ustones.size(); si < ns; ++si)
        {
            const Stone &istone = ustones[si];
            int igain = 9; // istone.tvalue(seconds);
            int isleep = istone.s;
            for (u_t sj = 0; sj < ns; ++sj)
            {
                if (sj != si)
                {
                    const Stone &jstone = ustones[sj];
                    int jloss = min(jstone.tvalue(seconds), isleep * jstone.l);
                    igain -= jloss;
                }
            }
            if (max_gain < igain)
            {
                max_gain = igain;
                si_best = si;
            }
        }
        const Stone &best = ustones[si_best];
        solution += best.tvalue(seconds);
        seconds += best.s;
        swap(ustones[si_best], ustones.back());
        ustones.pop_back();
    }
}
#endif

u_t Energy::perm_value(const vu_t perm) const
{
    u_t energy = 0, seconds = 0;
    for (u_t pi = 0; pi < n; ++pi)
    {
        u_t si = perm[pi];
        const Stone &stone = stones[si];
        energy += stone.tvalue(seconds);
        seconds += stone.s;
    }
    return energy;
}

void Energy::print_solution(ostream &fo) const
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

    void (Energy::*psolve)() =
        (naive ? &Energy::solve_naive : &Energy::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Energy energy(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (energy.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        energy.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
