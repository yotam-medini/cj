// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <queue>
#include <unordered_set>
#include <utility>
#include <tuple>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
typedef vector<ull_t> vull_t;
typedef pair<ull_t, vu_t> ull_vu_t;
// typedef priority_queue<ull_vu_t, vector<ull_vu_t>, greater<ull_vu_t>> pq_t;

static unsigned dbg_flags;

class Node
{
 public:
    typedef tuple<ull_t, vu_t, ull_t> t3_t;
    Node(ull_t _prod=1, const vu_t& _powers=vu_t(), ull_t _left=0) :
        t3{_prod, _powers, _left}
    {}
    ull_t& prod()   { return get<0>(t3); }
    vu_t&  powers() { return get<1>(t3); }
    ull_t& left()   { return get<2>(t3); }

    ull_t       prod()   const { return get<0>(t3); }
    const vu_t& powers() const { return get<1>(t3); }
    ull_t       left()   const { return get<2>(t3); }

    static bool lt(const Node& node0, const Node& node1)
    {
        bool ret = node0.t3 < node1.t3;
        return ret;
    }
 private:
    t3_t t3;
};

bool operator<(const Node& node0, const Node& node1)
{
    return Node::lt(node0, node1);
}
        
bool operator>(const Node& node0, const Node& node1)
{
    return Node::lt(node1, node0);
}
        
class Prime
{
 public:
    Prime(istream& fi);
    Prime(const vu_t& _P, const vull_t& _N) :
        M(_P.size()), P(_P), N(_N), solution(0) {}
    void solve_naive();
    void solve_pq();
    void solve_bt();
    void solve();
    void print_solution(ostream&) const;
    ull_t get_solution() const { return solution; }
 private:
    void backtrace(ull_t prod, ull_t pi_max, vu_t& powers, vu_t& pi_seq, 
                   ull_t left);
    void check_possible(const ull_t cand, const ull_t sum_all);
    u_t M;
    vu_t P;
    vull_t N;
    ull_t solution;
};

Prime::Prime(istream& fi) : solution(0)
{
    fi >> M;
    P.reserve(M);
    N.reserve(M);
    for (u_t i = 0; i < M; ++i)
    {
        u_t p;
        ull_t n;
        fi >> p >> n;
        P.push_back(p);
        N.push_back(n);
    }
}

void Prime::solve_naive()
{
    vu_t nums;
    for (u_t i = 0; i < M; ++i)
    {
        for (u_t k = 0; k < N[i]; ++k)
        {
            nums.push_back(P[i]);
        }
    }
    const u_t nn = nums.size();
    const u_t mask_max = 1u << nn;
    for (u_t mask = 0; mask < mask_max; ++mask)
    {
        ull_t sum = 0, prod = 1;
        for (u_t i = 0; i < nn; ++i)
        {
            if (((1u << i) & mask) == 0)
            {
                sum += nums[i];
            }
            else
            {
                prod *= nums[i];
            }
        }
        if ((sum == prod) && (solution < sum))
        {
            solution = sum;
        }
    }
}

void Prime::solve() // Using published analysis
{
    u_t max_group2_size = 0;
    ull_t sum_all = 0;
    for (size_t i = 0; i < M; ++i)
    {
        sum_all += (P[i] * N[i]);
    }
    ull_t pp = 1;
    for (size_t i = 0; (i < M) && (pp < sum_all); ++i)
    {
        for (u_t j = 0; (j < N[i]) && (pp < sum_all); ++j)
        {
            pp *= P[i];
            ++max_group2_size;
        }
    }
    ull_t max_group2_sum = 0;
    u_t nc = 0;
    for (int i = M - 1; (i >= 0) && (nc < max_group2_size); --i)
    {
        for (u_t j = 0; (j < N[i]) && (nc < max_group2_size); ++j, ++nc)
        {
            max_group2_sum += P[i];
        }
    }
    for (ull_t cand = sum_all - max_group2_sum; cand < sum_all; ++cand)
    {
        check_possible(cand, sum_all);
    }
}

void Prime::check_possible(const ull_t cand, const ull_t sum_all)
{
    bool possible = true;
    ull_t c = cand;
    ull_t group2_sum = 0;
    for (size_t i = 0; possible && (c > 1) && (i < M); ++i)
    {
        const ull_t prime = P[i];
        ull_t plog = 0;
        while (c % prime == 0)
        {
            ++plog;
            group2_sum += prime;
            c /= prime;
        }
        possible = (plog <= N[i]);
    }
    possible = possible && (c == 1) && (group2_sum + cand == sum_all);
    if (possible)
    {
        solution = cand;
    }
}

void Prime::solve_bt()
{
    ull_t total = 0;
    for (size_t i = 0; i < M; ++i)
    {
        total += P[i]*N[i];
    }
    vu_t powers(M, 0);
    vu_t pi_seq;
    backtrace(1, M - 1, powers, pi_seq, total);
}

void Prime::backtrace(ull_t prod, ull_t pi_max, vu_t& powers, vu_t& pi_seq, 
                      ull_t left)
{
    if (prod <= left)
    {
        if (dbg_flags & 0x1) {
            cerr<<"pt_max="<<pi_max<<", prod="<<prod << ", left="<<left << '\n';}
        if (dbg_flags & 0x2) { cerr << "pi_seq:";
            for (u_t pi: pi_seq) { cerr << ' ' << pi; } cerr << '\n';
        }
        if ((prod == left) && (solution < prod))
        {
            solution = prod;
        }
        else
        {
            const ull_t curr_prime = P[pi_max];
            if ((powers[pi_max] < N[pi_max]) && (curr_prime < left))
            {
                ++powers[pi_max];
                pi_seq.push_back(pi_max);
                backtrace(prod * curr_prime, pi_max, powers, pi_seq, 
                          left - curr_prime);
                pi_seq.pop_back();
                --powers[pi_max];
            }
            if (pi_max > 0)
            {
                backtrace(prod, pi_max - 1, powers, pi_seq, left);
            }
        }
    }
}

void Prime::solve_pq()
{
    typedef priority_queue<Node, vector<Node>, greater<Node>> pq_t;
    typedef unordered_set<ull_t> set_ull_t;
    pq_t pq;
    set_ull_t qset;
    ull_t total = 0;
    for (size_t i = 0; i < M; ++i)
    {
        total += P[i]*N[i];
    }
    {
        vu_t powers_01(M, 0);
        for (size_t i = 0; i < M; ++i)
        {
            vu_t powers(M, 0);
            ull_t left = total - P[i];
            if (P[i] <= left)
            {
                powers_01[i] = 1;
                pq.push(Node(P[i], powers_01, left));
                qset.insert(qset.end(), P[i]);
                powers_01[i] = 0;
            }
        }
    }
    while (!pq.empty())
    {
        const Node& node = pq.top();
        ull_t prod = node.prod();
        const vu_t powers = node.powers(); // copy
        ull_t left = node.left();
        if (dbg_flags & 0x1) {cerr << "prod="<<prod << ", left="<<left << '\n';}
        pq.pop();
        qset.erase(prod);
        if ((prod == left) && (solution < prod))
        {
            solution = prod;
        }
        for (size_t i = 0; i < M; ++i)
        {
            if (powers[i] < N[i])
            {
                const ull_t p_prod = prod*P[i];
                const ull_t p_left = left - P[i];
                if ((p_prod <= p_left) && (qset.find(p_prod) == qset.end()))
                {
                    vu_t p_powers(powers);
                    p_powers[i] += 1;
                    pq.push(Node(p_prod, p_powers, p_left));
                    qset.insert(qset.end(), p_prod);
                }
            }
        }
    }
}

void Prime::print_solution(ostream &fo) const
{
    fo << ' ' << solution;
}

int real_main(int argc, char ** argv)
{
    const string dash("-");

    bool naive = false;
    bool bt = false;
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
        else if (opt == string("-bt"))
        {
            bt = true;
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

    void (Prime::*psolve)() =
        (naive ? &Prime::solve_naive : &Prime::solve);
    if (bt) { psolve = &Prime::solve_bt; }
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Prime prime(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (prime.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        prime.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}

static vu_t primes;

static void eratosthenes()
{
    enum {PMAX = 500};
    vector<bool> sieve(size_t(PMAX), true);
    for (u_t n = 2; n*n < PMAX; ++n)
    {
        if (sieve[n])
        {
            primes.push_back(n);
            for (u_t d = n*n; d < PMAX; d += n)
            {
                sieve[d] = false;
            }
        }
    }
}

static int test_case(const vu_t& P, const vull_t& N)
{
    int rc = 0;

    Prime prime_pq(P, N);
    Prime prime_bt(P, N);
    Prime prime_release(P, N);

    prime_pq.solve_pq();
    prime_bt.solve_bt();
    prime_release.solve();

    ull_t solution_pq = prime_pq.get_solution();
    ull_t solution_bt = prime_bt.get_solution();
    ull_t solution = prime_release.get_solution();

    if ((solution != solution_pq) || (solution != solution_bt))
    {
        rc = 1;
        cerr << "Failed: solution="<<solution <<
            ", bt="<<solution_bt << ", pq="<<solution_pq << '\n';
        ofstream f("prime-fail.in");
        f << "1\n" << P.size() << '\n';
        for (size_t i = 0; i < P.size(); ++i)
        {
            f << P[i] << ' ' << N[i] << '\n';
        }
        f.close();
    }
    return rc;
}

static int test(int argc, char **argv)
{
    int rc = 0;
    eratosthenes();
    const u_t n_primes = primes.size();
    u_t ai = 0;
    u_t nt = stod(argv[ai++]);
    u_t NsumMax = stod(argv[ai++]);
    for (u_t ti = 0; (rc == 0) && (ti < nt); ++ti)
    {
        vu_t P;
        vull_t N;
        u_t Nsum = max<u_t>(rand() % (NsumMax + 1), 2);
        const u_t Nsum0 = Nsum;
        u_t pi = 0;
        for ( ; (pi + 1 < n_primes) && (Nsum > 0); ++pi)
        {
            u_t n = Nsum;
            for (u_t r = 0; r < 3; ++r)
            {
                n = min<u_t>(n, rand() % Nsum);
            }
            if (n > 0)
            {
                P.push_back(primes[pi]);
                N.push_back(n);
                Nsum -= n;
            }
        }
        if (Nsum > 0)
        {
            P.push_back(primes[pi]);
            N.push_back(Nsum);
        }
        cerr << "Tested " << ti << '/' << nt << " M=" << P.size() << 
            ", SN=" << Nsum0 << '\n';
        rc = test_case(P, N);
    }
    return rc;
}

int main(int argc, char **argv)
{
    int rc = ((argc > 1) && (string(argv[1]) == string("test"))
        ? test(argc - 2, argv + 2)
        : real_main(argc, argv));
    return rc;
}
