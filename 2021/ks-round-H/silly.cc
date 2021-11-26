// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

// #include <algorithm>
#include <fstream>
#include <iostream>
#include <list>
#include <string>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;

typedef list<u_t> lu_t;
typedef list<u_t>::iterator luiter_t;
typedef list<u_t>::const_iterator luciter_t;

static unsigned dbg_flags;

class Node;
typedef list<Node>::iterator lnode_iter_t;
typedef list<lnode_iter_t> llnditr_t;
typedef llnditr_t::iterator llnditritr_t;
typedef llnditr_t::const_iterator llnditrcitr_t;

llnditr_t llnditr_empty;
const llnditritr_t llnditr_empty_end = llnditr_empty.end();

class Node
{
 public:
    Node(u_t _v=0) : v(_v), ref(llnditr_empty_end) {}
    u_t v;
    llnditritr_t ref;
    bool is_ref() const { return ref != llnditr_empty_end; }
    void unref() { ref = llnditr_empty_end; }
};
typedef list<Node> lnode_t;
typedef lnode_t::iterator iter_t;
typedef lnode_t::const_iterator citer_t;
typedef vector<llnditr_t> vllnditr_t;

template <typename T>
typename list<T>::iterator liter_next(
    list<T>& l,
    typename list<T>::iterator iter
)
{
    if (iter != l.end())
    {
        ++iter;
    }
    return iter;
}

class Silly
{
 public:
    Silly(istream& fi);
    Silly(u_t _N, const string& _S, u_t _base=10) : N(_N), S(_S), base(_base) {}
    void solve_naive();
    void solve1();
    void solve();
    void print_solution(ostream&) const;
    const string& get_solution() const { return solution; }
 private:
    bool succ(u_t n, u_t n1)
    {
        bool ret = ((n + 1) % base) == n1;
        return ret;
    }
    void s2l();
    string& l2s(string& s) const;
    void dbg_showx() const
    {
        string s;
        if (dbg_flags & 0x1) { cerr << ".. " << l2s(s) << '\n'; }
    }
    void dbg_show_lists() const;
    iter_t iter_prev(iter_t iter)
    {
        if (iter != lx.begin())
        {
            --iter;
        }
        return iter;
    }
    iter_t iter_next(iter_t iter)
    {
        iter = liter_next<Node>(lx, iter);
        return iter;
    }
    llnditritr_t insert_end(llnditr_t& l, iter_t iter)
    {
        return l.insert(l.end(), iter);
    }
    u_t N;
    string S;
    u_t base;
    string solution;
    lnode_t lx;
    vllnditr_t vsuccs;
};

Silly::Silly(istream& fi) : base(10)
{
    fi >> N >> S;
}

void Silly::solve_naive()
{
    string x = S;
    if (dbg_flags & 0x2) { cerr << ".. " << x << '\n'; }
    for (bool change = true; change; )
    {
        const string x0(x);
        for (u_t n = 0; n < base; ++n)
        {
            u_t n1 = (n + 1) % base;
            u_t n2 = (n + 2) % base;
            char sfrom[2]; 
            sfrom[0] = ('0' + n);
            sfrom[1] = ('0' + n1);
            char cto = '0' + n2;
            for (size_t i = 0; i + 1 < x.size(); ++i)
            {
                if ((x[i] == sfrom[0]) && (x[i + 1] == sfrom[1]))
                {
                    x[i] = cto;
                    for (size_t j = i + 1, j1 = j + 1; j1 < x.size(); j = j1++)
                    {
                        x[j] = x[j + 1];
                    }
                    x.pop_back();
                    if (dbg_flags & 0x2) { cerr << ".. " << x << '\n'; }
                }
            }
        }
        change = (x != x0);
    }
    solution = x;
}


void Silly::s2l()
{
    lx.clear();
    for (char c: S)
    {
        lx.push_back(Node(c - '0'));
    }
}

string& Silly::l2s(string& s) const
{
    s.clear();
    for (const Node& node: lx)
    {
        s.push_back('0' + node.v);
    }
    return s;
}

void Silly::solve()
{
    s2l();
    dbg_showx();
    u_t n_succ = 0;    
    vsuccs = vllnditr_t(size_t(base), llnditr_t());    
    for (iter_t iter = lx.begin(), iter1 = iter_next(iter); iter1 != lx.end();
        iter = iter1++)
    {
        u_t v = iter->v, v1 = iter1->v;
        if ((v + 1) % base == v1)
        {
            llnditr_t& lsucc = vsuccs[v];
            llnditritr_t lliter = insert_end(lsucc, iter);
            iter->ref = lliter;
            ++n_succ;
        }
    }
    if (dbg_flags & 0x4) { dbg_show_lists(); }

    while (n_succ > 0)
    {
        if (dbg_flags & 0x1) { cerr << "n_succ="<<n_succ << '\n'; }
        for (u_t u0 = 0; (u0 < base) && (n_succ > 0); ++u0)
        {
            if (dbg_flags & 0x1) { cerr << "u0="<<u0 << '\n'; }
            const u_t u1 = (u0 + 1) % base;
            const u_t u2 = (u0 + 2) % base;
            llnditr_t& lsucc = vsuccs[u0];
            for (llnditritr_t liter = lsucc.begin(); liter != lsucc.end();
                ++liter)
            {
                iter_t iter = *liter;
                Node& node = *iter;
                node.v = u2;
                node.unref();
                iter_t iter1 = iter_next(iter);
                Node& node1 = *iter1;
                if (node1.is_ref())
                {
                    u_t v1 = node1.v;
                    vsuccs[v1].erase(iter1->ref);
                    --n_succ;
                }
                lx.erase(iter1);
                --n_succ;
                dbg_showx();
                iter1 = iter_next(iter);
                if ((iter1 != lx.end()) && (((iter->v + 1) % base) == iter1->v))
                {
                    iter->ref = insert_end(vsuccs[u2], iter);
                    ++n_succ;
                }
                if (iter != lx.begin())
                {
                    iter_t iter0 = iter; --iter0;
                    if (iter0->is_ref())
                    {
                        vsuccs[iter0->v].erase(iter1->ref);
                        iter0->unref();
                        --n_succ;
                    }
                    if (iter0->v == u1)
                    {
                        iter0->ref = insert_end(vsuccs[u1], iter0);
                        ++n_succ;
                    }
                }                
            }
            lsucc.clear();
            if (dbg_flags & 0x4) { dbg_show_lists(); }
        }
    }
    dbg_showx();
    l2s(solution);
}

void Silly::dbg_show_lists() const
{
    for (citer_t iter = lx.begin(); iter != lx.end(); ++iter)
    {
        const Node& node = *iter;
        cerr << ' ' << node.v << (node.is_ref() ? '+' : ' ');
    }
    cerr << "\n{ Lists\n";
    for (u_t u = 0; u < base; ++u)
    {
        cerr << "  [" << u << "]:";
        const llnditr_t& lsucc = vsuccs[u];
        for (llnditrcitr_t liter = lsucc.begin(); liter != lsucc.end();
            ++liter)
        {
            cerr << ' ' << (*liter)->v;
        }
        cerr << '\n';
    }
    cerr << "} Lists\n";
}

void Silly::print_solution(ostream &fo) const
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

    void (Silly::*psolve)() =
        (naive ? &Silly::solve_naive : &Silly::solve);
    if (solve_ver == 1) { psolve = &Silly::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Silly silly(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (silly.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        silly.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}

static int test_case(u_t N, const string& S, u_t base)
{
    int rc = 0;
    Silly silly(N, S);
    silly.solve();
    const string& sol = silly.get_solution();
    if (N < 20)
    {
        Silly silly_naive(N, S);
        silly_naive.solve_naive();
        const string& sol_naive = silly_naive.get_solution();
        if (sol != sol_naive)
        {
            cerr << "Failed\n"
                "test specific " << base << ' ' << N << ' ' << S << '\n';
            rc = 1;
        }
    }
    return rc;
}

static int test_specific(int argc, char ** argv)
{
    u_t base = strtoul(argv[1], 0, 0);
    u_t N = strtoul(argv[2], 0, 0);
    string S(argv[3]);
    return test_case(N, S, base);
}

static int test_random(int argc, char ** argv)
{
    int rc = 0;
    int ai = 1;
    u_t base = strtoul(argv[ai++], 0, 0);
    u_t n_tests = strtoul(argv[ai++], 0, 0);
    u_t n_min = strtoul(argv[ai++], 0, 0);
    u_t n_max = strtoul(argv[ai++], 0, 0);
    for (u_t ti = 0; (rc == 0) && (ti < n_tests); ++ti)
    {
        cerr << "Tested: " << ti << '/' << n_tests << '\n';
        u_t N = n_min + (n_min < n_max ? rand() % (n_max - n_min) : 0);
        string S;
        while (S.size() < N)
        {
            if (rand() % 2)
            {
                S.push_back('0' + (rand() % base));
            }
            else
            {
                for (char c: "013579")
                {
                    if (S.size() < N)
                    {
                        if (c - '0' < int(base)) { S.push_back(c); }
                    }
                }
            }
        }
        rc = test_case(N, S, base);
    }
    return rc;
}

static int test_slow(int argc, char ** argv)
{
    u_t N = strtoul(argv[2], 0, 0);
    dbg_flags = 0x2 | (N < 40 ? 0x1 : 0);
    string S; S.reserve(N);
    S.push_back('0');
    while (S.size() < N)
    {
        S.push_back('1');
        S.push_back('3');
        S.push_back('5');
        S.push_back('7');
        S.push_back('9');
    }
    return test_case(N, S, 10);
}


static int test(int argc, char ** argv)
{
    dbg_flags = 0x1;
    int rc = ((argc > 1) && (string(argv[1]) == string("specific"))
        ? test_specific(argc - 1, argv + 1)
        : ((argc > 1) && (string(argv[1]) == string("slow"))
            ? test_slow(argc, argv)
            : test_random(argc, argv)));
    return rc;
}

int main(int argc, char **argv)
{
    int rc = ((argc > 1) && (string(argv[1]) == string("test"))
        ? test(argc - 1, argv + 1)
        : real_main(argc, argv));
    return rc;
}
