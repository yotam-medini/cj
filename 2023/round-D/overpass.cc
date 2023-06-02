// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include <cstdlib>
#include <cstdio>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
typedef vector<vu_t> vvu_t;
typedef vector<double> vd_t;
typedef array<u_t, 2> au2_t;
typedef vector<au2_t> vau2_t;

static unsigned dbg_flags;

string dtos(const double x, u_t precision=6)
{
    char buf[0x20];
    snprintf(buf, sizeof(buf), "%.*f", precision, x);
    string ret(buf);
    while ((ret.back() == '0') && (ret[ret.size() - 2] != '.'))
    {
        ret.pop_back();
    }
    return ret;
}

class Node
{
 public:
    Node() :
        sum_paths(0),
        descendants(0),
        initial_contributers(0),
        pending_contributers(0) {}
    vu_t adjs;
    ull_t sum_paths;
    ull_t descendants;
    u_t initial_contributers; // except single parent paren
    u_t pending_contributers; // except single parent paren
};
typedef vector<Node> vnode_t;

class Overpass
{
 public:
    Overpass(istream& fi);
    Overpass(const vu_t&) {}; // TBD for test_case
    Overpass(const vu_t& _X, const vu_t& _F, const vau2_t& _AB) :
        W(_X.size()),
        E(_F.size()),
        C(_AB.size()),
        X(_X), F(_F), AB(_AB)
        {};
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    const vd_t& get_solution() const { return solution; }
 private:
    // naive
    void build_we_vadjs();
    ull_t get_sum_paths(
        u_t& n_descendants,
        const vvu_t& vadjs,
        u_t pre_root,
        u_t root) const;
    // non-naive
    void build_we_nodes();
    void build_nodes(vnode_t& nodes, u_t n, const vu_t& conns);

    ull_t W, E;
    u_t C;
    vu_t X, F;
    vau2_t AB;
    vd_t solution;
    // naive
    vvu_t we_vadjs[2];
    // non naive
    vnode_t we_nodes[2];
};

Overpass::Overpass(istream& fi)
{
    fi >> W >> E >> C;
    X.reserve(W);
    F.reserve(E);
    X.push_back(0); // unused
    F.push_back(0); // unused
    copy_n(istream_iterator<u_t>(fi), W - 1, back_inserter(X));
    copy_n(istream_iterator<u_t>(fi), E - 1, back_inserter(F));
    AB.reserve(C);
    while (AB.size() < C)
    {
        u_t a, b;
        fi >> a >> b;
        AB.push_back(au2_t{a, b});
    }
}

void Overpass::solve_naive()
{
    build_we_vadjs();
    u_t n_dummy;
    ull_t we_internal_paths[2];
    for (u_t wei: {0, 1})
    {
        const vvu_t& v_adjs = we_vadjs[wei];
        ull_t& internal_paths = we_internal_paths[wei];
        internal_paths = 0;
        const size_t sz = v_adjs.size();
        for (u_t r = 1; r < sz; ++r)
        {
            ull_t r_sum_paths = get_sum_paths(n_dummy, v_adjs, 0, r);
            internal_paths += r_sum_paths;
        }
        internal_paths /= 2; // since each path was counter twice
        if (dbg_flags& 0x2) { cerr << "internal_paths["<<wei<<"]=" << 
            internal_paths << '\n'; }
    }
    for (const au2_t& ab: AB)
    {
        const ull_t w_sum_paths = get_sum_paths(n_dummy, we_vadjs[0], 0, ab[0]);
        const ull_t e_sum_paths = get_sum_paths(n_dummy, we_vadjs[1], 0, ab[1]);
        if (dbg_flags & 0x2) { cerr << "ab=("<<ab[0]<<", "<<ab[1]<<") wpath=" <<
            w_sum_paths << " epath=" << e_sum_paths << '\n'; }
        const ull_t cross_paths = E*w_sum_paths + W*e_sum_paths + W*E*1;
        const ull_t total =
            we_internal_paths[0] + we_internal_paths[1] + cross_paths;
        const ull_t n_paths = (W*(W - 1))/2 + (E*(E - 1))/2 + W*E;
        const double average = double(total) / double(n_paths);
        solution.push_back(average);
    }
}

void Overpass::build_we_vadjs()
{
    for (u_t wei: {0, 1})
    {
        const u_t n = (wei == 0 ? W : E);
        const vu_t& parents = (wei == 0 ? X : F);
        vvu_t& vadjs =  we_vadjs[wei];
        vadjs.assign(n + 1, vu_t());
        for (u_t c = 1; c < n; ++c)
        {
            vadjs[parents[c]].push_back(c);
        }
        for (u_t c = 1; c < n; ++c)
        {
            vadjs[c].push_back(parents[c]);
        }
    }
}

ull_t Overpass::get_sum_paths(
    u_t& n_descendants,
    const vvu_t& vadjs,
    u_t pre_root,
    u_t root) const
{
    ull_t sum_paths = 0;
    n_descendants = 0;
    for (u_t c: vadjs[root])
    {
        if (c != pre_root)
        {
            u_t n_sub_desc = 0;
            ull_t sum_sub_paths = get_sum_paths(n_sub_desc, vadjs, root, c);
            n_descendants += n_sub_desc + 1;
            sum_paths += sum_sub_paths + n_sub_desc + 1;
        }
    }
    return sum_paths;
}

void Overpass::solve()
{
    build_we_nodes();
    ull_t we_internal_paths[2];
    for (u_t wei: {0, 1})
    {
        const vnode_t& nodes = we_nodes[wei];
        ull_t& internal_paths = we_internal_paths[wei];
        internal_paths = 0;
        for (u_t r = 1; r < we_nodes[wei].size(); ++r)
        {
            internal_paths += ull_t(nodes[r].sum_paths);
        }
        internal_paths /= 2; // since each path was counter twice
        if (dbg_flags& 0x2) { cerr << "internal_paths["<<wei<<"]=" << 
            internal_paths << '\n'; }
    }
    for (const au2_t& ab: AB)
    {
        const ull_t w_sum_paths = we_nodes[0][ab[0]].sum_paths;
        const ull_t e_sum_paths = we_nodes[1][ab[1]].sum_paths;
        if (dbg_flags & 0x2) { cerr << "ab=("<<ab[0]<<", "<<ab[1]<<") wpath=" <<
            w_sum_paths << " epath=" << e_sum_paths << '\n'; }
        const ull_t cross_paths = E*w_sum_paths + W*e_sum_paths + W*E*1;
        const ull_t total =
            we_internal_paths[0] + we_internal_paths[1] + cross_paths;
        const ull_t n_paths = (W*(W - 1))/2 + (E*(E - 1))/2 + W*E;
        const double average = double(total) / double(n_paths);
        solution.push_back(average);
    }
}

void Overpass::build_we_nodes()
{
    build_nodes(we_nodes[0], W, X);
    build_nodes(we_nodes[1], E, F);
}

void Overpass::build_nodes(vnode_t& nodes, u_t n, const vu_t& conns)
{
    nodes.assign(n + 1, Node());
    for (u_t i = 1; i <= n - 1; ++i)
    {
        const u_t con = conns[i];
        ++nodes[con].initial_contributers;
    }
    vu_t zero_pending_nodes;
    for (u_t i = 1; i <= n; ++i)
    {
        nodes[i].pending_contributers = nodes[i].initial_contributers;
        if (nodes[i].pending_contributers == 0)
        {
            zero_pending_nodes.push_back(i);
        }
    }
    vu_t topo_order; topo_order.reserve(n);
    while (!zero_pending_nodes.empty())
    {
        vu_t zero_pending_nodes_next;
        for (u_t inode: zero_pending_nodes)
        {
            topo_order.push_back(inode);
            if (inode < n)
            {
                const u_t con = conns[inode];
                const ull_t descendants = nodes[inode].descendants + 1;
                nodes[con].sum_paths += nodes[inode].sum_paths + descendants;
                nodes[con].descendants += descendants;
                if (--nodes[con].pending_contributers == 0)
                {
                    zero_pending_nodes_next.push_back(con);
                }
            }
        }
        swap(zero_pending_nodes, zero_pending_nodes_next);
    }
    if (topo_order.back() != n) {
        cerr << __FILE__ ":" << __LINE__ << ": ERROR\n";
    }
    // topo_order.pop_back(); // has no "parent"
    for (int i = topo_order.size() - 2; i >= 0; --i)
    {
        u_t inode = topo_order[i];
        Node& node = nodes[inode];
        const ull_t pre_contribution = node.sum_paths + node.descendants + 1;
        const Node& conn = nodes[conns[inode]];
        const ull_t sum_paths_add = (conn.sum_paths - pre_contribution) +
            (n - 1 - node.descendants);
        node.sum_paths += sum_paths_add;
    }
    if (dbg_flags & 0x1) { cerr << "sum_paths:";
        for (u_t i = 1; i <= n; ++i) {
            cerr << " ["<<i<<"]="<< nodes[i].sum_paths; } cerr << '\n'; }
}

void Overpass::print_solution(ostream &fo) const
{
    for (double x: solution) { fo << ' ' << dtos(x, 8); }
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
            dbg_flags = strtoul(argv[++ai], nullptr, 0);
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

    void (Overpass::*psolve)() =
        (naive ? &Overpass::solve_naive : &Overpass::solve);
    if (solve_ver == 1) { psolve = &Overpass::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Overpass overpass(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (overpass.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        overpass.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}

static u_t rand_range(u_t nmin, u_t nmax)
{
    u_t r = nmin + rand() % (nmax + 1 - nmin);
    return r;
}

static void save_case(
   const char* fn,
   const vu_t& X,
   const vu_t& F, 
   const vau2_t& AB)
{
    ofstream f(fn);
    const u_t W = X.size(), E = F.size(), C = AB.size();
    f << "1\n" << W << ' ' << E << ' ' << C;
    const char* sep = "\n";
    for (size_t i = 1; i < X.size(); ++i) { f << sep << X[i]; sep = " "; }
    sep = "\n";
    for (size_t i = 1; i < F.size(); ++i) { f << sep << F[i]; sep = " "; }
    f << '\n';
    for (const au2_t& ab: AB)
    {
        f << ab[0] << ' ' << ab[1] << '\n';
    }
    f.close();
}

static int test_case(const vu_t& X,  const vu_t& F, const vau2_t& AB)
{
    int rc = 0;
    vd_t solution, solution_naive;
    const u_t W = X.size() - 1, E = F.size() - 1, C = AB.size();
    bool small = (W <= 10) && (E <= 10) && (C <= 10);
    if (dbg_flags & 0x100) { save_case("overpass-curr.in", X, F, AB); }
    if (small)
    {
        Overpass p(X, F, AB);
        p.solve_naive();
        solution_naive = p.get_solution();
    }
    {
        Overpass p(X, F, AB);
        p.solve();
        solution = p.get_solution();
    }
    if (small && (solution != solution_naive))
    {
        rc = 1;
        cerr << "Failed: solution != solution_naive\n";
        save_case("overpass-fail.in", X, F, AB);
    }
    if (rc == 0) { cerr << " W="<<W << ", E="<<E << ", C="<<C << " ..." <<
        (small ? " (small) " : " (large) ") << " --> ";
        for (u_t i = 0; i < min<u_t>(2, C); ++i) {
            cerr << ' ' << solution[i]; }
        cerr << (C > 2 ? " ..." : "") << '\n'; }
    return rc;
}

static int test_random(int argc, char ** argv)
{
    int rc = 0;
    int ai = 0;
    if (string(argv[ai]) == string("-debug"))
    {
        dbg_flags = strtoul(argv[ai + 1], nullptr, 0);
        ai += 2;
    }
    const u_t n_tests = strtoul(argv[ai++], nullptr, 0);
    const u_t Wmin = strtoul(argv[ai++], nullptr, 0);
    const u_t Wmax = strtoul(argv[ai++], nullptr, 0);
    const u_t Emin = strtoul(argv[ai++], nullptr, 0);
    const u_t Emax = strtoul(argv[ai++], nullptr, 0);
    const u_t Cmin = strtoul(argv[ai++], nullptr, 0);
    const u_t Cmax = strtoul(argv[ai++], nullptr, 0);
    cerr << "n_tests=" << n_tests <<
        ", Wmin=" << Wmin << ", Wmax=" << Wmax <<
        ", Emin=" << Emin << ", Emax=" << Emax <<
        ", Cmin=" << Cmin << ", Cmax=" << Cmax <<
        '\n';
    for (u_t ti = 0; (rc == 0) && (ti < n_tests); ++ti)
    {
        cerr << "Tested: " << ti << '/' << n_tests << '\n';
        const u_t W = rand_range(Wmin, Wmax);
        const u_t E = rand_range(Emin, Emax);
        const u_t C = min(rand_range(Cmin, Cmax), W*E);
        vu_t X, F; X.reserve(W); F.reserve(E);
        X.push_back(0);
        F.push_back(0);
        while (X.size() < W)
        {
            X.push_back(rand_range(X.size() + 1, W));
        }
        while (F.size() < E)
        {
            F.push_back(rand_range(F.size() + 1, E));
        }
        vau2_t AB; AB.reserve(C);
        while (AB.size() < C)
        {
            const au2_t ab{rand_range(1, W), rand_range(1, E)};
            AB.push_back(ab);
        }
        rc = test_case(X, F, AB);
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
