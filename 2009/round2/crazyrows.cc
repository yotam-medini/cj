// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <fstream>
#include <string>
// #include <set>
#include <map>
#include <queue>
#include <vector>
#include <utility>

#include <cstdlib>
// #include <gmpxx.h>

using namespace std;

// typedef mpz_class mpzc_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
// typedef vector<ul_t> vul_t;
typedef vector<unsigned> vu_t;
typedef vector<int> vi_t;
typedef vector<string> vs_t;

static unsigned dbg_flags;

typedef enum { WHITE, GRAY, BLACK } color_t;

class Node
{
 public:
    Node(unsigned vd=0, color_t vc=WHITE) : d(vd), color(vc) {}
    unsigned d;
    color_t color;
};

typedef map<vi_t, Node> graph_t;
typedef queue<vi_t> qvi_t;

class CrazyRows
{
 public:
    CrazyRows(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    bool is_ll(const vi_t &vi) const; // low-left triangle matrix
    unsigned n;
    vs_t srows;
    vi_t max1pos;
    graph_t graph;
    qvi_t qvi;    
    unsigned solution;
};

CrazyRows::CrazyRows(istream& fi) : n(0), solution(~0)
{
    fi >> n;
    srows.reserve(n);
    max1pos.reserve(n);
    for (unsigned i = 0; i < n; ++i)
    {
        string s;
        fi >> s;
        srows.push_back(s);
        const char *cs = s.c_str();
        int pos = n - 1;
        while ((pos >= 0) && (cs[pos] == '0'))
        {
            --pos;
        }
        max1pos.push_back(pos);
    }
}

void CrazyRows::solve_naive()
{
    graph.insert(graph_t::value_type(max1pos, Node(0, GRAY)));
    qvi.push(max1pos);
    bool found = false;
    while (!found)
    {
        const vi_t& qtop_mtx = qvi.front();
        graph_t::iterator w = graph.find(qtop_mtx);
        Node &node = (*w).second;
        if (is_ll(qtop_mtx))
        {
            found = true;
            solution = node.d;
        }
        else
        {
            unsigned dnext = node.d + 1;
            for (unsigned i = 0, j = 1; j < n; i = j++)
            {
                vi_t vnext(qtop_mtx);
                swap(vnext[i], vnext[j]);
                w = graph.find(vnext);
                if (w == graph.end())
                {
                    graph.insert(graph_t::value_type(vnext, Node(dnext, GRAY)));
                    qvi.push(vnext);
                }
            }
        }
        node.color = BLACK;
        qvi.pop();
    }
}

void CrazyRows::solve()
{
    solve_naive();
}

bool CrazyRows::is_ll(const vi_t &vi) const
{
    bool ret = true;
    for (int j = 0, nn = n; ret && (j < nn); ++j)
    {
        ret = (vi[j] <= j);
    }
    return ret;
}

void CrazyRows::print_solution(ostream &fo) const
{
    fo << " " << solution;
}

int main(int argc, char ** argv)
{
    const string dash("-");

    bool naive = false;
    bool tellg = false;
    int rc = 0, ai;

    for (ai = 1; (rc == 0) && (argv[ai][0] == '-') && argv[ai][1] != '\0'; ++ai)
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

    unsigned n_cases;
    *pfi >> n_cases;

    void (CrazyRows::*psolve)() =
        (naive ? &CrazyRows::solve_naive : &CrazyRows::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        CrazyRows crazy_rows(*pfi);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << ", " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (crazy_rows.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        crazy_rows.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
