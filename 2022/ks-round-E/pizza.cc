// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <array>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <limits>
#include <set>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef long long int ll_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
typedef vector<vu_t> vvu_t;
typedef vector<int> vi_t;
typedef vector<vi_t> vvi_t;
typedef array<int, 2> ai2_t;
typedef vector<ai2_t> vai2_t;

static unsigned dbg_flags;

static string hex(ll_t x)
{
    ostringstream oss;
    if (x < 0)
    {
        oss << x;
    }
    else
    {
        oss << "0x" << hex << x;
    }
    return oss.str();
}

ll_t floor_div(ll_t n, int d)
{
    ll_t q = n / d;
    if ((n < 0) && ((-n) % d != 0))
    {
        --q;
    }
    return q;
}

class Op
{
 public:
    Op(char _c='+', u_t _k=0) : c(_c), k(_k) {}
    ll_t eval(ll_t x) const
    { 
        ll_t ret = 0;
        switch (c)
        {
         case '+':
             ret = x + k;
             break;
         case '-':
             ret = x - k;
             break;
         case '*':
             ret = x * k;
             break;
         case '/':
             ret = floor_div(x, k);
             break;
         default:
             cerr << __FILE__ ":" << __LINE__ << " BUG\n";
             ret = 0;
        }
        return ret;
    }
    char c;
    u_t k;
};

class Customer
{
 public:
    Customer(int _x=0, int _y=0, u_t _c=0) : x(_x), y(_y), c(_c) {}
    int x, y;
    u_t c;
};
typedef vector<Customer> vcust_t;

class Pizza
{
 public:
    Pizza(istream& fi);
    Pizza(
        u_t _N, u_t _M,
        u_t _Ar, u_t _Ac, 
        const array<Op, 4>& _ops, 
        const vcust_t& _customers
    ) :
        N(_N), P(_customers.size()), M(_M), 
        Ar(_Ar), Ac(_Ac), ops(_ops), customers(_customers), 
        solution(0)
        {}
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    ll_t get_solution() const { return solution; }
 private:
    typedef unordered_map<ull_t, ll_t> memo_t;
    // enum {North, East, West, South};
    ll_t path_eval(u_t m, const ull_t step_comn, const vvu_t& cust_pis) const;
    ll_t dp(int i, int j, u_t l, u_t deliver_mask);
    ull_t ijldm_to_key(int i, int j, ull_t l, ull_t deliver_mask) const
    {
        // i, j = 10*10, l = 21, deliver_mask=10
        ull_t ij_mask = ((i - 1) << 12) | (j - 1);
        return (deliver_mask << 48) | (l << 24) | ij_mask;
    }
    void key_tp_ijldm(ull_t key, int& i, int& j, u_t& l, u_t& deliver_mask) const
    {
        ull_t ij_mask = key & 0xffffff;
        i = (ij_mask & 0xfff) + 1;
        j = (ij_mask >> 12) + 1;
        ull_t dml_mask = (key >> 24) & 0xfffffffff;
        l = dml_mask & 0xffffff;
        deliver_mask = dml_mask >> 24;
    }
    void set_ij_to_customer();
    u_t N, P, M;
    int Ar, Ac;
    array<Op, 4> ops;
    vcust_t customers;
    ll_t solution;
    memo_t memo;
    vvi_t ij_to_customer;
};

Pizza::Pizza(istream& fi) : solution(0)
{
    fi >> N >> P >> M >> Ar >> Ac;
    for (Op& op: ops)
    {
        string s;
        fi >> s >> op.k;
        op.c = s[0];
    }
    customers.reserve(P);
    while (customers.size() < P)
    {
        Customer customer;
        fi >> customer.x >> customer.y >> customer.c;
        customers.push_back(customer);
    }
}

static void max_by(ll_t& v, ll_t x)
{
    if (v < x)
    {
        v = x;
    }
}

void Pizza::solve_naive()
{
    solution = numeric_limits<ll_t>::min();
    set_ij_to_customer();
    const u_t deliver_target = (1u << P) - 1;
    u_t m_best = 0; ull_t comb_best = 0;
    for (u_t m = 0; m <= M; ++m)
    {
        const ull_t comb_max = 1ull << (2*m);
        for (ull_t step_comb = 0; step_comb < comb_max; ++step_comb)
        {
            vai2_t path;
            vvu_t cust_pis(P, vu_t());
            u_t delivered = 0;
            path.push_back(ai2_t{Ar, Ac});
            bool inside = true;
            ull_t step_comb_shifted = step_comb;
            for (u_t pi = 0; inside && (pi < m); ++pi, step_comb_shifted >>= 2)
            {
                static const ai2_t steps[4] = { // going to
                    ai2_t{-1,  0}, // North
                    ai2_t{ 0,  1}, // East
                    ai2_t{ 0, -1}, // West
                    ai2_t{ 1,  0}  // South
                };
                u_t si = step_comb_shifted & 0x3;
                const ai2_t& step = steps[si];
                const ai2_t& curr = path.back();
                const int i = curr[0] + step[0], j = curr[1] + step[1];
                inside = (0 < min(i, j)) && (max(i, j) <= int(N));
                if (inside)
                {
                    path.push_back(ai2_t{i, j});
                    int icust = ij_to_customer[i][j];
                    if (icust >= 0)
                    {
                        cust_pis[icust].push_back(pi);
                        delivered |= (1u << icust);
                    }
                }
            }
            if (inside && (deliver_target == delivered))
            {
                ll_t candidate = path_eval(m, step_comb, cust_pis);
                max_by(solution, candidate);
                if (solution == candidate)
                {
                    m_best = m; comb_best = step_comb;
                }
            }
        }
    }
    if (dbg_flags & 0x1) { cerr << "Best path: ";
        ull_t step_comb_shifted = comb_best;
        for (u_t pi = 0; pi < m_best; ++pi, step_comb_shifted >>= 2) {
            u_t si = step_comb_shifted & 0x3;
            cerr << "NEWS"[si];
        }
        cerr << '\n';
    }
}

ll_t Pizza::path_eval(u_t m, const ull_t step_comn, const vvu_t& cust_pis) const
{
    ll_t best = numeric_limits<ll_t>::min();
    u_t n_cust_deliver = 1;
    for (const vu_t& cust_pi: cust_pis)
    {
        n_cust_deliver *= cust_pi.size();
    }
    for (u_t deliver_comb = 0; deliver_comb < n_cust_deliver; ++deliver_comb)
    {
        // Set delivery time in path;
        u_t dc_shifted = deliver_comb;
        vector<int> pi_deliver(m, -1);
        for (u_t ci = 0; ci < P; ++ci)
        {
            const vu_t cust_pi = cust_pis[ci];
            const u_t n_del_opt = cust_pi.size();
            const u_t iopt = deliver_comb % n_del_opt;
            u_t pi = cust_pi[iopt];
            pi_deliver[pi] = ci;
            dc_shifted /= n_del_opt;
        }

        ll_t c = 0;
        ull_t step_comn_shifted = step_comn;
        for (u_t pi = 0; pi < m; ++pi, step_comn_shifted >>= 2)
        {
            u_t si = step_comn_shifted & 0x3;
            c = ops[si].eval(c);
            int ci = pi_deliver[pi];
            if (ci >= 0)
            {
                c += customers[ci].c;
            }
        }
        max_by(best, c);
    }
    return best;
}

void Pizza::solve()
{
    set_ij_to_customer();
    const u_t all_delivered = (1u << P) - 1;
    solution = numeric_limits<ll_t>::min();
    for (u_t m = P; m <= M; ++m)
    {
        for (int i = 1; i <= int(N); ++i)
        {
            for (int j = 1; j <= int(N); ++j)
            {
                ll_t candidate = dp(i, j, m, all_delivered);
                max_by(solution, candidate);
            }
        }
    }
}

void Pizza::set_ij_to_customer()
{
    ij_to_customer = vvi_t(N + 1, vi_t(N + 1, -1));
    for (u_t ci = 0; ci < P; ++ci)
    {
        const Customer& customer = customers[ci];
        ij_to_customer[customer.x][customer.y] = ci;
    }
}

ll_t Pizza::dp(int i, int j, u_t l, u_t deliver_mask)
{
    static const ll_t min_infty = numeric_limits<ll_t>::min();
    ll_t ret = min_infty;
    ull_t key = ijldm_to_key(i, j, l, deliver_mask);
    memo_t::iterator iter = memo.find(key);
    if (iter == memo.end())
    {
        if (l == 0)
        {
            if ((deliver_mask == 0) && (i == Ar) && (j == Ac))
            {
                ret = 0;
            }
        }
        else
        {
            static const ai2_t steps[4] = { // coming from
                ai2_t{ 1,  0}, // North
                ai2_t{ 0, -1}, // East
                ai2_t{ 0,  1}, // West
                ai2_t{-1,  0}  // South
            };
            u_t deliver_bit = 0;
            int ci = ij_to_customer[i][j];
            if (ci >= 0)
            {
                deliver_bit = (1u << ci) & deliver_mask;
            }
            for (u_t iop = 0; iop < 4; ++iop)
            {
                const ai2_t& step = steps[iop];
                int i_pre = i + step[0], j_pre = j + step[1];
                if ((0 < i_pre) && (i_pre <= int(N)) &&
                    (0 < j_pre) && (j_pre <= int(N)))
                {
                    ll_t ret_pre = dp(i_pre, j_pre, l - 1, deliver_mask);
                    if (ret_pre != min_infty)
                    {
                        ret_pre = ops[iop].eval(ret_pre);
                        max_by(ret, ret_pre);
                    }
                    if (deliver_bit)
                    {
                        u_t mask = deliver_mask & (~deliver_bit);
                        ret_pre = dp(i_pre, j_pre, l - 1, mask);
                        if (ret_pre != min_infty)
                        {
                            ret_pre = ops[iop].eval(ret_pre);
                            ret_pre += customers[ci].c;
                            max_by(ret, ret_pre);
                        }
                    }
                }
            }
        }
        if (dbg_flags & 0x1) { cerr << "i="<<i << ", j="<<j << ", l="<<l <<
            ", deliver_mask="<<hex(deliver_mask) << " --> " << ret << '\n'; }
        memo.insert(iter, memo_t::value_type{key, ret});
    }
    else
    {
        ret = iter->second;
    }
    return ret;
}

void Pizza::print_solution(ostream &fo) const
{
    if (solution == numeric_limits<ll_t>::min())
    {
        fo << " IMPOSSIBLE";
    }
    else
    {
        fo << ' ' << solution;
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

    void (Pizza::*psolve)() =
        (naive ? &Pizza::solve_naive : &Pizza::solve);
    if (solve_ver == 1) { psolve = &Pizza::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Pizza pizza(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (pizza.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        pizza.print_solution(fout);
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
    const u_t N, 
    const u_t M,
    const u_t Ar,
    const u_t Ac,
    const array<Op, 4>& ops, 
    const vcust_t& customers)
{
    const u_t P = customers.size();
    ofstream f(fn);
    f << "1\n" << N << ' ' << P << ' ' << M <<
        ' ' << Ar << ' ' << Ac << '\n';
    for (const Op& op: ops)
    {
        f << op.c << ' ' << op.k << '\n';
    }
    for (const Customer& customer: customers)
    {
        f << customer.x << ' ' << customer.y << ' ' << customer.c << '\n';
    }
    f.close();
}

static int test_case(
    const u_t N, 
    const u_t M,
    const u_t Ar,
    const u_t Ac,
    const array<Op, 4>& ops, 
    const vcust_t& customers)
{
    const u_t P = customers.size();
    int rc = 0;
    ll_t solution(-1), solution_naive(-1);
    save_case("pizza-curr.in", N, M, Ar, Ac, ops, customers);
    bool small = (N <= 4) && (P <= 4) && (M <= 8);
    if (small)
    {
        Pizza p(N, M, Ar, Ac, ops, customers);
        p.solve_naive();
        solution_naive = p.get_solution();
    }
    {
        Pizza p(N, M, Ar, Ac, ops, customers);
        p.solve();
        solution = p.get_solution();
    }
    if (small && (solution != solution_naive))
    {
        rc = 1;
        cerr << "Failed: solution="<<solution << " != " <<
            solution_naive << " = solution_naive\n";
        save_case("pizza-fail.in", N, M, Ar, Ac, ops, customers);
    }
    if (rc == 0) {
        cerr << N << ' ' << P << ' ' << M << " --> " <<
            (small ? "(small) " : "(large) ");
        if (solution == numeric_limits<ll_t>::min()) {
            cerr << "IMPOSSIBLE";
        } else {
            cerr << solution;
        }
        cerr << '\n';
    }
    return rc;
}

static int test_random(int argc, char ** argv)
{
    int rc = 0;
    int ai = 0;
    u_t n_tests = strtoul(argv[ai++], 0, 0);
    u_t Nmin = strtoul(argv[ai++], 0, 0);
    u_t Nmax = strtoul(argv[ai++], 0, 0);
    u_t Pmax = strtoul(argv[ai++], 0, 0);
    u_t Mmin = strtoul(argv[ai++], 0, 0);
    u_t Mmax = strtoul(argv[ai++], 0, 0);
    cerr << 
        "Nmin=" << Nmin << ", Nmax=" << Nmax << 
        ", Pmax=" << Pmax << 
        ", Mmin=" << Mmin << ", Mmax=" << Mmax << 
        '\n';
    for (u_t ti = 0; (rc == 0) && (ti < n_tests); ++ti)
    {
        cerr << "Tested: " << ti << '/' << n_tests << '\n';
        u_t N = rand_range(Nmin, Nmax);
        u_t P = rand_range(0, min(Pmax, N*N - 1));
        u_t M = rand_range(Mmin, Mmax);
        int Ar = rand_range(1, N);
        int Ac = rand_range(1, N);
        array<Op, 4> ops;
        for (u_t i = 0; i < 4; ++i)
        {
            ops[i].c = "+-*/"[rand() % 4];
            ops[i].k = rand_range(1, 4);
        }
        set<ai2_t> ij_set;
        set<ai2_t>::iterator iter0 = ij_set.insert(ij_set.end(), ai2_t{Ar, Ac});
        while (ij_set.size() < P + 1)
        {
            int x = rand_range(1, N);
            int y = rand_range(1, N);
            ij_set.insert(ij_set.end(), ai2_t{x, y});
        }
        ij_set.erase(iter0);
        vcust_t customers; customers.reserve(P);
        for (const ai2_t& xy: ij_set)
        {
            u_t c = rand_range(1, 4);
            customers.push_back(Customer(xy[0], xy[1], c));
        }
        rc = test_case(N, M, Ar, Ac, ops, customers);
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
