// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <queue>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<string> vs_t;;
typedef vector<u_t> vu_t;;
typedef set<char> setc_t;;
typedef set<u_t> setu_t;;
typedef set<vu_t> setvu_t;;
typedef queue<setu_t> qsetu_t;;
typedef queue<vu_t> qvu_t;;
typedef map<vu_t, char> vutoc_t;
typedef pair<vu_t, char> vuc_t;
typedef map<vu_t, vuc_t> vu_to_vuc_t;

static unsigned dbg_flags;

class Node
{
 public:
    Node(char _c=' ', u_t _next=0, bool _self=false):
        c(_c), next(_next), self(_self)
        {}
    char c;
    u_t next;
    bool self;
};
typedef vector<Node> vnode_t;
typedef vector<vnode_t> vvnode_t;

class NonDetNode
{
 public:
    NonDetNode(char _c=' ', u_t _next=0): c(_c), next(_next)
        {}
    char c;
    u_t next;
};
typedef vector<NonDetNode> vnondetnode_t;

class Pattern
{
 public:
    Pattern(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    void reduce();
    bool q_visits(const vu_t& node, char c);
    bool visit(const vu_t& parent, char c, const vu_t& child);
    void build_solution();
    // void build_pautomata();
    // void build_automata();
    void next_bump(vu_t& next_node, u_t pi) const;
    bool alt_get(char& cnext, u_t pi, u_t ci) const;
    u_t n;
    vs_t p;
    vs_t pat_reduced;
    string solution;
    vu_t initial_node;
    vu_t final_node;
    qvu_t q;
    // setu_t initial_node;
    // setu_t final_node;
    // vvnode_t pautomata;
    // vnondetnode_t nd_automata; // non-deterministic
    // vu_t initial_nodes;
    // vu_t final_nodes;
    // setu_t initial_node;
    // setu_t final_node;
    setvu_t visited;
    vu_to_vuc_t parent;
};

Pattern::Pattern(istream& fi)
{
    fi >> n;
    copy_n(istream_iterator<string>(fi), n, back_inserter(p));
}

#if 0
void Pattern::solve_naive()
{
    bool possible = true, end = false;
    vu_t idx{n, 0};
    while (possible && !end)
    {
        char next = ' ';
        bool progress = false;
        for (u_t pi = 0; pi < n; ++pi)
        {
            char c = p[pi][idx[pi]];
            if (c != '*')
            {
                if (next == ' ')
                {
                    solution.push_back(c);
                    progress = true;
                }
                else
                {
                    possible = possible && (c == solution.back());
                }
            }
        }
        if (!progress)
        {
            // skip asterisks
            for (u_t pi = 0; pi < n; ++pi)
            {
                u_t ci = idx[pi];
                const string& pi = p[pi];
                for (; ci < 
            }
            
        }
    }
}
#endif

void Pattern::solve_naive()
{
    reduce();
    // const vu_t initial(size_t(n), 0);
    initial_node = vu_t(size_t(n), 0);
    visited.insert(visited.end(), initial_node);
    final_node.reserve(n);
    for (const string& pat: pat_reduced)
    {
        final_node.push_back(pat.size());
    }
    q.push(initial_node);
    bool possible = true, finished = false;

    while ((!q.empty()) && possible && !finished)
    {
        const vu_t node = q.front();
        q.pop();
        setc_t cs, alt;
        for (u_t pi = 0; pi < n; ++pi)
        {
            const string& pat = pat_reduced[pi];
            char c = pat[node[pi]];
            if (c == '*')
            {
                u_t ci_next = node[pi] + 1;
                if (ci_next < pat.size())
                {
                    alt.insert(alt.end(), pat[ci_next]);
                }
            }
            else  // above pre-elase not needed !?
            {
                cs.insert(cs.end(), c);
            }
        }
        if (cs.size() < 2)
        {
            setc_t calt(alt);
            if (!cs.empty())
            {
                calt.clear();
                calt.insert(calt.end(), *cs.begin());
            }
            if (calt.empty())
            {
                visit(node, '*', final_node);
                finished = true;
            }
            else
            {
                for (char c: calt)
                {
                    finished = finished || q_visits(node, c);
                }
            }
        }
    }
    build_solution();
}

void Pattern::reduce()
{
    pat_reduced.reserve(n);
    for (const string& pat: p)
    {
        string r;
        for (char c: pat)
        {
            if ((c != '*') || r.empty() || (r.back() != '*'))
            {
                r.push_back(c);
            }
        }
        pat_reduced.push_back(r);
    }
}

bool Pattern::q_visits(const vu_t& node, char c)
{
    bool finished = false;
    vu_t alt_idx;
    for (u_t pi = 0; pi < n; ++pi)
    {
        const string& pat = pat_reduced[pi];
        u_t ci_next = node[pi] + 1;
        if ((pat[node[pi]] == '*') && (ci_next < pat.size()) &&
            (pat[ci_next] == c))
        {
            alt_idx.push_back(pi);
        }
    }
    const u_t n_alt = alt_idx.size();
    const u_t alt_max = 1u << n_alt;
    for (u_t alt_mask = alt_max; (alt_mask > 0) && !finished; )
    {
        --alt_mask;
        vu_t next_node(node);
        u_t n_end = 0;
        for (u_t pi = 0, alti = 0; pi != n; ++pi)
        {
            const string& pat = pat_reduced[pi];
            u_t& nextpi = next_node[pi];
            if ((alti < n_alt) && (alt_idx[alti] == pi))
            {
                if ((alt_mask & (1u << alti)) != 0)
                {
                    nextpi += 2;
                }
                ++alti;
            }
            else if (pat[nextpi] == c)
            {
                ++nextpi;
            }
            n_end += (nextpi == pat.size() ? 1 : 0);
        }
        if (((n_end == 0) || (n_end == n)) && visit(node, c, next_node))
        {
            q.push(next_node);
            finished = (next_node == final_node);
        }
    }
    return finished;
}

void Pattern::next_bump(vu_t& next_node, u_t pi) const
{
    if (next_node[pi] < pat_reduced[pi].size())
    {
        ++next_node[pi];
    }
}

#if 0
bool Pattern::alt_get(char& cnext, u_t pi, u_t ci) const
{
    const string& pat = pat_reduced[pi];
    bool alt = (pat[ci] == '*') && ((ci + 1) < pat.size());
    if (alt)
    {
        cnext = pat[ci + 1];
    }
    return alt;
}
#endif

bool Pattern::visit(const vu_t& pnode, char c, const vu_t& child)
{
    auto er = visited.equal_range(child);
    bool added = (er.first == er.second);
    if (added)
    {
        visited.insert(er.first, child);
        vuc_t pc{pnode, c};
        vu_to_vuc_t::value_type v{child, pc};
        parent.insert(parent.end(), v);
    }
    return added;
}

void Pattern::build_solution()
{
    vu_to_vuc_t::const_iterator iter = parent.find(final_node);
    if (iter == parent.end())
    {
        solution = string("*");
    }
    else
    {
        while (iter != parent.end())
        {
            solution.push_back(iter->second.second);
            iter = parent.find(iter->second.first);
        }
        reverse(solution.begin(), solution.end());
    }
}

#if 0
void Pattern::solve()
{
    solve_naive();
    build_pautomata();
    build_automata();
}
#endif

#if 0
void Pattern::build_pautomata()
{
    initial_nodes.reserve(n);
    final_nodes.reserve(n);
    pautomata.reserve(n);
    u_t nn = 0;
    for (const string& pat: p)
    {
        initial_nodes.push_back(nn);
        vnode_t automata;
        for (u_t ci = 0, ce = pat.size(); ci < ce; )
        {
            char c = pat[ci];
            bool self = (c == '*');
            if (self)
            {
                for (; (ci + 1 < ce) && (pat[ci] == '*'); ++ci) {}
            }
            if (ci + 1 < ce)
            {
                ++ci;
                ++n;
                c = pat[ci];
            }
            automata.push_back(Node(c, nn, self));
        }
        final_nodes.push_back(automata.back().next);
        pautomata.push_back(automata);
    }
}

void Pattern::build_automata()
{
    // initial_node = setu_t{initial_nodes.begin(), initial_nodes.end()};
    qvu_t q;
    q.push(initial_nodes);
    
    setvu_t visited;
    possible = true;
    while (!q.empty() && possible)
    {
        vu_t node = q.front();
        q.pop();
        visited.insert(visited.end() node);
        setc_t cs;
        u_t nc = 0;
        vu_t alts;
        for (u_t pi = 0; pi != n; ++pi)
        {
            
            if (pnode.c != '*')
            {
                cs.insert(cs.end(), c)
                if (pnode.self)
                {
                    ++n_alt;
                }
                else
                {
                    ++nc;
                }
            }
        }
        possible = possible && (nc <= 1);
        for (char c: cs)
        {
            for (u_t pi = 0; pi < n; ++pi)
            {
                
            }
        }
    }
    
}
#endif

void Pattern::solve()
{
    solve_naive();
}

void Pattern::print_solution(ostream &fo) const
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

    void (Pattern::*psolve)() =
        (naive ? &Pattern::solve_naive : &Pattern::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Pattern pattern(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (pattern.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        pattern.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
