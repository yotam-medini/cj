// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iterator>
#include <limits>
#include <string>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
typedef vector<vu_t> vvu_t;
typedef vector<ull_t> vull_t;
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
    void set_children();
    void dfs(vu_t& parents, u_t root);
    u_t N, A, B;
    vu_t p1;
    vu_t parent;
    double solution;
    vb_t colored;
    // non-naive:
    vvu_t child;
    ull_t nA, nB;
    vull_t ab_count[2];
};

BeautyTree::BeautyTree(istream& fi) : solution(0.)
{
    fi >> N >> A >> B;
    if (N > 1)
    {
        copy_n(istream_iterator<u_t>(fi), N - 1, back_inserter(p1));
    }
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
    // solve_naive();
    nA = nB = 0;
    // lcm_ab = (ull_t(A)*ull_t(B)) / ull_t(gcd(A, B));
    child.insert(child.end(), N, vu_t());
    set_children();
    ab_count[0] = ab_count[1] = vull_t(N, 0);
    vu_t parents;
    dfs(parents, 0);
    ull_t num = N;
    num = num * (nA + nB);
    ull_t m = 0;
    for (u_t node = 0; node != N; ++node)
    {
        m += ab_count[0][node] * ab_count[1][node];
    }
    num -= m;
    ull_t denom = ull_t(N) * ull_t(N);
    solution = double(num) / double(denom);
}

void BeautyTree::dfs(vu_t& parents, u_t root)
{
    u_t depth = parents.size();
    nA += (depth / A) + 1;
    nB += (depth / B) + 1;
    parents.push_back(root);
    for (u_t c: child[root])
    {
        dfs(parents, c);
    }
    parents.pop_back();
    for (u_t abi: {0, 1})
    {
        u_t X = (abi == 0 ? A : B);
        vull_t& x_count = ab_count[abi];
        ++x_count[root];
        if (X <= depth)
        {
            u_t grandpa = parents[depth - X];
            ab_count[abi][grandpa] += ab_count[abi][root];
        }
    }
}

void BeautyTree::set_children()
{
    child.insert(child.end(), N, vu_t());
    for (u_t c = 1; c < N; ++c)
    {
        u_t p = parent[c];
        child[p].push_back(c);
    }
    for (u_t p = 0; p < N; ++p)
    {
        vu_t& children = child[p];
        sort(children.begin(), children.end()); // actually not needed
    }
}

string dbl2str(double x)
{
    constexpr auto max_digits10 = numeric_limits<double>::max_digits10;
    ostringstream os;
    os.precision(max_digits10);
    os.setf(ios::fixed);
    os << x;
    string s = os.str();
    while (s.back() == '0')
    {
        s.pop_back();
    }
    if (s.back() == '.')
    {
        s.push_back('0');
    }
    return s;
}

void BeautyTree::print_solution(ostream &fo) const
{
    fo << ' ' << dbl2str(solution);
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
        // getline(*pfi, ignore);
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
