// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <queue>
#include <set>
#include <sstream>
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
typedef set<char> setc_t;
typedef set<vu_t> setvu_t;
typedef queue<vu_t> qvu_t;
typedef pair<vu_t, char> vuc_t;
typedef map<vu_t, vuc_t> vu_to_vuc_t;
typedef set<string> sets_t;
typedef set<u_t> setu_t;

static unsigned dbg_flags;

string vu_to_str(const vu_t& a)
{
    ostringstream os;
    os << '{';
    const char* sep = "";
    for (u_t x: a)
    {
        os << sep << x; sep = ", ";
    }
    os << '}';
    string ret = os.str();
    return ret;
}

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
    u_t n_advance_options(const vu_t& node, u_t pi, char c) const;
    void build_solution();
    // non naive
    bool agree_simple();
    bool agree_begin();
    bool agree_end();
    void add_internals(const string& pat);
    bool contains(const string& simple, const string& aster) const;
    bool contained_increasing(vs_t& vs) const;
    u_t n;
    vs_t p;
    vs_t pat_reduced;
    string solution;
    string solution_head;
    string solution_tail;
    vu_t initial_node;
    vu_t final_node;
    qvu_t q;
    setvu_t visited;
    vu_to_vuc_t parent;
};

Pattern::Pattern(istream& fi)
{
    fi >> n;
    copy_n(istream_iterator<string>(fi), n, back_inserter(p));
}

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
    vu_t skip_idx;
    u_t n_combs = 1;
    for (u_t pi = 0; pi < n; ++pi)
    {
        u_t n_advopt = n_advance_options(node, pi, c);
        n_combs *= n_advopt;
    }
    for (u_t comb = n_combs; (comb > 0) && !finished; )
    {
        u_t rcomb = --comb;
        vu_t next_node(node);
        u_t n_end = 0;
        for (u_t pi = 0; pi != n; ++pi)
        {
            const string& pat = pat_reduced[pi];
            const u_t n_advopt = n_advance_options(node, pi, c);
            u_t adv = 1;
            if (n_advopt > 1)
            {
                adv = (rcomb % n_advopt) + (pat[node[pi]] == '*' ? 0 : 1);
                rcomb /= n_advopt;
            }
            next_node[pi] += adv;
            n_end += (next_node[pi] == pat.size() ? 1 : 0);
        }
        if (((n_end == 0) || (n_end == n)) && visit(node, c, next_node))
        {
            q.push(next_node);
            finished = (next_node == final_node);
        }
    }
    return finished;
}

u_t Pattern::n_advance_options(const vu_t& node, u_t pi, char c) const
{
    const string& pat = pat_reduced[pi];
    u_t ci = node[pi], ci_next = ci + 1;
    u_t n_opt = 0;
    char cnext = (ci_next < pat.size()) ? pat[ci_next] : ' ';
    if (pat[ci] == c)
    {
        n_opt = (cnext == '*') ? 2 : 1;
    }
    else if (pat[ci] == '*')
    {
        n_opt = (cnext == c) ? 3 : 2;
    }
    return n_opt;
}

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

void Pattern::solve()
{
    reduce();
    const string fail("*");
    if (agree_simple() && agree_begin() && agree_end())
    {
        if (solution.empty()) // not set by agree_simple()
        {
            solution = solution_head;
            for (const string& pat: pat_reduced)
            {
                add_internals(pat);
            }
            solution += solution_tail;
        }
    }
    else
    {
        solution = fail;
    }
}

bool Pattern::agree_simple()
{
    vu_t idx_simple, idx_aster;
    for (u_t pi = 0; pi < n; ++pi)
    {
        vu_t* pidx = (pat_reduced[pi].find('*') == string::npos
            ? &idx_simple : &idx_aster);
        pidx->push_back(pi);
    }
    bool agree = true;
    for (u_t pi = 0; agree && (pi < idx_simple.size()); ++pi)
    {
        const string& simple = pat_reduced[idx_simple[pi]];
        for (u_t pj = 0; agree && (pj < idx_aster.size()); ++pj)
        {
            agree = contains(simple, pat_reduced[idx_aster[pj]]);
        }
        if (solution.size() < simple.size())
        {
            solution = simple;
        }
    }
    return agree;
}

void Pattern::add_internals(const string& pat)
{
    size_t pos = pat.find('*') + 1; // must != npos, since no simple 
    size_t apos = pat.find('*', pos);
    while (apos != string::npos)
    {
        size_t sz = apos - pos;
        const string sub(pat, pos, sz);
        solution += sub;
        pos = apos + 1;
        apos = pat.find('*', pos);
    }
}

bool Pattern::contains(const string& simple, const string& aster) const
{
    bool ret = true;
    size_t simple_pos = 0, aster_bpos = 0;
    size_t apos = aster.find('*', aster_bpos);
    bool done = false;
    while (ret && !done)
    {
        done = apos == string::npos;
        u_t sz = (done ? aster.size() : apos) - aster_bpos;
        const string sub(aster, aster_bpos, sz);
        size_t found_pos = simple.find(sub, simple_pos);
        ret = (found_pos != string::npos);
        simple_pos = found_pos + sz;
        aster_bpos = apos + 1;
        apos = aster.find('*', aster_bpos);
    }
    return ret;
}

bool Pattern::agree_begin()
{
    bool agree = true;
    sets_t simple;
    vs_t aster;
    for (u_t i = 0; agree && (i != n); ++i)
    {
        const string& pat = pat_reduced[i];
        if (pat[0] != '*')
        {
            size_t apos = pat.find('*');
            string s = pat;
            if (apos == string::npos)
            {
                simple.insert(simple.end(), pat);
                agree = simple.size() == 1;
            }
            else
            {
                s = string(pat, 0, apos);
                aster.push_back(s);
            }
            if (solution_head.size() < s.size())
            {
                solution_head = s;
            }
        }
    }
    agree = agree && contained_increasing(aster);
    return agree;
}

bool Pattern::agree_end()
{
    bool agree = true;
    sets_t simple;
    vs_t aster;
    for (u_t i = 0; agree && (i != n); ++i)
    {
        const string& pat = pat_reduced[i];
        size_t sz = pat.size();
        if (pat[sz - 1] != '*')
        {
            size_t apos = pat.rfind('*');
            string s = pat;
            if (apos == string::npos)
            {
                simple.insert(simple.end(), pat);
                agree = simple.size() == 1;
            }
            else
            {
                s = string(pat, apos + 1);
                size_t mirror_sz = sz - apos - 1;
                string mirror(pat.crbegin(), pat.crbegin() + mirror_sz);
                aster.push_back(mirror);
            }
            if (solution_tail.size() < s.size())
            {
                solution_tail = s;
            }
        }
    }
    agree = agree && contained_increasing(aster);
    return agree;
}

bool Pattern::contained_increasing(vs_t& vs) const
{
    bool increasing = true;
    if (!vs.empty())
    {
         sort(vs.begin(), vs.end());
         for (u_t i = 1, e = vs.size(); increasing && (i != e); ++i)
         {
              const string& pre = vs[i - 1];
              const string& curr = vs[i];
              increasing = (pre.size() <= curr.size()) &&
                  equal(pre.begin(), pre.end(), curr.begin());
         }
    }
    return increasing;
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
