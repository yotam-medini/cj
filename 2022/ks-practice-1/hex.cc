// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <set>
#include <array>
#include <string>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<string> vs_t;
typedef vector<u_t> vu_t;
typedef vector<vu_t> vvu_t;
typedef array<u_t, 2> au2_t;
typedef vector<au2_t> vau2_t;
typedef vector<vau2_t> vvau2_t;

static unsigned dbg_flags;

class Node
{
 public:
    Node(u_t _d, const au2_t& _ij=au2_t{0, 0}) : d(_d), ij(_ij) {}
    u_t d;
    au2_t ij;
};
bool operator<(const Node& node0, const Node& node1)
{
    return node0.d < node1.d;
}
typedef set<Node> setnode_t;


class Hex
{
 public:
    Hex(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    bool count_ok();
    void ij_get_adjs(vau2_t& adjs, const au2_t& ij) const;
    bool is_winner(char color, bool first);
    void init_dist_mat();
    u_t N;
    vs_t board;
    bool possible;
    char winner;
    vvu_t dist_mat;
    u_t nb, nr;
};

Hex::Hex(istream& fi) : possible(true), winner(' '), nb(0), nr(0)
{
    fi >> N;
    copy_n(istream_iterator<string>(fi), N, back_inserter(board));
}

void Hex::solve_naive()
{
    possible = count_ok();
    if (possible)
    {
        dist_mat = vvu_t(size_t(N), vu_t(size_t(N), 0));
        if (is_winner('B', true))
        {
            winner = 'B';
            possible = possible && (nb >= nr);
        }
        else if (possible && is_winner('R', true))
        {
            winner = 'R';
            possible = possible && (nr >= nb);
        }
    }
}

void Hex::solve()
{
    solve_naive();
}

bool Hex::count_ok()
{
    for (const string& row: board)
    {
        for (char c: row)
        {
            if (c == 'B')
            {
                ++nb;
            }
            else if (c == 'R')
            {
                ++nr;
            }
        }
    }
    u_t delta = (nb < nr ? nr - nb : nb - nr);
    bool ok = delta <= 1;
    return ok;
}

bool Hex::is_winner(char color, bool first) // Dijkstra
{
    u_t shortest = N*N; // infinity
    u_t ishort = N; // undef
    vau2_t win_path;
    vvau2_t pre(size_t(N), vau2_t(size_t(N), au2_t{N, N}));
    const u_t run_dim = (color == 'B') ? 1 : 0;
    for (u_t x = 0; x < N; ++x)
    {
        setnode_t q;
        au2_t start{0, 0};
        init_dist_mat();
        start[1 - run_dim] = x;
        if (board[start[0]][start[1]] == color)
        {
            dist_mat[start[0]][start[1]] = 0;
            q.insert(Node(0, start));
        }
        while (!q.empty())
        {
            Node node = *(q.begin());
            q.erase(q.begin());
            if (node.ij[run_dim] == N - 1)
            {
                if (shortest > node.d)
                {
                    shortest = node.d;
                    ishort = x;
                    win_path.clear();
                    win_path.push_back(node.ij);
                    while (win_path.back() != start)
                    {
                        const au2_t& wij = win_path.back();
                        win_path.push_back(pre[wij[0]][wij[1]]);
                    }
                }
            }
            else
            {
                vau2_t adjs;
                ij_get_adjs(adjs, node.ij);
                for (const au2_t& adj: adjs)
                {
                    bool ignore = adj[run_dim] == 0;
                    if ((!ignore) && (board[adj[0]][adj[1]] == color) &&
                       (node.d + 1 < dist_mat[adj[0]][adj[1]]))
                    {
                        dist_mat[adj[0]][adj[1]] = node.d + 1;
                        q.insert(Node(node.d + 1, adj));
                        pre[adj[0]][adj[1]] = node.ij;
                    }
                }
            }
        }
    }
    bool ret = (ishort < N);
    if (ret && first)
    {
        // cancel the shortest winning path
        char lc = (color == 'B' ? 'b' : 'w');
        for (const au2_t& wij: win_path)
        {
            board[wij[0]][wij[1]] = lc;
        }
        bool win_twice = is_winner(color, false);
        possible = possible && !win_twice;
        ret = possible;
    }
    return ret;
}

void Hex::init_dist_mat()
{
    for (vu_t& row: dist_mat)
    {
        fill(row.begin(), row.end(), N*N);
    }
}

void Hex::ij_get_adjs(vau2_t& adjs, const au2_t& ij) const
{
    adjs.clear();
    const u_t i = ij[0], j = ij[1];
    if (j > 0)
    {
        adjs.push_back(au2_t{i, j - 1});
    }
    if (j + 1 < N)
    {
        adjs.push_back(au2_t{i, j + 1});
    }
    if (i > 0)
    {
        adjs.push_back(au2_t{i - 1, j});
        if (j + 1 < N)
        {
            adjs.push_back(au2_t{i - 1, j + 1});
        }
    }
    if (i + 1 < N)
    {
        adjs.push_back(au2_t{i + 1, j});
        if (j > 0)
        {
            adjs.push_back(au2_t{i + 1, j - 1});
        }
    }
}

void Hex::print_solution(ostream &fo) const
{
    if (possible)
    {
        switch (winner)
        {
         case 'B':
             fo << " Blue wins";
             break;
         case 'R':
             fo << " Red wins";
             break;
         default:
             fo << " Nobody wins";
             break;
        }
    }
    else
    {
        fo << " Impossible";
    }
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

    void (Hex::*psolve)() =
        (naive ? &Hex::solve_naive : &Hex::solve);
    if (solve_ver == 1) { psolve = &Hex::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Hex hex(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (hex.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        hex.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}

static int test_specific(int argc, char ** argv)
{
    int rc = 0;
    return rc;
}

static int test_random(int argc, char ** argv)
{
    int rc = 0;
    int ai = 0;
    u_t n_tests = strtoul(argv[ai++], 0, 0);
    for (u_t ti = 0; (rc == 0) && (ti < n_tests); ++ti)
    {
        cerr << "Tested: " << ti << '/' << n_tests << '\n';
    }
    return rc;
}

static int test(int argc, char ** argv)
{
    int rc = ((argc > 1) && (string(argv[1]) == string("specific"))
        ? test_specific(argc - 1, argv + 1)
        : test_random(argc, argv));
    return rc;
}

int main(int argc, char **argv)
{
    int rc = ((argc > 1) && (string(argv[1]) == string("test"))
        ? test(argc - 1, argv + 1)
        : real_main(argc, argv));
    return rc;
}
