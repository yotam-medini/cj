// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

// #include <algorithm>
#include <fstream>
#include <iostream>
// #include <iterator>
// #include <map>
#include <unordered_map>
#include <set>
#include <string>
#include <array>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;

static unsigned dbg_flags;

typedef vector<u_t> vu_t;
typedef vector<vu_t> vvu_t;
typedef vector<ull_t> vull_t;
typedef array<u_t, 2> au2_t;
typedef array<ull_t, 2> aull2_t;
typedef vector<au2_t> vau2_t;

class Hash_AU2 {
 public:
  size_t operator()(const au2_t& a) const {
    int n = a[0] ^ (a[1] << 7);
    return hash_uint(n);
  }
 private:
  hash<u_t> hash_uint;
};

typedef unordered_map<au2_t, aull2_t, Hash_AU2> au2toull_t;

static const ull_t MOD_BIG = 1000000000 + 7; // 1000000007
static const ull_t INV_MOD_MILLION = 142857001;

static ull_t div_million(ull_t x)
{
    ull_t ret = (INV_MOD_MILLION * x) % MOD_BIG;
    return ret;
}

class Event
{
 public:
    Event(int p=(-1), ull_t _A=0, ull_t _B=0) :
        parent(p), 
        A(_A), B(_B), 
        arep(div_million(A)), brep(div_million(B)) {}
    int parent;
    ull_t A, B;
    ull_t arep, brep;
};
typedef vector<Event> vevent_t;

class DepEvents
{
 public:
    DepEvents(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    ull_t query(const au2_t& q);
    ull_t probabiliy_assuming(u_t ei, u_t assumed, bool occur);
    void eprobs();
    void set_deps();
    u_t N, Q;
    u_t K;
    vevent_t events;
    vau2_t queries;
    vull_t solution;
    vvu_t deps; // indexed from-0
    vull_t probs; // indexed from-0
    au2toull_t memo;
};

DepEvents::DepEvents(istream& fi)
{
    fi >> N >> Q;
    fi >> K;
    events.reserve(N);
    events.push_back(Event(-1, K, K));
    for (u_t i = 1; i < N; ++i)
    {
        u_t parent, A, B;
        fi >> parent >> A >> B;
        events.push_back(Event(parent, A, B));
    }
    queries.reserve(Q);
    for (u_t i = 0; i < Q; ++i)
    {
        au2_t q;
        fi >> q[0] >> q[1];
        queries.push_back(q);
    }
    
}

void DepEvents::solve_naive()
{
    set_deps();
    eprobs();
    for (const au2_t& q: queries)
    {
        solution.push_back(query(q));
    }
}

ull_t DepEvents::query(const au2_t& query_base_1)
{
    const au2_t query{query_base_1[0] - 1, query_base_1[1] - 1};
    au2_t ancestors(query);
    while (ancestors[0] != ancestors[1])
    {
        u_t i = int(ancestors[0] < ancestors[1]);
        const Event& event = events[ancestors[i]];
        ancestors[i] = event.parent - 1;
    }
    const u_t icommon = ancestors[0];
    if (dbg_flags & 0x1) {
        cerr << "("<<query[0]<<","<<query[1]<<")"<< ", common="<<icommon << '\n';
    }
    const ull_t p_ancestor = probs[icommon];
    const ull_t pnot_ancestor = ((MOD_BIG + 1) - p_ancestor) % MOD_BIG;

    aull2_t prob_if_ancestor, prob_ifnot_ancestor;
    for (u_t i: {0, 1})
    {
        prob_if_ancestor[i] = probabiliy_assuming(query[i], icommon, true);
        prob_ifnot_ancestor[i] = probabiliy_assuming(query[i], icommon, false);
    }
    ull_t p_if = (prob_if_ancestor[0] * prob_if_ancestor[1]) % MOD_BIG;
    p_if = (p_if * p_ancestor) % MOD_BIG;
    ull_t p_ifnot = (prob_ifnot_ancestor[0] * prob_ifnot_ancestor[1]) % MOD_BIG;
    p_ifnot = (p_ifnot * pnot_ancestor) % MOD_BIG;
    ull_t p = (p_if + p_ifnot) % MOD_BIG;
    
    return p;
}

void DepEvents::eprobs()
{
    probs = vull_t(size_t(N), 0);
    probs[0] = (K * INV_MOD_MILLION) % MOD_BIG;
    set<u_t> queue;
    for (u_t dependant: deps[0])
    {
        queue.insert(queue.end(), dependant);
    }
   
    while (!queue.empty())
    {
        u_t ei = *(queue.begin());
        queue.erase(queue.begin());
        const Event& event = events[ei];
        u_t pi = event.parent - 1;
        ull_t comp_prob = (1 + (MOD_BIG - probs[pi])) % MOD_BIG;
        ull_t m = (probs[pi] * event.A + comp_prob * event.B) % MOD_BIG;
        m = div_million(m);
        probs[ei] = m;
        if (dbg_flags & 0x1) { cerr << "Prob["<<ei<<"] = " << m << '\n'; }
        for (u_t dependant: deps[ei])
        {
            queue.insert(queue.end(), dependant);
        }
    }
}

void DepEvents::set_deps()
{
    deps = vvu_t{size_t(N), vu_t()};
    for (u_t i = 1; i < N; ++i)
    {
        const Event& event = events[i];
        deps[event.parent - 1].push_back(i);
    }
}

// Assuming ei is descendent of assumed
ull_t DepEvents::probabiliy_assuming(u_t ei, u_t assumed, bool occur)
{
    const au2toull_t::key_type key{ei, assumed};
    au2toull_t::iterator iter = memo.find(key);
    if (iter == memo.end())
    {
        const Event& e = events[ei];
        aull2_t prob2{0, 1};
        if (assumed != ei)
        {
            for (u_t i: {0, 1})
            {
                bool b(i);
                ull_t pp = probabiliy_assuming(e.parent - 1, assumed, b);
                prob2[i] = (e.arep*pp + e.brep*(MOD_BIG + 1 - pp)) % MOD_BIG;
            }
        }
        
        iter = memo.insert(memo.end(), au2toull_t::value_type{key, prob2});
    }
    const au2toull_t::value_type& v = *iter;
    const u_t ioccur = u_t(occur);
    ull_t ret = v.second[ioccur];
    return ret;
}

void DepEvents::solve()
{
    solve_naive();
}

void DepEvents::print_solution(ostream &fo) const
{
    for (const ull_t r: solution)
    {
        fo << ' ' << r;
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

    void (DepEvents::*psolve)() =
        (naive ? &DepEvents::solve_naive : &DepEvents::solve);
    if (solve_ver == 1) { psolve = &DepEvents::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        DepEvents depevents(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (depevents.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        depevents.print_solution(fout);
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
