// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <array>
#include <fstream>
#include <iostream>
#include <limits>
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
typedef vector<int> vi_t;
typedef vector<vi_t> vvi_t;
typedef array<int, 2> ai2_t;

static unsigned dbg_flags;

class Op
{
 public:
    Op(char _c='+', u_t _k=0) : c(_c), k(_k) {}
    ll_t eval(ull_t x) const
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
             ret = x / k;
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
    Pizza(const vu_t&) {}; // TBD for test_case
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    ull_t get_solution() const { return 0; }
 private:
    typedef unordered_map<ull_t, ll_t> memo_t;
    enum {North, East, West, South};
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

void Pizza::solve_naive()
{
    set_ij_to_customer();
    const u_t all_delivered = (1u << P) - 1;
    if (P > 0)
    {
        solution = numeric_limits<ll_t>::min();
    }
    for (int i = 1; i <= int(N); ++i)
    {
        for (int j = 1; j <= int(N); ++j)
        {
            ll_t candidate = dp(i, j, M, all_delivered);
            if (solution < candidate)
            {
                solution = candidate;
            }
        }
    }
}

void Pizza::solve()
{
    solve_naive();
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
            ret = (deliver_mask == 0 ? 0 : min_infty);
        }
        else
        {
            static const ai2_t steps[4] = { // coming from
                ai2_t{ 1,  0}, // North
                ai2_t{ 0, -1}, // East
                ai2_t{ 0,  1}, // West
                ai2_t{-1,  0}  // South
            };
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
                        if (ret < ret_pre)
                        {
                            ret = ret_pre;
                        }
                    }
                }
            }
            int ci = ij_to_customer[i][j];
            if (ci > 0)
            {
                u_t bit = (1u << ci);
                if (deliver_mask & bit)
                {
                    u_t mask = deliver_mask & (~bit);
                    ll_t ret_pre_pizza = dp(i, j, l - 1, mask);
                    if (ret < ret_pre_pizza)
                    {
                        ret = ret_pre_pizza;
                    }
                }
            }
        }
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
    if (solution < 0)
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

static int test_case(int argc, char ** argv)
{
    int rc = rand_range(0, 1);
    ull_t solution(-1), solution_naive(-1);
    bool small = rc == 0;
    if (small)
    {
        Pizza p{vu_t()};
        p.solve_naive();
        solution_naive = p.get_solution();
    }
    {
        Pizza p{vu_t()};
        p.solve();
        solution = p.get_solution();
    }
    if (small && (solution != solution_naive))
    {
        rc = 1;
        cerr << "Failed: solution="<<solution << " != " <<
            solution_naive << " = solution_naive\n";
        ofstream f("pizza-fail.in");
        f << "1\n";
        f.close();
    }
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
        rc = test_case(argc, argv);
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
