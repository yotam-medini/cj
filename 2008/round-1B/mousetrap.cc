// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <deque>
#include <utility>

#include <cstdlib>

using namespace std;

typedef unsigned long ul_t;
typedef vector<ul_t> vul_t;
typedef vector<vul_t> vvul_t;

static unsigned dbg_flags;

void v2err(const string &msg, const vul_t& v)
{
    cerr << msg;
    for (auto i: v) { cerr << ' ' << i; }
    cerr << "\n";
}

class MouseTrap
{
 public:
    MouseTrap(istream& fi);
    void solve();
    void solve_naive();
    void print_solution(ostream&) const;
 private:
    ul_t K;
    vul_t indices;
    vul_t solution;
    vvul_t power_count;
    void print_sol_pc() const;
};

MouseTrap::MouseTrap(istream& fi)
{
    fi >> K;
    unsigned n;
    fi >> n;
    for (unsigned i = 0; i < n; ++i)
    {
        ul_t j;
        fi >> j;
        indices.push_back(j);
    }
}

void MouseTrap::solve_naive()
{
    solution = vul_t(vul_t::size_type(K + 1), 0);
    deque<ul_t> deck;
    for (ul_t k = 0; k < K; ++k) { deck.push_back(k + 1); }
    ul_t elim = 1;
    while (!deck.empty())
    {
        ul_t pos;
        for (ul_t recycle = 0; recycle < elim - 1; ++recycle)
        {
            pos = deck.front();
            deck.pop_front();
            deck.push_back(pos);
        }
        pos = deck.front();
        deck.pop_front();
        solution[pos] = elim++;
        if (dbg_flags & 0x1)
        {
            v2err("solution", solution);
            { vul_t vdeck(deck.begin(), deck.end()); 
              v2err("deck", vdeck); 
            }
        }
    }
}

void MouseTrap::print_sol_pc() const
{
    v2err("solution", solution);
    for (unsigned p = 0; p < power_count.size(); ++p)
    {
        ostringstream ostr;
        ostr << "pc[" << p << "]";
        v2err(ostr.str(), power_count[p]);
    }
}

void MouseTrap::solve()
{
    solution = vul_t(vul_t::size_type(K + 1), 0);

    // init power count;
    for (ul_t chunk = 1; chunk <= K; chunk *= 2)
    {
        power_count.push_back(vul_t());
        vul_t& pc = power_count.back();
        pc.insert(pc.end(), K/chunk, chunk);
        ul_t residue = K % chunk;
        if (residue > 0)
        {
            pc.push_back(residue);
        }
    }

    ul_t elim = 0;
    ul_t unset_behind = 0;
    while (elim < K)
    {
if (dbg_flags & 0x1) { print_sol_pc(); }
        // get to elim free.
        ul_t unset = (K - elim);
        ul_t unset_forward = unset - unset_behind;
        ++elim;
        ul_t skip = elim;
        if (elim <= unset_forward)
        {
            skip += unset_behind;
        }
        else
        {
            skip -= unset_forward;
        }
        skip %= unset;
        if (skip == 0) { skip = unset; }
        unset_behind = skip - 1;

        unsigned p = power_count.size();
        ul_t pos = 0;
        unsigned pi = 0;
        while (p > 0)
        {
            --p;
            const vul_t& pc = power_count[p];
            // for-loop cycle at most one-time
            for (; skip > pc[pi]; skip -= pc[pi++], pos += (1ul << p));
            pi *= 2;
        }

        solution[pos + 1] = elim;
        for (p = 0; p < power_count.size(); ++p, pos /= 2)
        {
            --(power_count[p][pos]);
        }
    }
}

void MouseTrap::print_solution(ostream &fo) const
{
    for (unsigned ii = 0; ii < indices.size(); ++ii)
    {
        unsigned ind = indices[ii];
        fo << " " << solution[ind];
    }
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
        if (getenv("TELLG"))
        {
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg="<<pfi->tellg() << "\n";
        }
        MouseTrap problem(*pfi);
        if (dbg_flags & 0x2) 
        {
            problem.solve_naive();
        }
        else
        {
            problem.solve();
        }
        fout << "Case #"<< ci+1 << ":";
        problem.print_solution(fout);
        fout << "\n";
        fout.flush();

    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
