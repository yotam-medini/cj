// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
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
    DepEvents(u_t _K, const vevent_t& e, const vau2_t& q) :
        N(e.size()), Q(q.size()), K(_K),
        events(e), queries(q)
        {}
    void solve_naive();
    void solve();
    const vull_t& get_solution() const { return solution; }
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

typedef long long ll_t;

class Frac
{
 public:
    Frac(ull_t _n=0, ull_t _d=1, int _sign=1): n(_n), d(_d),
       sign(_sign == 1 ? 1 : -1) 
    {
        reduce();
    }
    ull_t numerator() const { return n; }
    ull_t denominator() const { return d; }
    double dbl() const 
    { 
        double ret = double(n) / double(d);
        if (sign != 1) { ret = -ret; }
        return ret;
    }
    static Frac& add(Frac& res, const Frac& q0, const Frac& q1)
    {
        ll_t rn = q0.snum() * q1.sdenom() + q1.snum() * q0.sdenom();
        int rs = 1;
        if (rn < 0)
        {
            rn = -rn;
            rs = -1;
        }
        res = Frac(rn, q0.d * q1.d, rs);
        return res;
    };
    static Frac& sub(Frac& res, const Frac& q0, const Frac& q1)
    {
        Frac minus_q1;
        mult(minus_q1, minus_one, q1);
        return add(res, q0, minus_q1);
    };
    static Frac& mult(Frac& res, const Frac& q0, const Frac& q1)
    {
        res = Frac(q0.n * q1.n, q0.d * q1.d, q0.sign * q1.sign);
        return res;
    };
    static Frac& div(Frac& res, const Frac& q0, const Frac& q1)
    {
        res = Frac(q0.n * q1.d, q0.d * q1.n, q0.sign * q1.sign);
        return res;
    };
    bool strset(const string& s);
    string str() const;
    static const Frac zero, one, minus_one;
 private:
    static ull_t gcd(ull_t x0, ull_t x1);
    ll_t snum() const
    {
        ll_t ret = n;
        if (sign != 1) { ret = -ret; }
        return ret;
    }
    ll_t sdenom() const { return d; }
    void reduce()
    {
        ull_t g = gcd(n, d);
        n /= g;
        d /= g;
    }
    ull_t n, d;
    int sign;
};

const Frac Frac::zero(0);
const Frac Frac::one(1);
const Frac Frac::minus_one(1, 1, -1);

ull_t Frac::gcd(ull_t x0, ull_t x1)
{
    while (x1 != 0)
    {
        ull_t r = x0 % x1;
        x0 = x1;
        x1 = r;
    }
    return x0;
}

bool Frac::strset(const string& s)
{
    bool ok = true;
    string sub(s);
    sign = 1;
    if (sub[0] == '-')
    {
        sign = -1;
        sub = s.substr(1);
    }
    size_t pnext;
    n = stoi(sub, &pnext);
    d = 1;
    if (sub[pnext] == '/')
    {
        ++pnext;
        sub = sub.substr(pnext);
        d = stoi(sub);
    }
    ok = (d > 0);
    if (ok) { reduce(); }
    return ok;
}

string Frac::str() const
{
    ostringstream os;
    os << '(' << (sign == 1 ? "" : "-") << n;
    if (d != 1)
    {
         os << '/' << d;
    }
    os << ')';
    string ret = os.str();
    return ret;
}

static bool deps_next(vu_t& deps)
{
    bool next = false;
    for (size_t i = deps.size() - 1; (i > 0) && !next; --i)
    {
        u_t v1 = deps[i] + 1;
        if (v1 < i)
        {
            next = true;
            deps[i] = v1;
            fill(deps.begin() + i + 1, deps.end(), 0); // zero tail
        }
    }
    return next;
}

static bool fracs_next(vu_t& indices, u_t n_idx)
{
    bool next = false;
    for (size_t i = indices.size(); (i > 0) && !next;)
    {
        --i;
        u_t v1 = indices[i] + 1;
        if (v1 < n_idx)
        {
            next = true;
            indices[i] = v1;
            fill(indices.begin() + i + 1, indices.end(), 0); // zero tail
        }
    }
    return next;
}

typedef vector<Frac> vfrac_t;

class TestCase
{
 public:
    TestCase(
        const vu_t& _deps,
        const Frac& _k_prob,
        const vfrac_t& _a_probs, 
        const vfrac_t& _b_probs
    ) :
        deps(_deps), k_prob(_k_prob), a_probs(_a_probs), b_probs(_b_probs) 
    {}
    int test_uv(const u_t u, const u_t v);
    u_t N() const { return deps.size(); }
    ostream& show(ostream& os) const;
 private:
    TestCase(const TestCase&);
    TestCase& operator=(const TestCase&);
    u_t uv_ancestor(const au2_t uv) const;
    const Frac& probabiliy_of(Frac& prob, u_t x) const;
    const Frac& probabiliy_assuming(Frac& prob, u_t x, u_t assumed, bool occur)
        const;
    u_t frac2mil(const Frac& f) const 
    { 
        return (1000000*f.numerator()) / f.denominator();
    }
    const vu_t& deps;  // deps[0] ignored
    const Frac& k_prob;
    const vfrac_t& a_probs;
    const vfrac_t& b_probs;
};

u_t TestCase::uv_ancestor(const au2_t uv) const
{
    au2_t ancestors; // masks
    for (u_t i: {0, 1})
    {
        ancestors[i] = 0;
        for (u_t a = uv[i]; a != 0; a = deps[a])
        {
            ancestors[i] |= (1u << a);
        }
    }
    u_t papa = 0;
    const u_t ancestors_common = ancestors[0] & ancestors[1];
    const u_t uv_min = min(uv[0], uv[1]);
    for (u_t x = 0; x <= uv_min; ++x)
    {
        if (ancestors_common & (1u << x))
        {
            papa = x;
        }
    }
    return papa;
}

int TestCase::test_uv(const u_t u, const u_t v)
{
    int rc = 0;
    au2_t uv{u, v};
    u_t ancestor = uv_ancestor(uv);
    Frac prob_ancestor, prob_not_ancestor;
    probabiliy_of(prob_ancestor, ancestor);
    Frac::sub(prob_not_ancestor, Frac::one, prob_ancestor);
    Frac uv_assuming_ancestor[2];
    Frac uv_assuming_not_ancestor[2];
    for (u_t i: {0, 1})
    {
        probabiliy_assuming(uv_assuming_ancestor[i], uv[i], ancestor, true);
        probabiliy_assuming(uv_assuming_not_ancestor[i], uv[i], ancestor, false);
    }
    Frac prob_uv_assuming_ancestor, prob_uv_assuming_not_ancestor;
    Frac prob_uv_if_ancestor, prob_uv_if_not_ancestor;
    Frac::mult(prob_uv_assuming_ancestor,
        uv_assuming_ancestor[0], uv_assuming_ancestor[1]);
    Frac::mult(prob_uv_if_ancestor, prob_uv_assuming_ancestor,
        prob_ancestor);
    Frac::mult(prob_uv_assuming_not_ancestor,
        uv_assuming_not_ancestor[0], uv_assuming_not_ancestor[1]);
    Frac::mult(prob_uv_if_not_ancestor, prob_uv_assuming_not_ancestor,
         prob_not_ancestor);
    Frac uv_probability;
    Frac::add(uv_probability, prob_uv_if_ancestor, prob_uv_if_not_ancestor);
    if (N() <= 3)
    {
        cerr << "  Prob(" << u << ", " << v << ") = " << uv_probability.str() <<
            '\n';
    }
    u_t _K = frac2mil(k_prob);
    vevent_t events;
    events.push_back(Event(-1, _K, _K));
    for (u_t i = 0; i < N() - 1; ++i)
    {
        const Event e(deps[i + 1] + 1, 
            frac2mil(a_probs[i]), frac2mil(b_probs[i]));
        events.push_back(e);
    }
    
    vau2_t q; q.push_back(au2_t{u + 1, v + 1});
    DepEvents dep_events(_K, events, q);
    dep_events.solve();
    const vull_t& solution = dep_events.get_solution();
    if (solution.size() != 1)
    {
        rc = 1;
    }
    else
    {
        ull_t solution0 = solution[0];
        ull_t unreduce = 1000000 / uv_probability.denominator();
        ull_t uv_prob_bigmod_rep = 
            (unreduce * uv_probability.numerator() * INV_MOD_MILLION) %
            MOD_BIG;
        if (solution0 != uv_prob_bigmod_rep)
        {
            rc = 1;
        }
    }
    return rc;
}

const Frac& TestCase::probabiliy_of(Frac& prob, u_t x) const
{
    if (x == 0)
    {
        prob = k_prob;
    }
    else
    {
        u_t papa = deps[x];
        Frac p, notp, a_p, b_notp;
        Frac::sub(notp, Frac::one, probabiliy_of(p, papa));
        Frac::mult(a_p, a_probs[x - 1], p);
        Frac::mult(b_notp, b_probs[x - 1], notp);
        Frac::add(prob, a_p, b_notp);
    }
    return prob;
}

const Frac& TestCase::probabiliy_assuming(
    Frac& prob, 
    u_t x, 
    u_t assumed,
    bool occur) const
{
    if (x == assumed)
    {
        prob = occur ? Frac::one : Frac::zero;
    }
    else
    {
        u_t papa = deps[x];
        Frac p, notp, a_p, b_notp;
        Frac::sub(notp, Frac::one, probabiliy_assuming(p, papa, assumed, occur));
        Frac::mult(a_p, a_probs[x - 1], p);
        Frac::mult(b_notp, b_probs[x - 1], notp);
        Frac::add(prob, a_p, b_notp);
    }
    return prob;
}

ostream& TestCase::show(ostream& os) const
{
    const char* sep = "";
    os << "{ N=" << N() << "\n"
        "   deps = [ "; sep = "";
    for (const Frac& f: deps) { os << sep << f.str(); sep = ", "; }
    os << "]\n"
        "   K = " << k_prob.str() << "\n"
        "   a_probs = ["; sep = "";
    for (const Frac& f: a_probs) { os << sep << f.str(); sep = ", "; }
    os << "]\n"
        "   b_notp = ["; sep = "";
    for (const Frac& f: b_probs) { os << sep << f.str(); sep = ", "; }
    os << "]\n}\n";
        
    return os;
}

static int test_specific(int argc, char ** argv)
{
    int rc = 0;
    int ai = 2;
    u_t N = strtoul(argv[++ai], 0, 0);
    vu_t deps;
    deps.push_back(0);
    while (deps.size() < N)
    {
        deps.push_back(strtoul(argv[++ai], 0, 0));
    }
    vfrac_t a_probs, b_probs;
    Frac k_prob, f;
    k_prob.strset(argv[++ai]);
    while (a_probs.size() < N - 1)
    {
        f.strset(argv[++ai]);
        a_probs.push_back(f);
    }
    while (b_probs.size() < N - 1)
    {
        f.strset(argv[++ai]);
        b_probs.push_back(f);
    }
    u_t u = strtoul(argv[++ai], 0, 0);
    u_t v = strtoul(argv[++ai], 0, 0);
    TestCase tc(deps, k_prob, a_probs, b_probs);
    tc.show(cerr);
    rc = tc.test_uv(u, v);
    return rc;
}

static int test_probs(
    const vu_t& deps,
    const Frac& k_prob,
    const vfrac_t& a_probs, 
    const vfrac_t& b_probs)
{
    int rc = 0;
    TestCase tc(deps, k_prob, a_probs, b_probs);
    const u_t N = tc.N();
    if (N <= 3)
    {
        tc.show(cerr);
    }
    for (u_t u = 0; (rc == 0) && (u < N); ++u)
    {
        for (u_t v = u + 1; (rc == 0) && (v < N); ++v)
        {
            rc = tc.test_uv(u, v);
        }
    }
    return rc;
}

static int test_all(int argc, char ** argv)
{
    int rc = 0;
    int ai = 2;
    const u_t n_max = strtoul(argv[ai++], 0, 0);
    vfrac_t fracs10;
    const u_t denom = 10;
    while (fracs10.size() < denom + 1)
    {
         fracs10.push_back(Frac(fracs10.size(), denom));
    }
    vfrac_t fracs_few;
    fracs_few.push_back(Frac(0));
    fracs_few.push_back(Frac(1, 10));
    fracs_few.push_back(Frac(1, 2));
    fracs_few.push_back(Frac(3, 5));
    fracs_few.push_back(Frac(1));
    for (u_t N = 2; N <= n_max; ++N)
    {
        const vfrac_t& fracs = (N <= 2 ? fracs10 : fracs_few);
        cerr << "N: " << N << '/' << n_max << '\n';
        ull_t ti = 0, di = 0;
        vu_t deps = vu_t(size_t(N), 0);
        vfrac_t a_probs(size_t(N - 1), Frac(0));
        vfrac_t b_probs(size_t(N - 1), Frac(0));
        for (bool deps_more = true; (rc == 0 && deps_more);
            deps_more = deps_next(deps), ++di)
        {
            if ((di & (di - 1)) == 0) {
                    cerr << "N="<<N << ", di="<<di << '\n'; }
            vu_t fracs_indices = vu_t(2*(N - 1) + 1, 0);
            for (bool fracs_more = true; (rc == 0) && fracs_more;
                fracs_more = fracs_next(fracs_indices, fracs.size()), ++ti)
            {
                if ((ti & (ti - 1)) == 0) {
                    cerr << "N="<<N << ", tested: "<<ti << '\n'; }
                u_t fi = 0;
                const Frac k_prob = fracs[fracs_indices[0]];
                for (u_t ei = 0; ei < N - 1; ++ei)
                {
                    a_probs[ei] = fracs[fracs_indices[++fi]];
                    b_probs[ei] = fracs[fracs_indices[++fi]];
                }
                rc = test_probs(deps, k_prob, a_probs, b_probs);
            }
        }
    }
    return rc;
}

static int test(int argc, char ** argv)
{
    int rc = ((argc > 2) && (string(argv[2]) == string("specific"))
        ? test_specific(argc, argv)
        : test_all(argc, argv));
    return rc;
}

int main(int argc, char **argv)
{
    int rc = ((argc > 1) && (string(argv[1]) == string("test"))
        ? test(argc, argv)
        : real_main(argc, argv));
    return rc;
}
