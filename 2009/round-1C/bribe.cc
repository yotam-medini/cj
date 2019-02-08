// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <map>
#include <vector>
#include <utility>

#include <cstdlib>

using namespace std;

typedef unsigned long ul_t;
typedef set<ul_t> setu_t;
typedef vector<unsigned> vu_t;
typedef pair<unsigned, unsigned> uu_t;
typedef map<uu_t, ul_t> uu2ul_t;

static unsigned dbg_flags;

static void iota(vu_t& v, unsigned n, unsigned val=0)
{
    v.clear();
    v.reserve(n);
    for (unsigned k = 0; k < n; ++k, val++)
    {
        v.push_back(val);
    }
}

void permutation_first(vu_t &p, unsigned n)
{
    iota(p, n, 0);
}

bool permutation_next(vu_t &p)
{
    unsigned n = p.size();

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
        unsigned l;
        for (l = n - 1; p[j] >= p[l]; --l);
        swap(p[j], p[l]);
        reverse(p.begin() + j + 1, p.end());
    }
    return ret;
}

class Bribe
{
 public:
    Bribe(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    // ul_t coins(unsigned cellb, unsigned celle);
    ul_t order_cost(const vu_t order) const;
    ul_t get_seg_cost(unsigned qi, unsigned qj);
    ul_t p;
    vu_t q;
    vu_t qex;
    uu2ul_t seg_cost;
    // setu_t q_in;
    // setu_t q_out;
    ul_t solution;
};

Bribe::Bribe(istream& fi) : solution(0)
{
    fi >> p;
    unsigned nq;
    fi >> nq;
    q = vu_t(vu_t::size_type(nq), 0);
    for (unsigned i = 0; i < nq; ++i)
    {
        unsigned v;
        fi >> v;
        q[i] = v; // leave room for 0
    }
    sort(q.begin(), q.end());
}

void Bribe::solve_naive()
{
    vu_t order;
    permutation_first(order, q.size());
    solution = order_cost(order);
    while (permutation_next(order))
    {
        ul_t cost = order_cost(order);
        if (solution > cost)
        {
            solution = cost;
        }
    }
}

ul_t Bribe::order_cost(const vu_t order) const
{
    ul_t cost = 0;
    vector<bool> cells = vector<bool>(vector<bool>::size_type(p + 2), false);
    cells[0] = true;
    cells[p + 1] = true;
    for (unsigned i = 0; i < q.size(); ++i)
    {
        unsigned cell = q[order[i]];
        cells[cell] = true;
        for (unsigned j = cell + 1; !cells[j]; ++j, ++cost) {}
        for (unsigned j = cell - 1; !cells[j]; --j, ++cost) {}
    }
    return cost;
}

void Bribe::solve()
{
    qex.reserve(q.size() + 2);
    qex.push_back(0);
    qex.insert(qex.end(), q.begin(), q.end());
    qex.push_back(p + 1);
    solution = get_seg_cost(0, qex.size() - 1);
}

ul_t Bribe::get_seg_cost(unsigned qi, unsigned qj)
{
    ul_t cost = ul_t(-1);
    uu_t key(qi, qj);
    auto where = seg_cost.equal_range(key);
    if (where.first == where.second)
    {
        if (qj <= qi + 1)
        {
            cost = 0;
        }
        else
        {
            cost = qex[qj] - qex[qi] - 2;
            ul_t sub_cost = ul_t(-1);
            for (unsigned i = qi + 1; i < qj; ++i)
            {
                ul_t i_sub_cost = get_seg_cost(qi, i) + get_seg_cost(i, qj);
                if (sub_cost > i_sub_cost)
                {
                    sub_cost = i_sub_cost;
                }
            }
            cost += sub_cost;
        }
        uu2ul_t::value_type v(key, cost);
        seg_cost.insert(where.first, v);
    }
    else
    {
        cost = (*where.first).second;
    }
    return cost;
}

void Bribe::print_solution(ostream &fo) const
{
    fo << " " << solution;
}

int main(int argc, char ** argv)
{
    const string dash("-");

    unsigned n_opts = 0;
    bool naive = false;

    if ((argc > 1) && (string(argv[1]) == "-naive"))
    {
        naive = true;
        n_opts = 1;
    }
    int ai_in = n_opts + 1;
    int ai_out = ai_in + 1;
    int ai_dbg = ai_out + 1;
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

    if (ai_dbg < argc) { dbg_flags = strtoul(argv[ai_dbg], 0, 0); }

    unsigned n_cases;
    *pfi >> n_cases;

    void (Bribe::*psolve)() =
        (naive ? &Bribe::solve_naive : &Bribe::solve);

    ostream &fout = *pfo;
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        if (getenv("TELLG"))
        {
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg="<<pfi->tellg() << "\n";
        }
        Bribe problem(*pfi);
        (problem.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        problem.print_solution(fout);
        fout << "\n";
        fout.flush();

    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
