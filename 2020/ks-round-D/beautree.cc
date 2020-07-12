// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iterator>
#include <string>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
typedef vector<bool> vb_t;

static unsigned dbg_flags;

class BeautyTree
{
 public:
    BeautyTree(istream& fi);
    void solve_naive();
    void solve();
    u_t color(u_t step, u_t node);
    void print_solution(ostream&) const;
 private:
    u_t N, A, B;
    vu_t p1;
    vu_t parent;
    double solution;
    vb_t colored;
};

BeautyTree::BeautyTree(istream& fi) : solution(0.)
{
    fi >> N >> A >> B;
    copy_n(istream_iterator<u_t>(fi), N - 1, back_inserter(p1));
    parent.reserve(N);
    parent.push_back(u_t(-1));
    for (u_t p: p1)
    {
        parent.push_back(p - 1);
    }
}

void BeautyTree::solve_naive()
{
    const ull_t NN = N*N;
    const double dNN = double(NN);
    ull_t n_total_color = 0;
    for (u_t a = 0; a < N; ++a)
    {
        colored.clear();
        colored.insert(colored.end(), N, false);
        u_t nca = color(A, a);
        const vb_t acolored(colored);
        for (u_t b = 0; b < N; ++b)
        {
            colored = acolored;
            u_t ncb = color(B, b);
            ull_t nc = nca + ncb;
            n_total_color += nc;
        }
    }
    solution = double(n_total_color) / dNN;
}

u_t BeautyTree::color(u_t step, u_t node)
{
    u_t nc = 0;
    if (!colored[node])
    {
        colored[node] = true;
        nc = 1;
    }
    while (node != 0)
    {
        u_t s = 0;
        for (; (s < step) && (node != 0); ++s)
        {
            node = parent[node];
        }
        if ((s == step) && !colored[node])
        {
            colored[node] = true;
            ++nc;
        }
    }
    return nc;
}

void BeautyTree::solve()
{
    solve_naive();
}

void BeautyTree::print_solution(ostream &fo) const
{
    ostringstream os;
    os.precision(8);
    os.setf(ios::fixed);
    os << solution;
    string s = os.str();
    while (s.back() == '0')
    {
        s.pop_back();
    }
    fo << ' ' << s;
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

    void (BeautyTree::*psolve)() =
        (naive ? &BeautyTree::solve_naive : &BeautyTree::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        BeautyTree beauTree(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (beauTree.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        beauTree.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
