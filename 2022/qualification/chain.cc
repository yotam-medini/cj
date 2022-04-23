// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <numeric>
#include <queue>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
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
typedef vector<vull_t> vvull_t;

static unsigned dbg_flags;

class Node
{
 public:
    Node(u_t _i=0, ull_t _fun=0,  u_t _papa=0, ull_t _fun_minmax=0) :
        i(_i), fun(_fun), papa(_papa), fun_minmax(_fun_minmax),
        active(true), depth(0)
        {}
    string str() const;
    u_t i; // my index
    ull_t fun;
    u_t papa;
    vu_t children; // sort in decreasing fun_max
    ull_t fun_minmax; // of me and subtree;
    bool active;
    u_t depth;
};
typedef vector<Node> vnode_t;

string Node::str() const
{
    ostringstream os;
    os << "{i="<<i << ", fun="<<fun << ", P="<<papa << ", Cs=[";
    const char *sep = "";
    for (u_t c: children) { os << sep << c; sep = ", "; }
    os << "] fun_minmax=" << fun_minmax << ", D="<<depth << "}";
    return os.str();
}

class Chain
{
 public:
    Chain(istream& fi);
    Chain(const vull_t& _F, const vu_t& _P) : 
        N(_F.size()), F(_F), P(_P), solution(0) {}
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    ull_t get_solution() const { return solution; }
 private:
    void set_pm1();
    void get_initiators();
    ull_t simulate(const vu_t& order, vull_t& take) const;
    void build_nodes_graph();
    void set_nodes_depths();
    void set_minmax();
    void reduce_nodes_fun();
    void print_nodes_tree(ostream& os=cerr) const { print_tree(os, 0, ""); }
    void print_tree(ostream& os, u_t idx, const string& indent) const;
    
    u_t N;
    vull_t F;
    vu_t P;
    vu_t Pm1;
    ull_t solution;
    vvu_t depths_nodes;
    vu_t initiators;
    vu_t roots;
    vnode_t nodes;
};

Chain::Chain(istream& fi) : solution(0)
{
    fi >> N;
    copy_n(istream_iterator<ull_t>(fi), N, back_inserter(F));
    copy_n(istream_iterator<u_t>(fi), N, back_inserter(P));
}

void Chain::solve_naive()
{
    vu_t best_perm;
    vull_t best_take;
    set_pm1();
    get_initiators();
    const u_t n_initiators = initiators.size();
    vu_t perm(size_t(n_initiators), 0);
    iota(perm.begin(), perm.end(), 0);
    for (bool permutate = true; permutate; 
        permutate = next_permutation(perm.begin(), perm.end()))
    {
        vull_t take;
        ull_t perm_fun = simulate(perm, take);
        if (solution < perm_fun)
        {
            solution = perm_fun;
            best_perm = perm;
            best_take = take;
        }
    }
    if (dbg_flags & 0x1) { cerr << "best_perm:";
        for (u_t i = 0; i < best_perm.size(); ++i)
        { 
            cerr << " [" << initiators[best_perm[i]] << "]=" << best_take[i];
        } 
        cerr << '\n';
    }
}

ull_t Chain::simulate(const vu_t& order, vull_t& take) const
{
    ull_t total_fun = 0;
    vull_t active_fun(F);
    for (u_t i: order)
    {
        ull_t fun_max = 0;
        for (u_t m = initiators[i]; m != N; m = Pm1[m])
        {
            fun_max = max(fun_max, active_fun[m]);
            active_fun[m] = 0;
        }
        take.push_back(fun_max);
        total_fun += fun_max;
    }
    return total_fun;
}

void Chain::solve()
{
    typedef pair<ull_t, u_t> funidx_t;
    typedef set<funidx_t, greater<funidx_t>> set_funidx_t;
    set_pm1();
    build_nodes_graph();
    reduce_nodes_fun();
    set_funidx_t funidxs;
    for (const Node& node: nodes)
    {
        funidxs.insert(funidxs.end(), funidx_t(node.fun, node.i));
    }
    const u_t n_initiators = initiators.size();
    for (u_t action = 0; action < n_initiators; ++action)
    {
        ull_t fun_next = 0;
        while (fun_next == 0)
        {
            const funidx_t& funidx = *funidxs.begin();
            u_t fun = funidx.first;
            u_t idx = funidx.second;;
            funidxs.erase(funidxs.begin());
            if (nodes[idx].active)
            {
                nodes[idx].active = false;
                fun_next = fun;
                if (dbg_flags & 0x2) {cerr<<"Take i="<<idx<<", fun="<<fun<<'\n';}
                // need to inactivate a lot ...
                u_t c = idx;
                while (!nodes[c].children.empty())
                {
                    u_t cnext = nodes[c].children.back();
                    nodes[cnext].active = false;
                    nodes[c].children.pop_back();
                    c = cnext;
                }
                // if we are single child of ancestors
                for (u_t papa = nodes[idx].papa;
                    (papa != N) && (nodes[papa].children.size() == 1);
                    papa = nodes[papa].papa)
                {
                    nodes[papa].active = false;
                    nodes[papa].children.clear(); // uneeded?
                }
            }
        }
        solution += fun_next;
    }
}

void Chain::set_pm1()
{
    Pm1.reserve(N);
    for (u_t p: P)
    {
        Pm1.push_back(p > 0 ? p - 1 : N);
    }
}

void Chain::get_initiators()
{
    vector<bool> maybe(N, true);
    for (u_t p: Pm1)
    {
        maybe[p] = false;
    }
    initiators.clear();
    for (u_t i = 0; i < N; ++i)
    {
        if (maybe[i])
        {
            initiators.push_back(i);
        }
    }

}

void Chain::build_nodes_graph()
{
    get_initiators();
    nodes.insert(nodes.end(), N, Node());
    for (u_t i = 0; i < N; ++i)
    {
        Node& node = nodes[i];
        node.i = i;
        node.fun = F[i];
        node.fun_minmax = node.fun;
        u_t v = Pm1[i];
        node.papa = v;
        if (v == N)
        {
            roots.push_back(i);
        }
        else
        {
            nodes[v].children.push_back(i);
        }
    }
#if 0
    for (u_t i: initiators)
    {
        ull_t fun_max = nodes[i].fun_max;
        for (u_t pi = nodes[i].papa; pi != N; pi = nodes[pi].papa)
        {
            fun_max = nodes[pi].fun_max = max(fun_max, nodes[pi].fun_max);
        }
    }
#endif
    set_nodes_depths();
    set_minmax();
    for (Node& node: nodes)
    {
        sort(node.children.begin(), node.children.end(),
            [this](u_t i0, u_t i1) -> bool
            {
                return nodes[i0].fun_minmax > nodes[i1].fun_minmax;
            });
    }
    if (dbg_flags & 0x1) { cerr << __func__<<'\n'; print_nodes_tree(cerr); }
}

void Chain::set_nodes_depths()
{
    depths_nodes.push_back(vu_t()); // 0-depth
    for (u_t root: roots) // just one !?
    {
        queue<u_t> q;
        q.push(root);
        nodes[root].depth = 0;
        depths_nodes[0].push_back(root);
        while (!q.empty())
        {
            const u_t idx = q.front();
            const u_t depth1 = nodes[idx].depth + 1;
            q.pop();
            const vu_t& children = nodes[idx].children;
            if ((!children.empty()) && (depths_nodes.size() <= depth1))
            {
                depths_nodes.push_back(vu_t());
            }
            vu_t& depth_nodes = depths_nodes[depth1];
            for (u_t c: nodes[idx].children)
            {
                q.push(c);
                nodes[c].depth = depth1;
                depth_nodes.push_back(c);
            }
        }
    }
}

void Chain::set_minmax()
{
    for (u_t depth = depths_nodes.size(); depth > 0; )
    {
        --depth;
        for (u_t i: depths_nodes[depth])
        {
            Node& node = nodes[i];
            vu_t& children = node.children;
            if (!children.empty())
            {
                ull_t fun_min = nodes[children[0]].fun_minmax;
                for (u_t c: children)
                {
                    fun_min = min(fun_min, nodes[c].fun_minmax);
                }
                node.fun_minmax = max(node.fun_minmax, fun_min);
            }
        }
    }
    if (dbg_flags & 0x1) { cerr << __func__ << '\n'; print_nodes_tree(); }
}

void Chain::reduce_nodes_fun()
{
    for (u_t root: roots)
    {
        queue<u_t> q;
        q.push(root);
        while (!q.empty())
        {
            u_t idx = q.front();
            Node& node = nodes[idx];
            q.pop();
            vu_t& children = node.children;
            if (!children.empty())
            {
                if (node.fun <= nodes[children.back()].fun)
                {
                    node.fun = 0;
                }
                for (u_t c : node.children)
                {
                    q.push(c);
                }
            }
        }
    }
    if (dbg_flags & 0x1) { cerr << __func__ << '\n'; print_nodes_tree(); }
}

void Chain::print_tree(ostream& os, u_t idx, const string& indent) const
{
    os << indent << nodes[idx].str() << '\n';
    string sub_indent = indent + string("  ");
    for (u_t c: nodes[idx].children)
    {
        print_tree(os, c, sub_indent);
    }
}

void Chain::print_solution(ostream &fo) const
{
    fo << ' ' << solution;
}

static int real_main(int argc, char ** argv)
{
    const string dash("-");

    bool naive = false;
    bool tellg = false;
    u_t solve_ver = 0;
    int rc = 0, ai;

    for (ai = 1; (rc == 0) && (ai < argc) && (argv[ai][0] == '-') &&
        argv[ai][1] != '\0'; ++ai)
    {
        const string opt(argv[ai]);
        if (opt == string("-naive"))
        {
            naive = true;
        }
        else if (opt == string("-solve1"))
        {
            solve_ver = 1;
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

    void (Chain::*psolve)() =
        (naive ? &Chain::solve_naive : &Chain::solve);
    if (solve_ver == 1) { psolve = &Chain::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Chain chain(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (chain.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        chain.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}

static int test_case(const vull_t& F, const vu_t& P)
{
    int rc = 0;
    ull_t solution_naive = ull_t(-1), solution = ull_t(-1);
    const u_t N = F.size();
    {
        Chain chain(F, P);
        chain.solve();
        solution = chain.get_solution();
    }
    if (N < 10)
    {
        Chain chain(F, P);
        chain.solve_naive();
        solution_naive = chain.get_solution();
    }
    if (N < 10)
    {
        if (solution_naive != solution)
        {
            rc = 1;
            cerr << "Solution: naive = "<<solution_naive <<
                " != prod = " << solution << '\n';
            ofstream f("chain-fail.in");
            f << "1\n" << N << "\n";
            const char *sep = "";
            for (ull_t fun: F) { f << sep << fun; sep = " "; }
            f << '\n';
            sep = "";
            for (u_t p: P) { f << sep << p; sep = " "; }
            f << '\n';
            f.close();
        }
    }
    return rc;
}

template <typename T>
T randint(T low, T high)
{
    low += rand() % (high + 1 - low);
    return low;
}

static int test_random(int argc, char ** argv)
{
    int rc = 0;
    int ai = 0;
    u_t n_tests = strtoul(argv[ai++], 0, 0);
    u_t Nmin = strtoul(argv[ai++], 0, 0);
    u_t Nmax = strtoul(argv[ai++], 0, 0);
    u_t Fmin = strtoul(argv[ai++], 0, 0);
    u_t Fmax = strtoul(argv[ai++], 0, 0);
    cerr << "Nmin="<<Nmin << ", Nmax="<<Nmax << 
        ", Fmin="<<Fmin << ", Fmax="<<Fmax <<
        '\n';
    for (u_t ti = 0; (rc == 0) && (ti < n_tests); ++ti)
    {
        cerr << "Tested: " << ti << '/' << n_tests << '\n';
        u_t N = randint(Nmin, Nmax);
        vull_t F; vu_t P;
        for (u_t i = 1; i <= N; ++i)
        {
            F.push_back(randint(Fmin, Fmax));
            P.push_back(randint<u_t>(0, i - 1));
        }
        rc = test_case(F, P);
    }
    return rc;
}

int main(int argc, char **argv)
{
    int rc = ((argc > 1) && (string(argv[1]) == string("test"))
        ? test_random(argc - 2, argv + 2)
        : real_main(argc, argv));
    return rc;
}
