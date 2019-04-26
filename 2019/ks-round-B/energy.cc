// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <utility>
#include <algorithm>
#include <numeric>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
typedef pair<int, u_t> iu_t;
typedef map<iu_t, int> iu2i_t;

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
    int max_energy(int t, u_t lossi);
    u_t n;
    vstones_t stones;
    vu_t loss_order; // decreasing
    iu2i_t memo;
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

void Energy::solve()
{
    permutation_first(loss_order, n);
    sort(loss_order.begin(), loss_order.end(),
        [this](const u_t &i0, const u_t &i1)
        {
            const Stone &stone0 = stones[i0];
            const Stone &stone1 = stones[i1];
            bool lt = stone0.l * stone1.s > stone1.l * stone0.s;
            return lt;
        });
    solution = max_energy(0, 0);
}

int Energy::max_energy(int t, u_t lossi)
{
    int e = 0;
    if (lossi < n)
    {
        iu_t key(t, lossi);
        auto er = memo.equal_range(key);
        iu2i_t::iterator iter = er.first;
        if (er.first == er.second)
        {
            u_t i = loss_order[lossi];
            const Stone &stone = stones[i];
            int et =  stone.tvalue(t);
            int e_with = et + max_energy(t + stone.s, lossi + 1);
            int e_without = max_energy(t, lossi + 1);
            e = max(e_with, e_without);
            iu2i_t::value_type v(key, e);
            memo.insert(iter, v);
        }
        else
        {
            e = (*iter).second;
        }
    }
    return e;
}

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
