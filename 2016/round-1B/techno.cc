// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <string>
#include <set>
#include <list>
#include <map>
#include <vector>
#include <utility>

#include <cstdlib>
// #include <gmpxx.h>

using namespace std;

// typedef mpz_class mpzc_t;
typedef unsigned long ul_t;
typedef vector<unsigned> vu_t;
typedef vector<ul_t> vul_t;

typedef set<string> setstr_t;
typedef set<unsigned> setu_t;
typedef vector<string> vstr_t;
typedef map<unsigned, setu_t> u2setu_t;
typedef map<string, unsigned> s2u_t;

// typedef map<unsigned, vstr_t> u2vs_t;

static unsigned dbg_flags;

class Topic
{
 public:
    string w[2];
};
typedef vector<Topic> vtopic_t;

class I2
{
 public:
    I2(unsigned i0=0, unsigned i1=0) : i{i0, i1} {}
    unsigned i[2];
    string str() const
    {
        ostringstream ostr;
        ostr << "(" << i[0] << ", " << i[1] << ")";
        return ostr.str();
    }
};
typedef vector<I2> vi2_t;
bool operator<(const I2& x, const I2 &y)
{
    return (x.i[0] < y.i[0]) || ((x.i[0] == y.i[0]) && (x.i[1] < y.i[1]));
}
bool operator==(const I2& x, const I2 &y)
{
    return (x.i[0] == y.i[0]) && (x.i[1] == y.i[1]);
}
bool operator!=(const I2& x, const I2 &y)
{
    return !(x == y);
}
typedef set<I2> seti2_t;
typedef map<I2, seti2_t> i2toseti2_t;
typedef map<I2, I2> i2toi2_t;

class DEdge
{
 public:
    DEdge(const I2& from=I2(), const I2& to=I2()) : node{from, to} {}
    I2 node[2];
    string str() const
    {
        ostringstream ostr;
        ostr << "[" << node[0].str() << ", " << node[1].str() << "]";
        return ostr.str();
    }
};
bool operator<(const DEdge& x, const DEdge& y)
{
    return (x.node[0] < y.node[0]) ||
        ((x.node[0] == y.node[0]) && (x.node[1] < y.node[1]));
}

class CF
{
 public:
    CF(int c=0, int f=0) : capacity(c), flow(f) {}
    int capacity;
    int flow;
    string str() const
    {
        ostringstream ostr;
        ostr << "{c:" << capacity << ", f="<<flow << "}";
        return ostr.str();
    }
};

class Techno
{
 public:
    Techno(istream& fi);
    void solve();
    void print_solution(ostream&) const;
 private:
    typedef map<DEdge, CF> e2cf_t;
    void set_flow();
    void add_edge(const I2&, const I2&);
    void ford();
    bool ford_augment();
    void print_nwf(ostream &, unsigned n) const;
    unsigned N;
    unsigned nw[2];
    vtopic_t topics;
    vi2_t itopics;
    I2 source, target;    
    i2toseti2_t graph; 
    e2cf_t network_flow;
    i2toi2_t bfs_path;
    unsigned solution;
};

Techno::Techno(istream& fi) :
    source(2),
    target(3),
    solution(0)
{
    fi >> N;
    topics.reserve(N);
    for (unsigned i = 0; i < N; ++i)
    {
        Topic topic;
        fi >> topic.w[0] >> topic.w[1];
        topics.push_back(topic);
    }
}

void Techno::solve()
{
    set_flow();
    ford();
    unsigned max_match = 0;
    const seti2_t& src_adjs = graph[source];
    for (auto i = src_adjs.begin(); i != src_adjs.end(); ++i)
    {
        const I2 &a = *i;
        DEdge edge(source, a);
        const CF &cf = network_flow[edge];
        max_match += cf.flow;
    }

    unsigned min_orig = nw[0] + nw[1] - max_match;
    solution = topics.size() - min_orig;
}

void Techno::set_flow()
{
    s2u_t ti[2];

    itopics.reserve(topics.size());
    for (auto vti = topics.begin(); vti != topics.end(); ++vti)
    {
        const Topic &topic = *vti;
        unsigned i2[2];
        for (unsigned i01 = 0; i01 != 2; ++i01)
        {
            s2u_t &ti01 = ti[i01];
            const string &w = topic.w[i01];
            auto er = ti01.equal_range(w);
            if (er.first == er.second)
            {
                i2[i01] = ti01.size();
                ti01.insert(er.first, s2u_t::value_type(w, i2[i01]));
            }
            else
            {
                i2[i01] = (*(er.first)).second;
            }
        }
        itopics.push_back(I2(i2[0], i2[1]));
    }
    nw[0] = ti[0].size();
    nw[1] = ti[1].size();

    // Set network_flow
    CF cf_one(1, 0);
    seti2_t adj;
    seti2_t target_single;
    target_single.insert(target_single.end(), target);
    adj.clear();
    for (unsigned i = 0, n = ti[0].size(); i != n; ++i)
    {
        const I2 node(0, i);
        adj.insert(adj.end(), node);
        e2cf_t::value_type v(DEdge (source, node), cf_one);
        network_flow.insert(network_flow.end(), v);
    }
    graph.insert(graph.end(), i2toseti2_t::value_type(source, adj));
    for (unsigned i = 0, n = ti[1].size(); i != n; ++i)
    {
        const I2 node(1, i);
        e2cf_t::value_type v(DEdge (node, target), cf_one);
        graph.insert(graph.end(), i2toseti2_t::value_type(node, target_single));
        network_flow.insert(network_flow.end(), v);
    }
    for (auto i = itopics.begin(); i != itopics.end(); ++i)
    {
        const I2& it = *i;
        I2 from(0, it.i[0]);
        I2 to(1, it.i[1]);
        add_edge(from, to);
        add_edge(to, from);
        e2cf_t::value_type v(DEdge(from, to), CF(1));
        network_flow.insert(network_flow.end(), v);
        e2cf_t::value_type rv(DEdge(to, from), CF(1));
        network_flow.insert(network_flow.end(), rv);
    }
}

void Techno::add_edge(const I2& from, const I2& to)
{
    auto er = graph.equal_range(from);
    i2toseti2_t::iterator gi = er.first;
    if (er.first == er.second)
    {
        i2toseti2_t::value_type v(from, seti2_t());
        gi = graph.insert(gi, v);
    }
    seti2_t &setto = (*gi).second;
    setto.insert(setto.end(), to);
}

static void minby(unsigned &m, unsigned by)
{
    if (m > by)
    {
        m = by;
    }
}

void Techno::ford()
{
    unsigned aug_count = 0;
    if (dbg_flags & 0x1) { print_nwf(cerr, 0); }
    while (ford_augment())
    {
        ++aug_count;
        unsigned min_aug = ~0;
        for (unsigned dry_wet = 0; dry_wet != 2; ++dry_wet)
        {
            for (I2 node = target, prev = bfs_path[target];
                node != source; node = prev, prev = bfs_path[prev])
            {
                const DEdge edge(prev, node);
                CF &cf = network_flow[edge];
                if (dry_wet == 0)
                {
                    minby(min_aug, cf.capacity - cf.flow);
                }
                else
                {
                    cf.flow += min_aug;
                    const DEdge rev_edge(node, prev);
                    auto i = network_flow.find(rev_edge);
                    if (i != network_flow.end())
                    {
                        CF& rev_cf = (*i).second;
                        rev_cf.flow -= min_aug;
                    }
                }
            }
        }
        if (dbg_flags & 0x1) { print_nwf(cerr, aug_count); }
    }
}

bool Techno::ford_augment() // BFS
{
    bool ret = false;
    list<I2> queue;
    set<I2> visited;

    queue.push_back(source);
    visited.insert(visited.end(), source);

    bfs_path.clear();

    while (!queue.empty() && !ret)
    {
        I2 node = queue.front();
        queue.pop_front();

        const seti2_t &adj = graph[node];
        for (auto i = adj.begin(); !ret && (i != adj.end()); ++i)
        {
            const I2 &a = *i;
            auto er = visited.equal_range(a);
            if (er.first == er.second)
            {
                const DEdge edge(node, a);
                // const CF &cf = network_flow[edge];
                auto nfi = network_flow.find(edge);
                if (nfi == network_flow.end())
                {
                    cerr << "ERROR in line " << __LINE__ << "\n";
                }
                const CF &cf = (*nfi).second;
                if (cf.flow < cf.capacity)
                {
                    queue.push_back(a);
                    visited.insert(er.first, a);
                    i2toi2_t::value_type v(a, node);
                    bfs_path.insert(bfs_path.end(), v);
                    ret = (a == target);
                }
            }
        }
    }

    return ret;
}

void Techno::print_nwf(ostream& ef, unsigned n) const
{
    ef << "{ Network-Flow: (n=" << n << "}\n";
    for (auto i = network_flow.begin(); i != network_flow.end(); ++i)
    {
         const e2cf_t::value_type& v = *i;
         const DEdge& edge = v.first;
         const CF &cf = v.second;
         ef << "  " << edge.str() << ": " << cf.str() << "\n";
    }
    ef << "}\n";
}

void Techno::print_solution(ostream &fo) const
{
    fo << " " << solution;
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

    if (argc > 3) { dbg_flags = strtoul(argv[3], 0, 0);
        cerr << "dbg_flags=" << hex << dbg_flags << "\n"; }
    // bool large = (argc > 4) && !strcmp(argv[4], "-large");

    unsigned n_cases;
    *pfi >> n_cases;

    ostream &fout = *pfo;
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Techno problem(*pfi);
        // cerr << "Case ci="<<ci << " (ci+1)="<<ci+1 << "\n";
        problem.solve();
        fout << "Case #"<< ci+1 << ":";
        problem.print_solution(fout);
        fout << "\n";
        fout.flush();

    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
